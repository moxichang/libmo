/**********************************************************
 * Filename:    SslClient.h
 * Text Encoding: utf-8
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 * Create Data:	2008-08-10
 *
 *********************************************************/

#ifndef _INCLUDE_SSLCLIENT_H_
#define _INCLUDE_SSLCLIENT_H_

#include <string>

#include <openssl/ssl.h>

#include "Net/Socket.h"

/*!
 * @class SslClient
 * @brief SSL通信客户端操作封装类
 * @author 莫锡昌
 * @version 0.1
 * @date 2009-5-24
 *
 * 封装了基本的SSL操作
 * 支持读取/发送及超时返回
 * 支持CLRF字符串读取
 *
 * @todo 
 * 	拷贝构造函数
 *	重载"="操作符
 */
class SslClient : public Socket
{
	public:
		/*!
		 * @brief 构造函数
		 * 
		 * @param[in] keyfile_ ssl客户端密钥文件
		 * @param[in] password_ 密钥文件密码
		 */
		SslClient(const std::string& keyfile_, const std::string& password_);

		/*!
		 * @brief 析构函数
		 *
		 */
		~SslClient();

		/*!
		 * @brief 加载信任证书
		 * 
		 * @param[in] certfile_ 被信任证书的地址
		 */
		void LoadTrustCert(const std::string& certfile_);

		/*!
		 * @brief 连接远程主机 
		 * 
		 * @param[in] host_ 远程主机名
		 * @param[in] port_ 远程端口
		 * @param[in] timeout_ 超时设置
		 */
		void SSLConnect(const std::string& host_, int port_, int timeout_ = 75000000);

		/*!
		 * @brief 读取数据
		 * 
		 * 当timeout_参数为-1时，无时间限制 \n
		 * 当timeout_参数为0时，使用默认的超时时间 \n
		 * 当timeout_参数大于0时，使用timeout_的值作为超时时间，单位为微秒(microsecond '1 E-6 s')
		 * 
		 * @param[out] buf_ 数据缓冲区
		 * @param[in] length_ 缓冲区长度
		 * @param[in] timeout_ 超时时限
		 *
		 * @return
		 *	-- 0 连接已关闭
		 *	-- 非零 本次调用读取的字节数
		 */
		int Recv(void *buf_, size_t length_, long timeout_= -1);

		/*!
		 * @brief 发送数据 
		 * 
		 * 当timeout_参数为-1时，无时间限制 \n
		 * 当timeout_参数为0时，使用默认的超时时间 \n
		 * 当timeout_参数大于0时，使用timeout_的值作为超时时间，单位为微秒(microsecond '1 E-6 s')
		 * @param[in] buf_ 被发送数据所在的缓冲区
		 * @param[in] length_ 被发送数据长度
		 * @param[in] timeout_ 超时时限
		 *
		 * @return
		 *	-- 0 连接已关闭
		 * 	-- 非零 本次调用发送的字节数
		 */
		int Send(const void *buf_, size_t length_, long timeout_= -1);

		/*!
		 * @brief 读取数据 
		 * 
		 * 读取长度为length_的数据，存入buf中。\n
		 * 区别于系统自带的recv，在buf未满的情况下不返回
		 * 可设置超时时间 \n
		 *
		 * 当timeout_参数为-1时，无时间限制 \n
		 * 当timeout_参数为0时，使用默认的超时时间 \n
		 * 当timeout_参数大于0时，使用timeout_的值作为超时时间，单位为微秒(microsecond '1 E-6 s')
		 *
		 * @param[in] buf_ 缓冲区
		 * @param[in] length_ 需要读取的读取字节数
		 * @param[in] timeout_ 超时设置
		 */
		void CRecv(void *buf_, size_t length_, long timeout_= -1);
		
		/*!
		 * @brief 发送数据
		 * 
		 * 从buf中读取长度为size_的数据，通过SSL发送。\n
		 * 区别于系统自带的send，buf中的数据若未发送完毕则不返回
		 * 可设置超时时间  \n
		 *
		 * 当timeout_参数为-1时，无时间限制 \n
		 * 当timeout_参数为0时，使用默认的超时时间 \n
		 * 当timeout_参数大于0时，使用timeout_的值作为超时时间，单位为微秒(microsecond '1 E-6 s')
		 *
		 * @param[in] buf_ 缓冲区
		 * @param[in] length_ 需要发送的字节数
		 * @param[in] timeout_ 超时设置
		 */

		void CSend(const void *buf_, size_t length_, long timeout_= -1);

		/*!
		 * @brief 读取一个CLRF字符串
		 *  
		 * @param[out] clrf_str_ 收到的CLRF字符串
		 * @param[in] timeout_ 超时设置
		 *
		 */
		int RecvCLRF(std::string &clrf_str_, long timeout_= -1);

		/*!
		 * @brief 发送一个CLRF字符串 
		 * 
		 * 在传入的字符串后自动加上CLRF(\\r\\n)
		 * 
		 * @param[in] clrf_str_ 收到的CLRF字符串
		 * @param[in] timeout_ 超时设置
		 */
		int SendCLRF(const std::string& clrf_str_, long timeout_= -1);

		/*!
		 * @brief ssl内部使用密码存储函数
		 *
		 */
		static int password_cb(char *buf,int num, int rwflag,void *userdata);

		/*!
		 * @brief 获取当前Common Name校验配置
		 *
		 * @return
		 *   -- false 不校验
		 *   -- true  校验
		 */
		bool IsCheckCommonName() const;

		/*!
		 * @brief 获取当前Common Name校验配置
		 *
		 * @param[in] is_check_ 设置值
		 *
		 */	
		void SetCheckCommonName(bool is_check_);

	private:

		//! SSL 上下文句柄
		SSL_CTX *ctx_m;

		//! SSL 连接句柄
		SSL *ssl_m;

		//! BIO 对象句柄
		BIO *sbio_m;

		//! 客户端密钥
		std::string password_m;

		//! 初始化SSL上下文
		SSL_CTX* Initialize_CTX(const char *keyfile_, const char *password_);

		//! 校验通信对方证书
		void CheckCertification(const char *host_);

		//! 是否校验证书Common Name字段
		//! 即检查当前连接使用的主机名（域名或IP）是否一致
		bool check_cname_m;

};

#endif /* _INCLUDE_SSLCLIENT_H_ */

