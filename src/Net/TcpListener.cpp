#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

#include <sstream>
#include <string>

#include "Exception.h"
#include "Net/TcpListener.h"

TcpListener::TcpListener()
	: Socket(TcpSocket)
{
}

TcpListener::TcpListener(int port_, const std::string& address_)
	: Socket(TcpSocket)
{
	this->Listen(port_, address_);
}

TcpListener::TcpListener(const TcpListener& tcp_): Socket(tcp_)
{
	
}

TcpListener::~TcpListener()
{
}

TcpListener& TcpListener::operator = (TcpListener& tcp_)
{
	if (this != &tcp_)
	{
		Socket::operator=(tcp_);
	}
	
	return tcp_;
}

void TcpListener::Listen(int port_, const std::string& address_){

	struct sockaddr_in serv_addr;

	// FIXME 是否重复？
	// 重复了 xian
/*
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		Throw("create listen socket failed", errno);
	}
*/
	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	if(address_.empty())
	{
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		int err = inet_pton(AF_INET, address_.c_str(), &serv_addr.sin_addr);
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

	// 绑定地址和端口
	if (bind(this->sock_fd, (sockaddr*)&serv_addr, sizeof(sockaddr_in)) == -1)
	{
		int err = errno;
		std::stringstream info;
		info<<"bind to "<<address_<<":"<<port_<<" failed";
		Throw(info.str(), err);
	}

	if(listen(this->sock_fd, SOMAXCONN) == -1)
	{
		Throw("listen on socket failed", errno);
	}

	return;
}

TcpClient* TcpListener::Accept()
{
	int sock = -1;
	sockaddr_in sin_remote;
	int addr_size = sizeof(sockaddr_in);

	while((sock = accept(this->sock_fd, (sockaddr*)&sin_remote, (socklen_t*)&addr_size)) == -1
			&& errno == EINTR );

	if(sock == -1)
	{
		Throw("accept failed", errno);
	}
	else if(sock < 0)
	{
		Throw("accept failed, return value is negative but not -1", errno);
	}

	// 创建TcpClient对象
	TcpClient * sockobj;
	sockobj = new TcpClient(sock);

	return sockobj;
}

void TcpListener:: SetLingerOption(int active_, int seconds_)
{
	// 设置linger时间
	int i;
	struct linger linger_opt;/*Linger active, timeout 0 */

	linger_opt.l_onoff = active_;
	linger_opt.l_linger = seconds_;

	setsockopt(sock_fd, SOL_SOCKET, SO_OOBINLINE, &i, sizeof(i));
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
	setsockopt(sock_fd, SOL_SOCKET, SO_LINGER,
		   &linger_opt, sizeof(linger_opt));
}
