#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>

#include <string>
#include <stdexcept>

#include "Exception.h"
#include "Net/TcpClient.h"

#define BUFSIZE 1024

TcpClient::TcpClient() 
	: Socket(TcpSocket)
{
}

TcpClient::TcpClient(int sock_fd_) 
	: Socket(sock_fd_)
{
}

TcpClient::TcpClient(const std::string& ip_, int port_)
	: Socket(TcpSocket)
{
	Connect(ip_, port_);
}

TcpClient::TcpClient(const TcpClient& tcp_): Socket(tcp_)
{
	
}

TcpClient::~TcpClient()
{
}

TcpClient& TcpClient::operator = (TcpClient& tcp_)
{
	if (this != &tcp_)
	{
		Socket::operator=(tcp_);
	}
	
	return tcp_;
}

int TcpClient::Recv(char *buf_, size_t length_, long timeout_)
{
	int retval;
	int bytes;
	fd_set read_fds;
	struct timeval timeout;
	struct timeval* ptimeout;
	
	while(1)
	{
		FD_ZERO(&read_fds);
		FD_SET(this->sock_fd, &read_fds);

		ptimeout = TranslateTime(timeout_, &timeout);

		// 等待数据到来
/*		
		while((retval = select(FD_SETSIZE, &read_fds, NULL, NULL, ptimeout)) == -1 
				&& errno == EINTR);
		// xian add 2011-9-26 15:16:23 由 FD_SETSIZE ==> this->sock_fd + 1
*/		
		while((retval = select(this->sock_fd + 1, &read_fds, NULL, NULL, ptimeout)) == -1 
				&& errno == EINTR);

		// select 成功返回
		if ( retval > 0 )
		{
			if (FD_ISSET(this->sock_fd, &read_fds))
			{
				// 接收数据
				while( (bytes = recv(this->sock_fd, buf_, length_, MSG_DONTWAIT)) == -1 
						&& errno == EINTR); 

				// 套接字有可能在select返回到recv读取的这段时间内又不可用
				// 重新回去等待
				// FIXME 是否会导致忙等？
				if(bytes == -1 && errno == EAGAIN)
				{
					continue;
				}	
				else if (bytes == 0) // 连接被关闭
				{
					break;
				}
				else if (bytes < 0)
				{
					Throw("Socket error",errno);
				}
				else
				{
					break;
				}
			}
		}
		else if( retval == 0 )
		{
			Throw("Socket time out", 0);
		}
		else
		{
			Throw("Select failed", errno);
		}
	}

	return bytes;
}

int TcpClient::Send(const char *buf_, size_t length_, long timeout_)
{
	int retval;
	int bytes;
	fd_set write_fds;
	struct timeval timeout;
	struct timeval* ptimeout;
	
	size_t tol = 0;

	while(tol < length_)
	{
		FD_ZERO(&write_fds);
		FD_SET(this->sock_fd, &write_fds);

		// 设置超时
		ptimeout= TranslateTime(timeout_, &timeout);

		// 等待套接字可写
		while((retval = select(this->sock_fd + 1, NULL, &write_fds, NULL, ptimeout)) == -1 
			&& errno == EINTR);

		// select 成功返回
		if( retval > 0 )
		{
			if (FD_ISSET(this->sock_fd, &write_fds))
			{
				// 发送数据
				while( (bytes = send(this->sock_fd, buf_+tol, length_-tol, MSG_DONTWAIT)) == -1 
					&& errno == EINTR );

				if(bytes == -1 && errno == EAGAIN)
				{
					continue;
				}
				else if	(bytes	< 0)
				{
					Throw("socket error",errno);
				}
				else
				{
					tol += bytes;
					//break;
				}
			}
		}
		else if( retval == 0 )
		{
			Throw("Socket time out", 0);
		}
		else
		{
			Throw("Select failed", errno);
		}
	}

	return tol;
}

void TcpClient::CRecv(char *buf_, size_t length_, long timeout_)
{
	int retval;
	fd_set read_fds;
	size_t recv_bytes = 0;
	struct timeval timeout;
	struct timeval* ptimeout;

	while( recv_bytes != length_ )
	{

		FD_ZERO(&read_fds);
		FD_SET(this->sock_fd, &read_fds);

		// 超时设置
		ptimeout= TranslateTime(timeout_, &timeout);

		// 等待套接字可读
		while((retval = select(this->sock_fd + 1, &read_fds, NULL, NULL, ptimeout)) == -1 
			&& errno == EINTR);

		// select 成功返回
		if ( retval > 0 )
		{
			if (FD_ISSET(this->sock_fd, &read_fds))
			{
				int bytes;

				while( (bytes = recv(this->sock_fd, buf_ + recv_bytes, length_ - recv_bytes, MSG_DONTWAIT)) == -1 
					&& errno == EINTR); 
				
				if(bytes == -1 && errno == EAGAIN)
				{
					continue;
				}
				else if (bytes == 0) // 连接被关闭
				{
					Throw("socket closed by client", 0);
				}
				else if	(bytes	< 0)
				{
					Throw("socket error",errno);
				}

				recv_bytes += bytes;
			}
		}
		else if( retval == 0 )
		{
			Throw("socket time out", 0);
		}
		else
		{
			Throw("select failed", errno);
		}
	}

	return;
}

void TcpClient::CSend(const char *buf_, size_t length_, long timeout_)
{
	int retval;
	fd_set write_fds;
	size_t sent_bytes = 0;
	struct timeval timeout;
	struct timeval* ptimeout;

	while(sent_bytes != length_)
	{

		FD_ZERO(&write_fds);
		FD_SET(this->sock_fd, &write_fds);

		// 超时设置
		ptimeout= TranslateTime(timeout_, &timeout);

		// 等待套接字可写
		while((retval = select(this->sock_fd + 1, NULL, &write_fds, NULL, ptimeout)) == -1 
			&& errno == EINTR);

		if( retval > 0 )
		{
			if (FD_ISSET(this->sock_fd, &write_fds))
			{

				int bytes;
				while( (bytes = send(this->sock_fd, buf_ + sent_bytes, length_ - sent_bytes, MSG_DONTWAIT)) == -1 
					&& errno == EINTR );

				if(bytes == -1 && errno == EAGAIN)
				{
					continue;
				}
				else if (bytes < 0)
				{
					Throw("socket error",errno);
				}

				sent_bytes += bytes;
			}
		}
		else if( retval == 0 )
		{
			Throw("Socket time out", 0);
		}
		else
		{
			Throw("Select failed", errno);
		}
	}

	return;
}


int TcpClient::RecvCLRF(std::string& str_, long timeout_)
{
	char buf[BUFSIZE + 1];
	std::string::size_type pos_clrf;

	fd_set read_fds;
	int retval;
	struct timeval timeout;
	struct timeval *ptimeout;

	str_.clear();
//	buf[1] = '\0';

	while(1)
	{
		FD_ZERO(&read_fds);
		FD_SET(this->sock_fd, &read_fds);

		ptimeout= TranslateTime(timeout_, &timeout);

		// 等待套接字可读
		while((retval = select(this->sock_fd + 1, &read_fds, NULL, NULL, ptimeout)) == -1 
				&& errno == EINTR);

		if(retval > 0)
		{
			if (FD_ISSET(this->sock_fd, &read_fds))
			{

				int bytes;
				while( (bytes = recv(this->sock_fd, buf, BUFSIZE, MSG_DONTWAIT)) == -1 
						&& errno == EINTR);

				if( bytes == -1 && errno == EAGAIN)
				{
					continue;
				}
				else if (bytes == 0)
				{
					Throw("socket closed by client", 0);
				}
				else if(bytes < 0)
				{
					Throw("recv data failed", errno);
				}

				buf[bytes] = 0;
				str_ += buf;

				// 如果没有clrf字符串，继续读取
				if( (pos_clrf = str_.find("\r\n")) == std::string::npos )
				{
					continue;
				}
				else // 找到clrf字符串，提取并跳出循环
				{
					str_.erase(pos_clrf);
					break;
				}
			}
		}
		else if(retval == 0)
		{
			Throw("Socket timeout",0);
		}
		else
		{
			Throw("Select failed", errno);
		}
	}

	return 0;
}

int TcpClient::SendCLRF(const std::string& str_, long timeout_)
{
	std::string str = str_ + "\r\n";

	try
	{
	//  将Send改为CSend，保证数据能全部发送出	
	//	this->Send(str.c_str(), str.length(), timeout_);
		this->CSend(str.c_str(), str.length(), timeout_);
	}
	catch(std::exception &e)
	{
		Throw(e.what(), MException::ME_RETHROW);
	}

	return 0;
}
