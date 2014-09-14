/**********************************************************
 * Filename:    Mutex.h
 * Text Encoding: utf-8
 *
 * Description:
 *              
 *
 * Author: moxichang (ishego@gmail.com)
 *         Harbin Engineering University 
 *         Information Security Research Center
 *
 * Create Data:	2008-10-20
 * Last Update: 2008-11-25
 *
 *********************************************************/

#ifndef _INCLUDE_MUTEX_H_
#define _INCLUDE_MUTEX_H_

#include <pthread.h>

/*!
 * @class Mutex
 * @brief Linux 互斥锁封装类
 * @author 莫锡昌
 * @version 0.1
 * @date 2009-4-14
 *
 * 互斥锁类，封装了POSIX 互斥锁操作，支持在进程间使用
 *
 * @todo 
 */

class Mutex{

	public:
		
		/*!
		 * @brief 构造函数，初始化互斥锁
		 *
		 */
		Mutex();

		/*!
		 * @brief 析构函数
		 * 
		 */
		~Mutex();

		/*!
		 * @brief 执行锁定操作
		 */
		void Wait();

		/*!
		 * @brief 尝试执行锁定操作
		 *
		 * 此操作不会导致阻塞，调用之后立即返回。
		 * 
		 * @return
		 *   -- 0 成功锁定
		 *   -- Mutex::BUSY，已经被锁定
		 */
		int TryWait();

		
		/*!
		 * @brief 解除锁定
		 *
		 */
		void Release();

		//! 状态标识符
		static const int BUSY = EBUSY;

	private:

		//! 互斥锁句柄
		pthread_mutex_t *mlock;

};

#endif /* _INCLUDE_MUTEX_H_ */
