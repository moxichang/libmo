/**********************************************************
 * Filename:    Log.h
 * Text Encoding: utf-8
 *
 * Description: functions for log message
 *
 * Author: moxichang (ishego@gmail.com) 
 *	   Harbin Engineering University
 *	   Information Security Research Center
 *
 * Create Data:	2008-08-10
 * Last Update: 2008-11-25
 *
 *********************************************************/


#ifndef	_INCLUDE_LOG_H_
#define	_INCLUDE_LOG_H_

#include <syslog.h>

#ifdef DAEMON

#define LOG(...) \
{ \
	Log::MSlog(__FILE__,__LINE__,__FUNCTION__, __VA_ARGS__); \
}

#else

#define LOG(...) \
{ \
	Log::MDebug(__FILE__,__LINE__,__FUNCTION__, __VA_ARGS__); \
}

#endif

class Log
{

	public:
		/*!
		 * @brief 调试打印函数
		 *
		 * 当DAEMON宏定义为未定义时，LOG宏将解释为为调用此函数
		 * 将输出日志信息至标准错误（stderr）
		 * 
		 * @param [in] file_ 文件名
		 * @param [in] line_ 行数
		 * @param [in] function_ 函数名
		 * @param [in] priority_ 信息级别, 与syslog函数一致。详细可查看 man 3 syslog
		 * @param [in] fmt_ 格式字符串，与printf函数相同
		 *
		 */
		static void MDebug(const char *file_, int line_, const char *function_, int priority_, const char * fmt, ...);

		/*!
		 * @brief 系统日志函数
		 *
		 * 当DAEMON宏定义存在时，LOG宏将解释为为调用此函数
		 * 将输出日志信息至syslog服务器
		 * 
		 * @param [in] file_ 文件名
		 * @param [in] line_ 行数
		 * @param [in] function_ 函数名
		 * @param [in] priority_ 信息级别, 与syslog函数一致。详细可查看 man 3 syslog
		 * @param [in] fmt_ 格式字符串，与printf函数相同
		 *
		 */
		static void MSlog(const char *file_, int line_, const char *function_, int priority_, const char * fmt, ...);

};

/* -------------------------------------- **
** 守护进程初始化函数，和LOG宏配合使用。
** 在编译时加上-DDAEMON选项，便可以使程序以
** 守护进程的形态运行于后台，同时，使用LOG宏
** 的输出将自动重定向到系统日志(/var/log/messages)中
** 参数：
**	path:
**		守护进程运行目录
**	pname:
**		程序名称（将出现在日志中）
**	facility:
**		日志类型参数
**		详见man openlog
** -------------------------------------- */
int Daemon_Init(const char *path, const char *pname, int facility);

#endif /* _INCLUDE_LOG_H_ */
