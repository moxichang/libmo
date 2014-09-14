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
 *********************************************************/

#ifndef _INCLUDE_SOCKET_H_
#define _INCLUDE_SOCKET_H_

#include <sys/time.h>
#include <sys/socket.h>

#include <string>

/*!
 * @class Socket
 * @brief Socket通信基类
 * @author 莫锡昌
 * @version 0.1
 * @date 2009-04-14
 *
 * Socket基类，封装了linux socket的基本操作
 * 包括创建socket，设置socket选项，绑定地址，获取socket相关信息，设置超时选项等功能
 * 
 * 2011-09-22 xian update
 * 1、参数由host改为ip
 * 2、通过增加getsockopt函数判断连接是否成功
 * 3、修改赋值运算符重载函数
 *
 * @todo 
 */
class Socket
{

	public:
		enum SocketType { TcpSocket, UdpSocket };

		/*!
		 * @brief 构造函数
		 * 
		 * @param[in] type_ 指明创建何种类型的socket
		 */
		Socket(SocketType type_);

		/*!
		 * @brief 重载的构造函数
		 * 
		 * @param[in] sock_fd_ 由已经存在的socket句柄初始化当前对象
		 */
		Socket(int sock_fd_);

		/*!
		 * @brief 拷贝构造函数
		 * 
		 * @param[in] sock_ 复制的目标对象
		 */
		Socket(const Socket& sock_);

		/*!
		 * @brief 析构函数
		 *
		 */
		virtual ~Socket();

		/*!
		 * @brief 重载赋值运算符 
		 *
		 * 需要这个函数在复制对象时保持引用计数
		 *
		 * @param[in] sock_ 复制对象
		 */
		Socket& operator = (Socket& sock_);

		/*!
		 * @brief 连接到远程主机
		 * 
		 * @param[in] hostname_ 目标主机IP
		 * @param[in] port_ 目标主机端口
		 * @param[in] timeout_ 超时设置
		 *
		 */
		void Connect(const std::string& ip_, int port_, int timeout_ = 75000000);

		/*!
		 * @brief 断开链接
		 *
		 */
		void Close();

		/*!
		 * @brief 获取当前绑定的端口
		 * 
		 * @return
		 *   -- 当前绑定的端口
		 */
		int GetLocalPort() const;

		/*!
		 * @brief 获取当前绑定的地址 
		 * 
		 * @return 
		 *   -- 当前绑定的IP地址
		 */
		std::string GetLocalAddress() const;

		/*!
		 * @brief 获取当前绑定的ip地址和端口
		 * 
		 * @param[out] ip_ 目标主机IP
		 * @return
		 *   -- 当前绑定的端口
		 */
		int GetLocalAddressAndPort(std::string& ip_) const;
		
		/*!
		 * @brief 获取远程端口
		 * 
		 * @return 
		 *   -- 连接到的远程端口
		 */
		int GetRemotePort() const;

		/*!
		 * @brief 获取远程主机IP
		 * 
		 * @return
		 *   -- 远程主机的IP
		 */
		std::string GetRemoteAddress() const;

		/*!
		 * @brief 获取远程主机IP和端口
		 * 
		 * @param[out] ip_ 目标主机IP
		 * @return
		 *   -- 远程端口
		 */
		int GetRemoteAddressAndPort(std::string& ip_) const;		

		/*!
		 * @brief 将IP地址和端口绑定到当前socket句柄
		 * 
		 * 如果ip_参数为空，则表示绑定到任意地址(INADDR_ANY)
		 *
		 * @param[in] port_ 目标主机端口
		 * @param[in] ip_ 目标主机IP
		 */
		void Bind(int port_, const std::string& ip_ = "");
		

		/*!
		 * @brief 设置超时时间
		 *
		 * 设置socket读取或写入的超时时间
		 *
		 * @param[in] micro_second_ 设置超时时间，单位为微秒（1 E-6 s）
		 * 
		 * @bug 
		 *  -- 是否要区分读取超时与发送超时？
		 *  -- 参数为0或负数的情况未作处理
		 *
		 * @todo 设置一个可以取消超时的方法
		 */
		void SetTimeout(long micro_second_);

		/*!
		 * @brief 获取当前设置的超时时间
		 *
		 * @return 
		 *  -- 当前设置的超时时间，单位为微秒
		 */
		long GetTimeout() const;

		/*!
		 * @brief 设置Socket为非阻塞状态
		 */
		void BeNonBlocking();

		/*!
		 * @brief 设置Socket为阻塞状态
		 */
		void BeBlocking();

		/*!
		 * @brief 设置Socket选项
		 *
		 * @param[in] level
		 * @param[in] optname
		 * @param[out] optval
		 * @param[out] optlen
		 * 详见 man setsockopt	
		 */
		void SetSockOpt(int level, int optname, const void *optval, socklen_t optlen);

		/*!
		 * @brief 获取Socket选项
		 *
		 * @param[in] level
		 * @param[in] optname
		 * @param[out] optval
		 * @param[out] optlen
		 * 详见 man getsockopt
		 */
		void GetSockOpt(int level, int optname, void *optval, socklen_t *optlen);

	protected:

		//! socket句柄
		int sock_fd;

		//! 当前socket状态
		//SocketState sock_state;

		//! 存储超时时间
		struct timeval timeout;

		//! 引用计数器
		int* ref_counter;

		/*! 
		 * @brief 时间格式转换
		 *
		 * 将微秒时间转为select系统调用需要的timeval结构体指针
		 *
		 * @param[in] timeout_ 单位为微秒的时间
		 * @param[out] timevs_ struct timeval结构体
		 * 
		 * @return 
		 *  -- 当timeout_为-1时，返回NULL
		 *  -- 否则返回指向传入参数timevs_的指针
		 */
		struct timeval* TranslateTime(long timeout_, struct timeval *timevs_);

};

#endif /* _INCLUDE_SOCKET_H_ */
