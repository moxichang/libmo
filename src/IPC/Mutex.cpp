/**********************************************************
 * Filename:    Mutex.cpp
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


#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "Exception.h"
#include "IPC/Mutex.h"

#define ERR_LEN 200

Mutex::Mutex(){

	int err;

#ifdef _POSIX_THREAD_PROCESS_SHARED
	/* -------------------------------------------------- **
	** Linux下的POSIX互斥锁实现不支持在进程间使用，即
	**	PTHREAD_PROCESS_SHARED
	** 标志即使设置了也是没有效果的。
	** 所以在使用前需要测试一下是否存定义了
	**	_POSIX_THREAD_PROCESS_SHARED
	** 宏。如果定义了，则是支持进程间共享的系统，直接开内存
	** 使用即可，如果没有则需要自行定义一块共享内存。
	** 参见：
	** http://linux.die.net/man/3/pthread_mutexattr_init
	** 以及《unix网络编程》30.8节
	** -------------------------------------------------- */
	int fd;
	pthread_mutexattr_t mattr;

	fd = open("/dev/zero", O_RDWR, 0);
	mlock = (pthread_mutex_t*)mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE,
				MAP_SHARED, fd, 0);
	if(mlock == MAP_FAILED){
		Throw("allocate space for mutex failed", errno);
	}

	close(fd);
#else
	// 其他情况下，开辟普通的空间即可
	mlock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

	if(mlock == NULL){
		Throw("allocate space for mutex failed", errno);
	}
#endif

	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

	err = pthread_mutex_init(mlock,NULL);

	if(err != 0){
		Throw("initialize mutex failed.", err);
	}

}

Mutex::~Mutex(){

	pthread_mutex_destroy(mlock);	

}

void Mutex::Wait(){

	int err = pthread_mutex_lock(mlock);

	if(err != 0){
		Throw("lock mutex failed.", err);
	}

	return;
}

int Mutex::TryWait(){

	int err = pthread_mutex_trylock(mlock);

	if(err == EBUSY){
		return EBUSY;
	}

	if(err != 0){
		Throw("try lock mutex failed.", err);
	}

	return 0;
}

void Mutex::Release(){

	int err = pthread_mutex_unlock(mlock);
	
	if(err != 0){
		Throw("unlock mutex failed.", err);
	}

	return;
}
