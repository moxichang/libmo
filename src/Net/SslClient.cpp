#include <openssl/ssl.h>
#include <openssl/err.h>

#include <string>
#include <iostream>

#include <Exception.h>
#include "Net/SslClient.h"

SslClient:: SslClient(const std::string& keyfile_, const std::string& password_)
	: Socket(TcpSocket), ctx_m(NULL), ssl_m(NULL), sbio_m(NULL), check_cname_m(true)
{
	ctx_m = Initialize_CTX(keyfile_.c_str(), password_.c_str());
}

SslClient:: ~SslClient()
{
	if(ssl_m)
	{
		SSL_shutdown(ssl_m);
		SSL_free(ssl_m);
	}

	if(ctx_m)
	{
		SSL_CTX_free(ctx_m);
	}
}

SSL_CTX* SslClient:: Initialize_CTX(const char *keyfile_, const char *password_)
{
	SSL_METHOD *meth;
	SSL_CTX *ctx;
	BIO *bio_err=0;

	if(!bio_err){
		// 初始化ssl库
		SSL_library_init();
		SSL_load_error_strings();

		// 错误句柄初始化
		bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
	}

	// FIXME
	// 向一个被对方单向关闭的socket写数据会引发SIGPIPE信号
	// 问题是，这应该由库函数来做么？
	//signal(SIGPIPE,sigpipe_handle);

	meth = SSLv23_method();
	ctx = SSL_CTX_new(meth);

	// 加载客户端密钥文件
	if(!(SSL_CTX_use_certificate_chain_file(ctx, keyfile_)))
	{
		Throw("Can't read certificate file", MException::ME_INVARG);
	}

	// 存储客户端密钥文件的密码
	SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)password_);

	if(!(SSL_CTX_use_PrivateKey_file(ctx, keyfile_, SSL_FILETYPE_PEM)))
	{
		Throw("Can't read key file", MException::ME_INVARG);
	}

	return ctx;
}

void SslClient:: LoadTrustCert(const std::string& certfile_)
{
	// 加载信任CA证书
	if(!(SSL_CTX_load_verify_locations(ctx_m, certfile_.c_str() , 0)))
	{
		Throw("Can't read CA list", MException::ME_INVARG);
	}

#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
	SSL_CTX_set_verify_depth(ctx,1);
#endif

}

void SslClient:: SSLConnect(const std::string& host_, int port_, int timeout_)
{
	// 连接对方主机，普通tcp连接
	Connect(host_, port_, timeout_);

	// 生成ssl句柄
	ssl_m = SSL_new(ctx_m);

	// 关联socket句柄与ssl句柄
	sbio_m = BIO_new_socket(sock_fd, BIO_NOCLOSE);

	SSL_set_bio(ssl_m, sbio_m, sbio_m);

	// 开始ssl会话
	if(SSL_connect(ssl_m) <= 0)
	{
		Throw("SLL_connect failed", MException::ME_RUNTIME);
	}

	// 进行安全校验
	CheckCertification( host_.c_str() );

	// 设置socket为非阻塞模式
	BeNonBlocking();
}

void SslClient:: CheckCertification(const char *host_)
{
	X509 *peer;
	char peer_CN[256];

	int retval;

	// 校验对方证书
	if( (retval = SSL_get_verify_result(ssl_m)) != X509_V_OK)
	{
		char err_msg[128];
		sprintf(err_msg,"Certificate doesn't verify. Code: %d", retval);

		Throw(err_msg, MException::ME_INVARG);
	}

	// 校验域名与证书中的common name是否相同
	peer = SSL_get_peer_certificate(ssl_m);
	X509_NAME_get_text_by_NID( X509_get_subject_name(peer), NID_commonName, peer_CN, 256);

#ifdef SslClient_DEBUG
	printf("peer: %s\n", peer_CN);
	printf("host: %s\n", host_);
#endif

	// 默认为校验Common Name字段
	if(check_cname_m == true)
	{
		if(strcasecmp(peer_CN, host_))
		{
			Throw("Common name doesn't match hostname", MException::ME_INVARG);
		}
	}
}

int SslClient:: Recv(void *buf_, size_t length_, long timeout_)
{
	int retval;
	int bytes;
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	struct timeval* ptimeout;

	// 由于ssl在用户与内核之间保留了一块缓冲区存放数据
	// 如果一开始就进行select，有可能遇到这样的情况：
	//  1、上一次调用后，ssl缓冲区内的数据还没有读完
	//  2、对方主机的数据已经发送完毕
	// 这时在SSL_read之前使用select，将会导致当前线程始终阻塞
	// 导致程序僵死
	while(1)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);

		bool need_io;
		bool shutdown;

		do
		{
			need_io = false;
			shutdown = false;

			// 读取数据
			if( ( bytes = SSL_read(this->ssl_m, (char *)buf_, length_) ) > 0)
			{
				break;
			}

			// 检查错误
			switch(SSL_get_error(ssl_m, bytes))
			{
				case SSL_ERROR_NONE:
					break;

				case SSL_ERROR_ZERO_RETURN:
					bytes = 0;
					shutdown = true;
					break;

				// ssl缓冲区已经为空，需要读取数据
				case SSL_ERROR_WANT_READ:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_READ\n");
#endif
					FD_SET(this->sock_fd, &read_fds);
					need_io = true;
					break;

				// 需要写数据
				// 注意：即使是SSL_read，也可能返回SSL_ERROR_WANT_WRITE
				// 因为ssl中涉及到一些握手与校验信息，通信始终是双向的
				// 所以不光要设置读套接字集(read_fds)，还要设置写套接字集(write_fds)
				case SSL_ERROR_WANT_WRITE:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_WRITE\n");
#endif
					FD_SET(this->sock_fd, &write_fds);
					need_io = true;
					break;

				case SSL_ERROR_SYSCALL:
					Throw("ssl I/O error occurred", MException::ME_RUNTIME);
					break;

				default:
					Throw("SSL_read() failed", MException::ME_RUNTIME);
			}

		}while(need_io == false && shutdown == false);

		// 跳出最外层的循环
		if(bytes >= 0)
		{
			break;
		}

		ptimeout= TranslateTime(timeout_, &timeout);

		while((retval = select(FD_SETSIZE, &read_fds, &write_fds, NULL, ptimeout)) == -1
				&& errno == EINTR);

		if ( retval > 0 )
		{
			// FIXME
			// 这里是否要调用FD_ISSET(this->sock_fd, &read_fds) ？
			continue;
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

int SslClient:: Send(const void *buf_, size_t length_, long timeout_)
{
	int retval;
	int bytes;
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	struct timeval* ptimeout;

	while(1)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);

		bool need_io;
		bool shutdown;
		do
		{
			need_io = false;
			shutdown = false;

			if( (bytes = SSL_write(this->ssl_m, (char *)buf_, length_)) > 0)
			{
				break;
			}

			// 测试错误
			switch(SSL_get_error(ssl_m, bytes))
			{
				case SSL_ERROR_NONE:
					break;

				// 连接已经断开
				case SSL_ERROR_ZERO_RETURN:
					bytes = 0;
					shutdown = true;
					break;

				case SSL_ERROR_WANT_READ:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_READ\n");
#endif
					FD_ZERO(&read_fds);
					FD_SET(this->sock_fd, &read_fds);

					need_io = true;
					break;

				case SSL_ERROR_WANT_WRITE:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_WRITE\n");
#endif
					FD_ZERO(&write_fds);
					FD_SET(this->sock_fd, &write_fds);

					need_io = true;
					break;

				case SSL_ERROR_SYSCALL:
					Throw("ssl I/O error occurred", MException::ME_RUNTIME);
					break;

				default:
					Throw("SSL_read() failed", MException::ME_RUNTIME);
			}

		}while(need_io == false && shutdown == false);
		
		if(bytes >= 0)
		{
			break;
		}

		ptimeout= TranslateTime(timeout_, &timeout);

		// 等待数据到来
		while((retval = select(FD_SETSIZE, &read_fds, &write_fds, NULL, ptimeout)) == -1
				&& errno == EINTR);

		if( retval > 0 )
		{
			continue;
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

void SslClient:: CRecv(void *buf_, size_t length_, long timeout_)
{
	int retval;
	fd_set read_fds;
	fd_set write_fds;
	size_t recv_bytes = 0;
	struct timeval timeout;
	struct timeval* ptimeout;

	do
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);

		bool need_io;

		do
		{
			int bytes;
			need_io = false;

			bytes = SSL_read(this->ssl_m, (char *)buf_ + recv_bytes, length_ - recv_bytes);

			if(bytes > 0)
			{
				recv_bytes += bytes;
				continue;
			}

			switch(SSL_get_error(ssl_m, bytes))
			{
				case SSL_ERROR_NONE:
					break;

				case SSL_ERROR_ZERO_RETURN:
					Throw("socket closed by client", 0);
					break;

				case SSL_ERROR_WANT_READ:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_READ\n");
#endif
					FD_SET(this->sock_fd, &read_fds);

					need_io = true;
					break;

				case SSL_ERROR_WANT_WRITE:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_WRITE\n");
#endif
					FD_SET(this->sock_fd, &write_fds);

					need_io = true;
					break;

				case SSL_ERROR_SYSCALL:
					Throw("ssl I/O error occurred", MException::ME_RUNTIME);
					break;

				default:
					Throw("SSL_read() failed", MException::ME_RUNTIME);

			}


		}while( need_io == false && recv_bytes != length_);

		if(recv_bytes == length_)
		{
			break;
		}

		ptimeout= TranslateTime(timeout_, &timeout);

		while((retval = select(FD_SETSIZE, &read_fds, NULL, NULL, ptimeout)) == -1
				&& errno == EINTR);

		if ( retval > 0 )
		{
			continue;
		}
		else if( retval == 0 )
		{
			Throw("Socket time out", 0);
		}
		else
		{
			Throw("Select failed", errno);
		}

	}while( recv_bytes != length_ );

}

void SslClient:: CSend(const void *buf_, size_t length_, long timeout_)
{
	int retval;
	fd_set write_fds;
	fd_set read_fds;
	size_t sent_bytes = 0;
	struct timeval timeout;
	struct timeval* ptimeout;

	do
	{
		FD_ZERO(&write_fds);
		FD_ZERO(&read_fds);

		bool need_io;

		do
		{
			int bytes;
			need_io = false;

			bytes = SSL_write(this->ssl_m, (char *)buf_ + sent_bytes, length_ - sent_bytes);

			if(bytes > 0)
			{
				sent_bytes += bytes;
				continue;
			}

			// 测试错误
			switch(SSL_get_error(ssl_m, bytes))
			{
				case SSL_ERROR_NONE:
					break;

				case SSL_ERROR_ZERO_RETURN:
					Throw("socket closed by client", 0);
					break;

				case SSL_ERROR_WANT_READ:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_READ\n");
#endif
					FD_SET(this->sock_fd, &read_fds);

					need_io = true;
					break;

				case SSL_ERROR_WANT_WRITE:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_WRITE\n");
#endif
					FD_SET(this->sock_fd, &write_fds);

					need_io = true;
					break;

				case SSL_ERROR_SYSCALL:
					Throw("ssl I/O error occurred", MException::ME_RUNTIME);
					break;

				default:
					Throw("SSL_read() failed", MException::ME_RUNTIME);

			}

		}while(need_io == false && sent_bytes != length_);

		if( sent_bytes == length_ )
		{
			break;
		}

		ptimeout= TranslateTime(timeout_, &timeout);

		// 等待数据到来
		while((retval = select(FD_SETSIZE, &read_fds, &write_fds, NULL, ptimeout)) == -1
				&& errno == EINTR);

		if( retval > 0 )
		{
			continue;
		}
		else if( retval == 0 )
		{
			Throw("Socket time out", 0);
		}
		else
		{
			Throw("Select failed", errno);
		}

	}while(sent_bytes != length_);

	return;
}

int SslClient:: RecvCLRF(std::string& str_, long timeout_)
{
	char buf[2];
	std::string::size_type pos_clrf;

	int retval;
	fd_set read_fds;
	fd_set write_fds;
	struct timeval timeout;
	struct timeval *ptimeout;
	
	str_ = "";
	buf[1] = '\0';

	// 如果将缓冲区设置为一个较大值
	// 则有可能导致当前读取的数据越过第一个CLRF符，读取到多余的数据
	// 在这种情况下，如果要使程序正常运行，则需要开辟全局的存储空间，存储CLRF后的数据
	// 这将导致前后两个RecvCLRF调用失去独立性，增加编程的复杂度
	// 权衡效率与复杂度，选择牺牲效率
	while(1)
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);

		bool need_io;
		do
		{
			need_io = false;

			int bytes;
			bytes = SSL_read(this->ssl_m, buf, 1);

			switch(SSL_get_error(ssl_m, bytes))
			{
				case SSL_ERROR_NONE:
					// 没有错误，把这个字节加到目标字符串上
					str_ += buf;
					break;

				case SSL_ERROR_ZERO_RETURN:
					Throw("socket closed by client", 0);
					break;

				case SSL_ERROR_WANT_READ:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_READ\n");
#endif
					FD_SET(this->sock_fd, &read_fds);

					need_io = true;
					break;

				case SSL_ERROR_WANT_WRITE:
#ifdef SslClient_DEBUG
					printf("SSL_ERROR_WANT_WRITE\n");
#endif
					FD_SET(this->sock_fd, &write_fds);

					need_io = true;
					break;

				case SSL_ERROR_SYSCALL:
					Throw("ssl I/O error occurred", MException::ME_RUNTIME);
					break;

				default:
					Throw("SSL_read() failed", MException::ME_RUNTIME);
			}

			// 测试是否读取到了CLRF
			if( (pos_clrf = str_.find("\r\n")) == std::string::npos )
			{
				// 没有，继续
				continue;
			}
			else
			{
				// 有，赋给传入参数
				str_.erase(pos_clrf);
			}

		}while(need_io == false && pos_clrf == std::string::npos);

		// 跳出外层循环
		if(pos_clrf != std::string::npos)
		{
			break;
		}

		ptimeout= TranslateTime(timeout_, &timeout);

		while((retval = select(FD_SETSIZE, &read_fds, &write_fds, NULL, ptimeout)) == -1
				&& errno == EINTR);

		if(retval > 0)
		{
			continue;
		}
		else if(retval == 0)
		{
			Throw("Socket timeout",0);
		}
		else
		{
			Throw("Select on socket failed", errno);
		}
	}

	return 0;
}

int SslClient:: SendCLRF(const std::string& str_, long timeout_)
{
	std::string str = str_+ "\r\n";

	this->Send(str.c_str(), str.length(), timeout_);

	return 0;
}

bool SslClient:: IsCheckCommonName() const
{
	return check_cname_m;
}

void SslClient:: SetCheckCommonName(bool is_check_)
{
	check_cname_m = is_check_;
}
