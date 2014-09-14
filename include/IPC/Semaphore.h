/**********************************************************
 * Filename:    Semaphore.h
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
 * Last Update: 2008-12-18
 *
 *********************************************************/

#ifndef _INCLUDE_SEMAPHORE_H_
#define _INCLUDE_SEMAPHORE_H_

#include <semaphore.h>

#include <string>

/*!
 * @class Semaphore
 * @brief POSIX有名信号量封装类
 * @author 莫锡昌
 * @version 0.1
 * @date 2009-3-24
 *
 * 封装了POSIX有名信号量的常用操作
 * 可安全用于多进程/多线程环境
 *
 * @todo 
 *  -- 析构时，是否要调用sem_unlink ?
 */
class Semaphore{

	public:

		/*!
		 * @brief 默认构造函数
		 * 
		 * 信号量初始值用户指定 \n
		 * 信号量路径自动生成
		 *
		 * @param[in] val_ 信号量初始值 默认为0 
		 */
		Semaphore(const int val_ = 0);

		/*! 
		 * @brief 构造函数
		 *
		 * 参数val为信号量初始值，默认为0 \n
		 * 信号量路径自行指定，形式必须为
		 * /somename，不能是/tmp/xxx的形式 \n
		 * 参见 man 7 sem_overview
		 *
		 * @param[in] name_ 信号量路径
		 * @param[in] val_ 信号量初始值 默认为0
		 */
		Semaphore(const std::string name_, const int val_ = 0);

		/*!
		 * @brief 拷贝构造函数
		 *
		 * 需要这个函数在复制的时候保持引用计数
		 *
		 * @param[in] sem_ 被复制的信号量对象
		 */
		Semaphore(const Semaphore& sem_);
		
		/*!
		 * @brief 析构函数
		 *
		 * 销毁信号量
		 *
		 */
		~Semaphore();

		/*!
		 * @brief 重载赋值运算符
		 * 
		 * 需要这个函数在复制对象时保持引用计数
		 * 
		 * @param[in] sem_ 被复制的信号量对象
		 */
		Semaphore& operator = (Semaphore& sem_);

		/*!
		 * @brief 封装了wait操作
		 *
		 * 执行wait操作，阻塞等待
		 *
		 */
		void Wait();

		/*!
		 * @brief 封装了post操作
		 *
		 * 执行post操作
		 *
		 */
		void Post();

	private:

		//! 信号量路径
		std::string name_m; 

		//! 信号量句柄
		sem_t *sem_m;

		/*!
		 * @brief 引用计数器
		 *
		 * 当一个对象存在多个引用对象时，为了避免删除某个对象时
		 * 释放资源导致其余引用失效，需要通过这个变量维护引用数 \n
		 * 当最终ref_counter指向的值为0时，析构才才释放资源，否则不释放
		 *
		 */
		int* ref_counter; 
		
		/*!
		 * @brief 初始化信号量
		 *
		 * 初始化信号量
		 * 如果对应路径的信号量不存在，创建一个
		 * 如果存在，则打开
		 *  
		 * @param[in] name 路径名
		 * @param[in] sem 有名信号量句柄
		 * @param[in] val 信号量初始值
		 *
		 * @return
		 * -- 0 创建成功
		 * -- -1 创建失败 
		 */
		int GetNamed(const char *name, sem_t **sem, int val);

		/*!
		 * @brief 生成随机路径
		 *
		 * 随机生成一个路径
		 *
		 */
		int GenName();

};

#endif /* _INCLUDE_SEMAPHORE_H_ */
