/**********************************************************
 * Filename:    Log.cpp
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


#include <stdarg.h>		/* ANSI C header file */
#include <syslog.h>		/* for syslog() */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MAXFD 64

#include "Log.h"

void Log:: MDebug(const char *file_, int line_, const char *function_, int priority_, const char * fmt, ...)
{

	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	char *p, *np;
	va_list ap;

	if ((p = (char *)malloc(size)) == NULL)
		return;

	while (1) {
		/* Try to print in the allocated space. */
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size){
		
#ifdef  LOGTIME
			// 在日志中加入日期时间
			time_t   timep;
			time   (&timep);
			char *pt = ctime(&timep);
			pt[strlen(pt) - 1] = ' '; // change '\n' to ' '
			fprintf(stderr,"%s%s:: %s\n", pt, function_, p);
#else		
			fprintf(stderr,"%s:: %s\n",function_, p);
#endif	
			// fprintf(stderr,"%s::%d::%s:: %s\n",file_, line_, function_, p);
			// fprintf(stderr,"%s:: %s\n",function_, p);   the old code
			free(p);
			return;
		}
		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */
		if ((np = (char *)realloc (p, size)) == NULL) {
			free(p);
			return;
		} else {
			p = np;
		}
	}
	
	return;
}

void Log:: MSlog(const char *file_, int line_, const char *function_, int priority_, const char * fmt, ...)
{

	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	char *p, *np;
	va_list ap;

	if ((p = (char *)malloc(size)) == NULL)
		return;

	while (1) {
		/* Try to print in the allocated space. */
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size){
			//fprintf(stderr,"%s::%d::%s:: %s\n",file_, line_, function_, p);
			syslog(priority_,"%s\n",p);

			free(p);
			return;
		}
		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */
		if ((np = (char *)realloc (p, size)) == NULL) {
			free(p);
			return;
		} else {
			p = np;
		}
	}
	
	return;
}


int Daemon_Init(const char *path, const char *pname, int facility)
{

#ifdef DAEMON
	pid_t pid;

	if ( (pid = fork()) < 0)
		return (-1);
	else if (pid)
		_exit(0);                       /* parent terminates */

	/* close off file descriptors */
	int i;
	for (i = 0; i < MAXFD; i++)
	      close(i);

	/* redirect stdin, stdout, and stderr to /dev/null */
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	chdir(path);

	openlog(pname, LOG_NDELAY| LOG_NOWAIT |LOG_PID, facility);
#endif

	return (0);                             /* success */
}

