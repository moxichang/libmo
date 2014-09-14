#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(libisrc, 0.1, kikass@outlook.com)
AC_CONFIG_SRCDIR([config.h.in])

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(libisrc, 0.1)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lclntsh':
#AC_CHECK_LIB([clntsh], [main])
# FIXME: Replace `main' with a function in `-ldl':
#AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lisrc':
#AC_CHECK_LIB([isrc], [main])
# FIXME: Replace `main' with a function in `-lm':
#AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lmysqlclient':
#AC_CHECK_LIB([mysqlclient], [main])
# FIXME: Replace `main' with a function in `-lpcap':
#AC_CHECK_LIB([pcap], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_rwlock_init])
AC_PROG_RANLIB
# FIXME: Replace `main' with a function in `-lssl':
# FIXME: Replace `main' with a function in `-lz':
#AC_CHECK_LIB([z], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h syslog.h unistd.h], , [AC_MSG_ERROR(missing head files)])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([bzero gethostbyname localtime_r memset select socket strcasecmp])

# optional modules
# ssl
AC_ARG_ENABLE([ssl], 
              [AC_HELP_STRING([--enable-ssl])],
              [ case "${enableval}" in
                    yes) enable_ssl=yes;;
                    no)  enable_ssl=no;;
                    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-ssl]) ;;
                esac], 
                [enable_ssl=no])

AS_IF([test "$enable_ssl" = "yes"], [AC_CHECK_LIB([ssl], [SSL_library_init], , [AC_MSG_ERROR(openssl lib is required)])])
AS_IF([test "$enable_ssl" = "yes"], [AC_CHECK_LIB([ssl], [SSL_library_init], , [AC_MSG_ERROR(openssl header is required)])])
AS_IF([test "$enable_ssl" = "yes"], [echo "ssl enabled"])

AM_CONDITIONAL([ENABLE_SSL], [test "$enable_ssl" = "yes"])

# other optional modules

AC_OUTPUT([Makefile
                 src/Makefile])
