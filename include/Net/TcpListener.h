/**********************************************************
 * Filename:    Socket.h
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

#ifndef _INCLUDE_TCPLISTENER_H_
#define _INCLUDE_TCPLISTENER_H_

#include <string>

#include "Socket.h"
#include "TcpClient.h"

/*!
 * @class TcpListener
 * @brief TCP监听类
 * @author 莫锡昌
 * @version 0.1
 * @date 2009-4-14
 *
 * Socket 派生类。封装了Tcp服务器基本操作 \n
 * 包括绑定端口及接收传入连接等。
 *
 * 
 *
 */
class TcpListener : public Socket
{
	public:
		/*!
		 * @brief 构造函数
		 *
		 * 此函数不执行任何操作，以便在创建对象之后再进行监听 
		 *
		 * @bug 使用此函数创建对象后，并没有机制保证在
		 * 没有调用Listen的情况不能调用Accept
		 * 这种情况下会导致错误发生。
		 */
		TcpListener();

		/*!
		 * @brief 重载的构造函数，可以指定监听的端口
		 * 
		 * 如果address_参数为空，则表示绑定到任意地址(INADDR_ANY)
		 *
		 * @param[in] port_ 绑定的监听端口
		 * @param[in] address_ 绑定的监听地址
		 */
		TcpListener(int port_, const std::string& address_ = "");
		
		/*!
		 * @brief 拷贝构造函数
		 * 
		 * @param[in] tcp_ 复制的目标对象
		 */
		TcpListener(const TcpListener& tcp_);
		
		/*!
		 * @brief 析构函数
		 * 
		 */
		~TcpListener();
		
		/*!
		 * @brief 重载赋值运算符 
		 *
		 * 需要这个函数在复制对象时保持引用计数
		 *
		 * @param[in] tcp_ 复制对象
		 */
		TcpListener& operator = (TcpListener& tcp_);

		/*!
		 * @brief 监听端口
		 * 
		 * @param[in] port_ 绑定的监听端口
		 * @param[in] address_ 绑定的监听地址
		 */
		void Listen(int port_, const std::string& address_ = "");

		/*!
		 * @brief 构造函数
		 * 
		 * @return
		 *  -- TcpClient 指向传入连接socket的指针
		 */
		TcpClient* Accept();

		/*
		 * @brief 设置Time Wait时间
		 *
		 * 当设置active_为1，seconds_为0时，
		 * 可避免绑定端口延时的问题
		 * 
		 * @param[in] active_ 是否启动linger
		 * @param[in] seconds_ linger秒数
		 */
		void SetLingerOption(int active_, int seconds_);

	private:
};

#endif /* _INCLUDE_TCPLISTENER_H_ */
