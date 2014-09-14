/**********************************************************
 * Filename:    MessageQueue.h
 * Text Encoding: utf-8
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 * Create Data:	2008-12-18
 * Last Update: 2009-04-14
 *
 *********************************************************/


#ifndef _INCLUDE_MESSAGEQUEUE_H_
#define _INCLUDE_MESSAGEQUEUE_H_

#include <sys/types.h>

#include <string>

/*!
 * @class MessageQueue
 * @brief Linux消息队列操作封类
 * @author 莫锡昌
 * @version 0.1
 * @date 2009-4-14
 *
 * 封装了Linux下的消息队列操作
 *
 * @todo 
 */
class MessageQueue
{

	public:

		/*!
		 * @brief 构造函数
		 * 
		 * 使用给定路径创建消息队列
		 *
		 * @param[in] path_ 系统路径 
		 */
		MessageQueue(const std::string path_ = "");

		//MessageQueue(key_t key_);

		/*!
		 * @brief 构造函数
		 *
		 * 使用消息队列ID创建对象 
		 *
		 * @param[in] msq_id_ 消息队列ID
		 */
		MessageQueue(const int msq_id_);

		/*!
		 * @brief 拷贝构造函数
		 * 
		 * @param[in] msq_ 被复制的对象
		 */
		MessageQueue(MessageQueue& msq_);

		/*!
		 * @brief 析构函数
		 */
		~MessageQueue();

		/*!
		 * @brief 重载赋值运算符
		 * 
		 * 用于保持引用计数
		 *
		 * @param[in] msq_ 被复制的对象
		 */
		MessageQueue& operator = (MessageQueue& msq_);

		/*!
		 * @brief 消息报文结构体
		 * 
		 */
		struct MessagePacket{
			long type; /*!< 报文类型 */
			void* data; /*!< 指向数据的指针 */

			/*!
			 * @brief 消息报文结构体构造函数
			 *
			 * @param[in] type_ 报文类型 
			 * @param[in] data_ 指向数据的指针
			 */
			MessagePacket(int type_, void* data_)
				: type(type_), data(data_)
			{}
		};
		
		/*!
		 * @brief 创建一个消息队列
		 * 
		 * @param[in] path_ 系统路径
		 */
		void Create(const std::string path_="");

		/*!
		 * @brief 销毁当前消息队列
		 * 
		 */
		void Destory();

		/*!
		 * @brief 发送消息
		 * 
		 * @param[in] packet_ 消息队列报文
		 */
		void Send(const struct MessagePacket* packet_);
		/*!
		 * @brief 接收消息
		 * 
		 * @param[in] packet_ 消息队列报文
		 */
		void Recv(struct MessagePacket* packet_);

		/*!
		 * @brief 设置消息类型
		 * 
		 * @param[in] type_ 消息类型
		 */
		void setMessageType(const int type_){
			msg_type_m = type_;
		}

		/*!
		 * @brief 获取当前消息类型
		 * 
		 * @return 消息类型
		 */
		int getMessageType(){
			return msg_type_m;
		}


	private:

		//! 消息队列对应的系统路径
		std::string path_m;

		//! 消息队列的键值
		key_t key_m;

		//! 消息队列id
		int msq_id_m;

		//! 消息类型
		long msg_type_m;

		//! 引用计数器
		int* ref_counter;

};

#endif /* _INCLUDE_MESSAGEQUEUE_H_ */
