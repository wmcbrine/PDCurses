dnl Process this file with autoconf to produce a configure script.
AC_INIT
AC_CONFIG_SRCDIR([curspriv.h])

AC_SUBST(prefix)

dnl ---------------------- check for C compiler -----------------
dnl Checks for one of the C compilers below - change if yours not there.
CC_LIST="gcc acc cc c89"
MH_PROG_CC
AC_ISC_POSIX

AC_CONFIG_HEADER(config.h)
dnl Checks for system first
AC_CANONICAL_SYSTEM([])

mymakefile="Makefile"
on_qnx=no
case "$target" in
	*hp-hpux*) 
		SYS_DEFS="-D_HPUX_SOURCE"
		;;
	*ibm-aix*)
		SYS_DEFS="-D_ALL_SOURCE"
		mymakefile="Makefile.aix"
		;;
	*dec-osf*)
		SYS_DEFS="-D_XOPEN_SOURCE_EXTENDED"
		;;
	sparc*solaris*)
		;;
	sparc*sunos*)
		;;
	*linux*)
		;;
	*pc-sco*)
		SYS_DEFS="-UM_XENIX -b elf"
		;;
	*nto-qnx*)
		;;
	*qnx*)
		on_qnx=yes
		SYS_DEFS="-Q"
		EEXTRA="-mf -N0x20000 -Q"
		;;
esac
AC_SUBST(SYS_DEFS)

MH_CHECK_MAX_SIGNALS(NSIG __sys_nsig)

dnl Check for dynamic shared library support libraries
MH_EXTRA_LIBS=""
MH_CHECK_LIB(dld dl)
SHLIBS="${MH_EXTRA_LIBS}"

dnl Check for other programs.
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl ensure that the system has System V IPC support
MH_IPC

if test $ac_cv_header_stdc != yes; then
	AC_MSG_ERROR([Need ANSI C headers])
fi

dnl Checks for libraries.

AC_CHECK_HEADERS(fcntl.h \
	sys/time.h \
	sys/select.h \
	dlfcn.h \
	dl.h \
	errno.h
	)


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
dnl AC_CHECK_TYPE(fd_set,int)
MH_CHECK_LIB(socket nls)
AC_SUBST(MH_EXTRA_LIBS)
MH_CHECK_CC_O

dnl Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(vsscanf usleep poll vsnprintf)

dnl Check for X includes and X libraries
MH_CHECK_X_INC
MH_CHECK_X_LIB
MH_CHECK_X_HEADERS(DECkeysym.h Sunkeysym.h xpm.h)
MH_CHECK_X_KEYDEFS(XK_KP_Delete XK_KP_Insert XK_KP_End XK_KP_Down XK_KP_Next \
		XK_KP_Left XK_KP_Right XK_KP_Home XK_KP_Up XK_KP_Prior XK_KP_Begin)
MH_CHECK_X_TYPEDEF(XPointer)
dnl
dnl extra for xpm library
if test $ac_cv_header_DECkeysym_h = yes; then
	MH_XLIBS="$MH_XLIBS -lXpm"
fi
dnl --------------- allow --enable-debug to compile in debug mode --------
AC_ARG_ENABLE(debug,
	[  --enable-debug          turn on debugging],
	[with_debug=$enableval],
	[with_debug=no],
)
cflags_g="`echo $CFLAGS | grep -c '\-g'`"
cflags_O="`echo $CFLAGS | grep -c '\-O'`"

if test "$with_debug" = yes; then
	if test "$cflags_g" = "0"; then
		CFLAGS="${CFLAGS} -g"
	fi
	if test "$cflags_O" != "0"; then
		CFLAGS="`echo ${CFLAGS} | sed -e s/-O.//`"
	fi
	CFLAGS="${CFLAGS} -DPDCDEBUG"
else
	if test "$cflags_O" = "0"; then
		CFLAGS="${CFLAGS} -O"
	fi
	if test "$cflags_g" != "0"; then
		CFLAGS="`echo ${CFLAGS} | sed -e s/-g//`"
	fi
fi
if test "$ac_cv_prog_CC" = "gcc"; then
	if test "$with_debug" = yes; then
		CFLAGS="${CFLAGS} -Wall"
	else
		CFLAGS="-O2 -Wall -fomit-frame-pointer"
	fi
fi
if test "$on_qnx" = yes; then
	if test "$with_debug" = yes; then
		CFLAGS="-g"
	else
		CFLAGS="-Otax"
	fi
fi

dnl ----------------- check for LATIN1 key support -------------------
dnl allow --enable-latin1 to include LATIN1 key support
AC_ARG_ENABLE(latin1,
	[  --enable-latin1         include support for LATIN1 keys],
	[with_latin1=$enableval],
	[with_latin1=no],
)
if test "$with_latin1" = yes; then
	AC_DEFINE(INCLUDE_LATIN1)
fi

dnl ----------------- check for Unicode support ---------------------
dnl allow --enable-unicode to include Unicode support
AC_ARG_ENABLE(unicode,
	[  --enable-unicode        include support for Unicode],
	[with_unicode=$enableval],
	[with_unicode=no],
)
if test "$with_unicode" = yes; then
	SYS_DEFS="$SYS_DEFS -DUNICODE"
fi

dnl ----------------- check for Purify support -----------------------
dnl allow --enable-purify to enable linking with Purify
AC_ARG_ENABLE(purify,
	[  --enable-purify         link with Purify (TM)],
	[with_purify=$enableval],
	[with_purify=no],
)
if test "$with_purify" = yes; then
	PURIFY="purify"
else
	PURIFY=""
fi
AC_SUBST(PURIFY)

dnl ---------------------- check for Xaw3d library -------------------
dnl allow --with-xaw3d to link with PDCurses
AC_ARG_WITH(xaw3d,
	[  --with-xaw3d            link with Xaw3d],
	[with_xaw3d=$withval],
	[with_xaw3d=no],
)
if test "$with_xaw3d" = yes; then
	AC_DEFINE(USE_XAW3D)
fi

dnl ---------------------- check for neXtaw library -------------------
dnl allow --with-nextaw to link with PDCurses
AC_ARG_WITH(nextaw,
	[  --with-nextaw           link with neXtaw],
	[with_nextaw=$withval],
	[with_nextaw=no],
)
if test "$with_nextaw" = yes; then
	AC_DEFINE(USE_NEXTAW)
fi

dnl --------------- Check how to make shared libraries -------------------
dnl Force the ability of shared library usage
MH_SHARED_LIBRARY(XCurses)

AC_CONFIG_FILES([Makefile x11/$mymakefile tools/Makefile x11/xcurses-config])
AC_OUTPUT

case "$target" in
        *ibm-aix*)
                mv x11/Makefile.aix x11/Makefile
                AC_MSG_RESULT(renaming x11/Makefile.aix to x11/Makefile)
                ;;
        *)
                ;;
esac
