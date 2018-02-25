dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(hac-1.0.0, 1.0)

#libtoolize: Consider adding 'AC_CONFIG_MACRO_DIRS([m4])' to configure.ac,
#libtoolize: Consider adding '-I m4' to ACLOCAL_AMFLAGS in Makefile.am.

LT_INIT

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CXX

dnl ###########################################################################
dnl Dependencies
dnl ###########################################################################

#PKG_CHECK_MODULES([HAC], [
#	x11 >= 1.6.4
#])

#MYSQLPP_LIBS=-L/usr/lib/x86_64-linux-gnu -lmariadbclient -lpthread -lz -lm -ldl -lmysqlpp -lmysqlcppconn
#"-L/usr/lib -lmysqlpp -lmysqlcppconn"
#MYSQLPP_CFLAGS="-I/usr/include/mysql++ -I/usr/include/mysql"
#AC_SUBST(MYSQLPP_LIBS)
#AC_SUBST(MYSQLPP_CFLAGS)

dnl  Detect pthreads libraries
PTHREAD_LIBS=error
AC_CHECK_LIB(pthread, pthread_attr_init, PTHREAD_LIBS="-lpthread")
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_LIB(pthreads, pthread_attr_init, PTHREAD_LIBS="-lpthreads")
fi
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_LIB(c_r, pthread_attr_init, PTHREAD_LIBS="-lc_r")
fi
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_FUNC(pthread_attr_init, PTHREAD_LIBS="")
fi
AC_SUBST(PTHREAD_LIBS)

AC_CONFIG_FILES(Makefile src/Makefile src/misc/Makefile src/client/Makefile src/server/Makefile)
AC_OUTPUT
