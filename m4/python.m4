AC_DEFUN([AC_PYTHON_DEVEL],[
	#
	# should allow for checking of python version here...
	#
	have_python=0

	AC_ARG_WITH(python, [  --without-python Build Kudu without python support], [build_python=$withval], [build_python=yes])

	if test x$build_python == xno
	then
		#AC_DEFINE([HAVE_PYTHON_H], [0], [Define to 1 if we have Python.h])
		#AC_DEFINE([HAVE_LIBPYTHON], [0], [Define to 1 if we have the Python libs])
		#AC_DEFINE([SUPPORT_PYTHON], [0], [Define to 1 if we support python])

		AM_CONDITIONAL(SUPPORT_PYTHON, test $have_python = 2)

	else 

		AC_REQUIRE([AM_PATH_PYTHON])

		# Check for Python include path
		AC_MSG_CHECKING([for Python include path])
		python_path=`echo $PYTHON | sed "s,/bin.*$,,"`
		for i in "$python_path/include/python$PYTHON_VERSION/" "$python_path/include/python/" "$python_path/" ; do
			python_path=`find $i -type f -name Python.h -print | sed "1q"`
			if test -n "$python_path" ; then
				break
			fi
		done
		python_path=`echo $python_path | sed "s,/Python.h$,,"`
		AC_MSG_RESULT([$python_path])
		if test -z "$python_path" ; then
			#AC_MSG_ERROR([cannot find Python include path])
			have_python=0
			else
			AC_SUBST([PYTHON_CPPFLAGS],[-I$python_path])
			AC_DEFINE([HAVE_PYTHON_H], [1], [Define to 1 if we have Python.h])
			let have_python+=1
		fi

		# Check for Python library path
		AC_MSG_CHECKING([for Python library path])
		python_path=`echo $PYTHON | sed "s,/bin.*$,,"`
		for i in "$python_path/lib/python$PYTHON_VERSION/config/" "$python_path/lib/python$PYTHON_VERSION/" "$python_path/lib/python/config/" "$python_path/lib/python/" "$python_path/" ; do
			python_path=`find $i -type f -name libpython$PYTHON_VERSION.* -print | sed "1q"`
			if test -n "$python_path" ; then
				break
			fi
		done
		python_path=`echo $python_path | sed "s,/libpython.*$,,"`
		AC_MSG_RESULT([$python_path])
		if test -z "$python_path" ; then
			#AC_MSG_ERROR([cannot find Python library path])
			have_python=0
		else
			let have_python+=1
			AC_DEFINE([HAVE_LIBPYTHON], [1], [Define to 1 if we have the Python libs])
			AC_SUBST([PYTHON_LDFLAGS],["-L$python_path -lpython$PYTHON_VERSION"])
			#
			python_site=`echo $python_path | sed "s/config/site-packages/"`
			AC_SUBST([PYTHON_SITE_PKG],[$python_site])
			#
			# libraries which must be linked in when embedding
			#
			AC_MSG_CHECKING(python extra libraries)
			PYTHON_EXTRA_LIBS=`$PYTHON -c "import distutils.sysconfig; \
				conf = distutils.sysconfig.get_config_var; \
				print conf('LOCALMODLIBS')+' '+conf('LIBS')"
			AC_MSG_RESULT($PYTHON_EXTRA_LIBS)`
			AC_SUBST(PYTHON_EXTRA_LIBS)

		fi

		if test $have_python = 2;
		then	AC_DEFINE([SUPPORT_PYTHON], [1], [Define to 1 if we support python])
		fi

		AM_CONDITIONAL(SUPPORT_PYTHON, test $have_python = 2)

	fi

])


