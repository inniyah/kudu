AC_PREREQ(2.59)

AC_INIT([kudu],[0.0.1],[redarrow@users.sourceforge.net])
AC_CONFIG_SRCDIR([src/globals.h])
AC_CONFIG_HEADER([src/config.h])

AC_LANG(C)

AM_INIT_AUTOMAKE

AC_PROG_CC

# Check for Librarys
#AC_CHECK_LIB([pthread], [pthread_create], [PYTHON_PREQ1=1])
#AC_CHECK_LIB([util], [openpty], [PYTHON_PREQ2=1])

AC_PYTHON_DEVEL

#AC_ARG_ENABLE(debug, [  --enable-debug    Turn on debugging],
#	[case "${enableval}" in
#		yes) debug=true ;;
#		no)  debug=false ;;
#	*) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
#	esac],[debug=false])



PKG_CHECK_MODULES(GSTUFF, gtk+-2.0 >= 2.6.0)
AM_PATH_GTKGLEXT_1_0([1.0.6], [], [AC_MSG_ERROR([Error GtkGLExt not found! Cannot continue.])])

# Check for headers
AC_PATH_X
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/timeb.h dirent.h], [], [AC_MSG_ERROR([*** error required header not found])])
AC_CHECK_HEADERS([GL/gl.h GL/glu.h], [], [AC_MSG_ERROR([*** error required header not found])])

AC_C_CONST

AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor ftime sqrt strcasecmp strncasecmp])

#AC_DEFINE_DIR([KUDU_DATADIR], [datadir/package], [Kudu shared data files])

AC_CONFIG_FILES([Makefile src/Makefile images/Makefile scripts/Makefile examples/Makefile])
AC_OUTPUT

if test "$no_python" = yes; then
echo "Kudu will be built without python!"
fi
