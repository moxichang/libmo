/**********************************************************
 * Filename:    TcpClient.h
 * Text Encoding: utf-8
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 * Create Data:	2008-08-11
 * Last Update: 2009-04-14
 *
 *********************************************************/

#ifndef _INCLUDE_TCPCLIENT_H_
#define _INCLUDE_TCPCLIENT_H_

#include <string>

#include "Socket.h"

/*!
 * @class TcpClient
 * @brief Socket客户端类
 * @author 莫锡昌
 * @version 0.1
 * @date 2009-4-14
 *
 * 封装了Tcp连接客户端操作
 * 支持读取/发送超时操作
 * 支持CLRF字符串读取
 *
 * 2011-9-26 xian update
 * 1、添加了复制构造函数和赋值运算符重载
 * 2、select函数参数 FD_SETSIZE ==> this->sock_fd + 1
 * 3、RecvCLRF函数由原来的每次只读取1个字节，改为每次读取1024个字节
 * 
 */
class TcpClient : public Socket
{
	public:
		/*!
		 * @brief 构造函数
		 * 
		 * 不执行任何操作，用于创建空对象 
		 * 
		 */
		TcpClient();

		/*!
		 * @brief 重载的构造函数 
		 * 
		 * @param[in] sock_fd_ socket文件描述符
		 */
		TcpClient(int sock_fd_);

		/*!
		 * @brief 连接到远程主机
		 * 
		 * @param[in] ip_ 远程主机IP
		 * @param[in] port_ 远程主机端口
		 */
		TcpClient(const std::string& ip_, int port_);

		/*!
		 * @brief 拷贝构造函数
		 * 
		 * @param[in] tcp_ 复制的目标对象
		 */
		TcpClient(const TcpClient& tcp_);
		
		/*!
		 * @brief 析构函数
		 * 
		 */
		~TcpClient();
		
		/*!
		 * @brief 重载赋值运算符 
		 *
		 * 需要这个函数在复制对象时保持引用计数
		 *
		 * @param[in] tcp_ 复制对象
		 */
		TcpClient& operator = (TcpClient& tcp_);

		/*!
		 * @brief 读取数据 
		 * 
		 * 从socket中读取数据，存入buf中，buf大小由size参数给出。\n
		 *
		 * 可设置超时时间 \n
		 *
		 * 当timeout_参数为-1时，无时间限制 \n
		 * 当timeout_参数为0时，使用默认的超时时间 \n
		 * 当timeout_参数大于0时，使用timeout_的值作为超时时间，单位为微秒(microsecond '1 E-6 s')
		 *
		 * @param[in] buf_ 缓冲区
		 * @param[in] size_ 读取字节数
		 * @param[in] timeout_ 超时设置
		 */
		int Recv(char *buf_, size_t size_, long timeout_ = -1);

		/*!
		 * @brief 发送数据
		 * 
		 * 从buf中读取长度为size_的数据，通过socket发送。\n
		 *
		 * 可设置超时时间  \n
		 *
		 * 当timeout_参数为-1时，无时间限制 \n
		 * 当timeout_参数为0时，使用默认的超时时间 \n
		 * 当timeout_参数大于0时，使用timeout_的值作为超时时间，单位为微秒(microsecond '1 E-6 s')
		 *
		 * @param[in] buf_ 缓冲区
		 * @param[in] size_ 读取字节数
		 * @param[in] timeout_ 超时设置
		 */
		int Send(const char *buf_, size_t size_, long timeout_ = -1);

		/*!
		 * @brief 读取数据 
		 * 
		 * 从socket中读取长度为size_的数据，存入buf中。\n
		 * 区别于系统自带的recv，在buf未满的情况下不返回
		 * 可设置超时时间 \n
		 *
		 * 当timeout_参数为-1时，无时间限制 \n
		 * 当timeout_参数为0时，使用默认的超时时间 \n
		 * 当timeout_参数大于0时，使用timeout_的值作为超时时间，单位为微秒(microsecond '1 E-6 s')
		 *
		 * @param[in] buf_ 缓冲区
		 * @param[in] size_ 需要读取的读取字节数
		 * @param[in] timeout_ 超时设置
		 */
		void CRecv(char *buf_, size_t size_, long timeout_ = -1);

		/*!
		 * @brief 发送数据
		 * 
		 * 从buf中读取长度为size_的数据，通过socket发送。\n
		 * 区别于系统自带的send，buf中的数据若未发送完毕则不返回
		 * 可设置超时时间  \n
		 *
		 * 当timeout_参数为-1时，无时间限制 \n
		 * 当timeout_参数为0时，使用默认的超时时间 \n
		 * 当timeout_参数大于0时，使用timeout_的值作为超时时间，单位为微秒(microsecond '1 E-6 s')
		 *
		 * @param[in] buf_ 缓冲区
		 * @param[in] size_ 需要发送的字节数
		 * @param[in] timeout_ 超时设置
		 */
		void CSend(const char *buf_, size_t size_, long timeout_ = -1);


		/*!
		 * @brief 读取一个CLRF字符串
		 *  
		 * @param[out] clrf_str_ 收到的CLRF字符串
		 * @param[in] timeout_ 超时设置
		 *
		 * @return 
		 */
		int RecvCLRF(std::string &clrf_str_, long timeout_ = -1);

		/*!
		 * @brief 发送一个CLRF字符串 
		 * 
		 * 会在传入的字符串后自动加上CLRF
		 * 
		 * @param[in] clrf_str_ 收到的CLRF字符串
		 * @param[in] timeout_ 超时设置
		 */
		int SendCLRF(const std::string &clrf_str_, long timeout_ = -1);

		//! 状态标识符，标识CLRF缓冲区中还有数据未读
		static const int CLRF_MORE = 1;
		//! 状态标识符，标识CLRF缓冲区中已无未读数据
		static const int CLRF_NONE = 0;

	private:

};

#endif /* _INCLUDE_TCPCLIENT_H_ */
