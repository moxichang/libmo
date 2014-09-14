#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>

#include <string>
#include <sstream>
#include <stdexcept>

#include "Exception.h"

#include "Net/Socket.h"

#define DTIMEOUT 3

Socket::Socket(SocketType type_)
{
	int sock_type;

	// 判断创建何种Socket
	if(type_ == TcpSocket)
	{
		sock_type = SOCK_STREAM;
	}
	else
	{
		sock_type = SOCK_DGRAM;
	}

	// 创建socket
	if((this->sock_fd = socket(AF_INET, sock_type, 0)) == -1)
	{
		Throw("Create socket failed", errno);
	}

	// 创建引用计数器
	ref_counter = new int(1);
	timeout.tv_sec = DTIMEOUT;
	timeout.tv_usec = 0;
}

Socket::Socket(int sock_fd_)
	: sock_fd(sock_fd_)
{
	// 创建引用计数器
	ref_counter = new int(1);
	timeout.tv_sec = DTIMEOUT;
	timeout.tv_usec = 0;
}

Socket::Socket(const Socket& sock_)
	: sock_fd(sock_.sock_fd),
	timeout(sock_.timeout)
{
	ref_counter = sock_.ref_counter;

	// 发生复制，引用计数器加一
	(*ref_counter)++;
}

Socket::~Socket()
{
	// 一个引用被删除，引用计数器减一
	// 如果所有引用都被删除，则关闭socket
	if(!--(*ref_counter))
	{
		this->Close();
		delete ref_counter;
	}
}

Socket& Socket:: operator = (Socket& sock_)
{
	// xian add 2011-9-26 11:40:22
	// 若是自己给自己赋值，直接返回
	if (this == &sock_)
	{
		return sock_;
	}
	
	// 删除自身引用，然后再赋新值
	if (!--(*ref_counter))
	{
		this->Close();
		delete ref_counter;		
	}

	this->sock_fd = sock_.sock_fd;
	this->timeout = sock_.timeout;

	// 发生复制，引用计数器加一
	ref_counter = sock_.ref_counter;
	(*ref_counter)++;

	return sock_;
}


void Socket::Connect(const std::string& ip_, int port_, int timeout_)
{
	struct sockaddr_in sin;
	int err = 0;
	socklen_t len = sizeof(err);
	int retval;
	
	bzero(&sin, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_port = htons(port_);
	inet_pton(AF_INET, ip_.c_str(), &sin.sin_addr);

	// Connect默认超时时间由系统决定，默认可以从75秒到几分钟的时间
	// 更改系统设置会影响到其他程序，因此采用程序动态设置的方法实现connect超时功能
	// 获取当前socket设置
	int flags;
	if( (flags = fcntl( this->sock_fd , F_GETFL, 0)) == -1 )
	{
		Throw("fcntl: F_GETFL", errno);
	}

	// 设置socket为非阻塞
	if (fcntl(this->sock_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		Throw("fcntl: F_SETFL", errno);
	}

	// 连接远程主机
	while( ( (retval = connect(this->sock_fd, (struct sockaddr *)&sin, sizeof(sin))) == -1) 
			&& ( (err = errno)) == EINTR);

	// 连接成功正常返回
	if(retval == 0)
	{
		return;
	}

	// 出错，且不是非阻塞模式下的正在处理（EINPROGRESS）
	if(retval == -1 && err != EINPROGRESS)
	{
		std::stringstream msg;
		msg << "connect to host " << ip_ << ":" << port_ <<" failed";
		Throw(msg.str(), err);
	}

	fd_set read_fds;
	fd_set write_fds;
	struct timeval i_timeout;

	// 填充描述符
	FD_ZERO(&read_fds);
	FD_SET(this->sock_fd, &read_fds);
	write_fds = read_fds;

	// 设置超时
	i_timeout.tv_sec = timeout_ / 1000000;
	i_timeout.tv_usec = timeout_ % 1000000;

	// 等待远程主机响应到来
/*	
	while((retval = select(FD_SETSIZE, &read_fds, &write_fds, NULL, timeout_? & i_timeout : NULL )) == -1 
			&& errno == EINTR);
	// xian add 2011-9-26 15:16:23 由 FD_SETSIZE ==> this->sock_fd + 1
*/	
	while((retval = select(this->sock_fd + 1, &read_fds, &write_fds, NULL, timeout_? & i_timeout : NULL )) == -1 
			&& errno == EINTR);

	// select超时返回
	if(retval == 0)
	{
		std::stringstream msg;
		msg << "connect to host " << ip_ << ":" << port_ <<" failed";
		Throw(msg.str(), ETIMEDOUT);
	}
	else if(retval > 0)
	{
		// 验证就绪的描述符是否是sock_fd
        if (!FD_ISSET(this->sock_fd, &read_fds) && !FD_ISSET(this->sock_fd, &write_fds))
        {
			std::stringstream msg;
			msg << "connect to host " << ip_ << ":" << port_ <<" failed";
			Throw(msg.str(), ETIMEDOUT);
        }

		// 获取套接口目前的信息来判断是否真的是连接上了
		if (getsockopt(this->sock_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
		{
			Throw("getsockopt fail", errno);
		}

		// err !=0 表示没有连接上
		if (err != 0)
		{
			std::stringstream msg;
			msg << "connect to host " << ip_ << ":" << port_ <<" failed";
			Throw(msg.str(), ETIMEDOUT);
		}		
	
		// select在超时时间内返回，连接成功
		// 还原socket设置
		if (fcntl(sock_fd, F_SETFL, flags) == -1)
		{
			Throw("fcntl: F_SETFL", errno);
		}
	}
	else
	{
		Throw("select failed", errno);
	}

}

void Socket::Close()
{
	close(this->sock_fd);
}

void Socket::Bind(const int port_, const std::string& ip_)
{
	struct sockaddr_in serv_addr;

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	if (ip_.empty())
	{
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		int err = inet_pton(AF_INET, ip_.c_str(), &serv_addr.sin_addr);
		if (err == 0)
		{
			Throw("the ip address try to listen on is invailed", 0);
		}
		else if (err == -1)
		{
			err = errno;
			Throw("inet_pton error", err);
		}
		else
		{
			// OK
		}
	}
	
	serv_addr.sin_port = htons(port_);

	// 绑定地址至socket句柄
	if (bind(this->sock_fd, (struct sockaddr*)&serv_addr, sizeof(sockaddr_in)) == -1)
	{
		int err = errno;
		std::stringstream msg;
		msg << "bind listen socket on " << ip_ << ":" << port_ << " failed.";
		Throw(msg.str(), err);
	}
}

int Socket::GetLocalPort() const
{
	int port = 0;
	struct sockaddr_in addr;
	int socklen;

	bzero(&addr, sizeof(addr));
	socklen = sizeof(addr);

	// 获取绑定在socket上的端口号
	if(getsockname(this->sock_fd,(struct sockaddr *)&addr,(socklen_t*)&socklen))
	{
		Throw("Get remote address failed", errno);
	}

	port = ntohs(addr.sin_port);

	return port;
}

std::string Socket::GetLocalAddress() const
{

	struct sockaddr_in addr;
	int socklen;

	bzero(&addr, sizeof(addr));
	socklen = sizeof(addr);

	// 获取绑定在socket上的地址
	if(getsockname(this->sock_fd,(struct sockaddr *)&addr,(socklen_t*)&socklen))
	{
		Throw("getsockname failed", errno);
	}
	
	char ip_cstr[20];
	if(inet_ntop(addr.sin_family, &(addr.sin_addr), ip_cstr, 20) == NULL)
	{
		Throw("Convert local address from sockaddr_in to string format failed", errno);
	}

	return std::string(ip_cstr);
}

int Socket::GetLocalAddressAndPort(std::string& ip_) const
{
	struct sockaddr_in addr;
	int socklen;
	int port = 0;

	bzero(&addr, sizeof(addr));
	socklen = sizeof(addr);

	// 获取绑定在socket上的地址
	if(getsockname(this->sock_fd, (struct sockaddr *)&addr, (socklen_t*)&socklen))
	{
		Throw("getsockname failed", errno);
	}
	
	char ip_cstr[20];
	if(inet_ntop(addr.sin_family, &(addr.sin_addr), ip_cstr, 20) == NULL)
	{
		Throw("Convert local address from sockaddr_in to string format failed", errno);
	}
	
	ip_ = ip_cstr;
	port = ntohs(addr.sin_port);
	
	return port;
}

int Socket::GetRemotePort() const
{
	int port = 0;
	struct sockaddr_in addr;
	int socklen;

	bzero(&addr, sizeof(addr));
	socklen = sizeof(addr);

	// 获取远程主机端口号
	if(getpeername(this->sock_fd,(struct sockaddr *)&addr,(socklen_t*)&socklen))
	{
		Throw("getpeername failed.", errno);
	}

	port = ntohs(addr.sin_port);

	return port;
}

std::string Socket::GetRemoteAddress() const
{
	struct sockaddr_in addr;
	int socklen;

	bzero(&addr, sizeof(addr));
	
	socklen = sizeof(addr);

	// 获取远程主机地址
	if(getpeername(this->sock_fd,(struct sockaddr *)&addr,(socklen_t*)&socklen))
	{
		Throw("getpeername failed", errno);
	}

	char ip_cstr[20];
	if(inet_ntop(addr.sin_family, &(addr.sin_addr), ip_cstr, 20) == NULL)
	{
		Throw("Convert local address from sockaddr_in to string format failed", errno);
	}

	return std::string(ip_cstr);
}

int Socket::GetRemoteAddressAndPort(std::string& ip_) const
{
	int port = 0;
	struct sockaddr_in addr;
	int socklen;

	bzero(&addr, sizeof(addr));
	socklen = sizeof(addr);

	// 获取远程主机端口号
	if(getpeername(this->sock_fd,(struct sockaddr *)&addr,(socklen_t*)&socklen))
	{
		Throw("getpeername failed.", errno);
	}
	
	char ip_cstr[20];
	if(inet_ntop(addr.sin_family, &(addr.sin_addr), ip_cstr, 20) == NULL)
	{
		Throw("Convert local address from sockaddr_in to string format failed", errno);
	}

	ip_ = ip_cstr;
	port = ntohs(addr.sin_port);

	return port;	
}

void Socket::SetTimeout(long micro_second_)
{
	this->timeout.tv_sec = micro_second_ / 1000000;
	this->timeout.tv_usec = micro_second_ % 1000000;
}

long Socket::GetTimeout() const
{
	return this->timeout.tv_sec * 1000000 + this->timeout.tv_usec;
}

void Socket:: BeNonBlocking()
{
	// 获取当前socket设置
	int flags = fcntl( sock_fd , F_GETFL, 0);

	// 设置非阻塞位
	flags |= O_NONBLOCK;

	// 写回设置
	if (fcntl(sock_fd, F_SETFL, flags) == -1)
	{
		Throw("fcntl: F_SETFL", errno);
	}
}

void Socket:: BeBlocking()
{
	// 获取当前socket设置
	int flags = fcntl( sock_fd , F_GETFL, 0);

	// 取消非阻塞位
	flags &= ~O_NONBLOCK;

	// 写回设置
	if (fcntl(sock_fd, F_SETFL, flags) == -1)
	{
		Throw("fcntl: F_SETFL", errno);
	}
}

void Socket::SetSockOpt(int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(this->sock_fd, level, optname, optval, optlen) == -1)
	{
		Throw("setsockopt error", errno);
	}
}

void Socket::GetSockOpt(int level, int optname, void *optval, socklen_t *optlen)
{
	if (getsockopt(this->sock_fd, level, optname, optval, optlen) == -1)
	{
		Throw("getsockopt error", errno);
	}
}


struct timeval* Socket:: TranslateTime(long timeout_, struct timeval *timevs_)
{

	struct timeval *ptimeout;

	if(timeout_ == -1)
	{
		ptimeout = NULL;
	}
	else if(timeout_ == 0)
	{
		ptimeout = &(this->timeout);
	}
	else
	{
		timevs_->tv_sec = timeout_ / 1000000;
		timevs_->tv_usec = timeout_ % 1000000;
		ptimeout = timevs_;
	}

	return ptimeout;
}
