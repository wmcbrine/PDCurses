dnl ---------------------------------------------------------------------------
dnl This file offers the following common macros...
dnl ---------------------------------------------------------------------------
dnl CHECK_REXX
dnl MH_IPC
dnl MH_CHECK_X_INC
dnl MH_CHECK_X_LIB
dnl MH_HAVE_PROTO
dnl MH_PROG_CC
dnl MH_CHECK_X_HEADERS
dnl MH_CHECK_X_KEYDEFS
dnl MH_CHECK_X_TYPEDEF
dnl MH_TRY_LINK
dnl MH_CHECK_LIB
dnl MH_HOWTO_SHARED_LIBRARY
dnl MH_SHARED_LIBRARY
dnl MH_HOWTO_DYN_LINK
dnl MH_CHECK_CC_O
dnl MH_CHECK_LEADING_USCORE
dnl MH_SHLPST

dnl ---------------------------------------------------------------------------
dnl Check REXX library and header files
dnl ---------------------------------------------------------------------------
AC_DEFUN([CHECK_REXX],
[
AC_REQUIRE([AC_CANONICAL_SYSTEM])
dnl
dnl Setup various things for different interpreters
dnl
extra_rexx_libs=""
extra_rexx_defines=""
orexx_incdirs=""
orexx_libdirs=""
REXX_LIBS=""
REXX_INCLUDES=""
case "$with_rexx" in
	regina)               dnl -------- Regina
		AC_DEFINE(USE_REGINA)
		AC_CHECK_PROG(regina_config, [regina-config], yes, no)
		rexx_h="rexxsaa.h"
		rexx_l="regina"
		REXX_INT="Regina"
		REXX_TARGET="Regina"
		if test "$ac_cv_prog_regina_config" = yes; then
			REXX_INCLUDES=`regina-config --cflags`
			REXX_LIBS=`regina-config --libs`
			REXX_INT="Regina (using regina-config)"
		else
			case "$target" in
				*nto-qnx*)
				AC_SEARCH_LIBS(dlopen,dl)
				;;
				*qnx*)
				;;
				*hp-hpux*)
				AC_SEARCH_LIBS(shl_load,dld)
				;;
				*)
			AC_SEARCH_LIBS(dlopen,dl)
				;;
			esac
			AC_SEARCH_LIBS(crypt,crypt)
			AC_PROG_LEX
		fi
	;;
	rexxtrans)            dnl -------- Rexx/Trans
		AC_DEFINE(USE_REXXTRANS)
		AC_CHECK_PROG(rexxtrans_config, [rexxtrans-config], yes, no)
		rexx_h="rexxtrans.h"
		rexx_l="rexxtrans"
		REXX_INT="Rexx/Trans"
		REXX_TARGET="RexxTrans"
		if test "$ac_cv_prog_rexxtrans_config" = yes; then
			REXX_INCLUDES=`rexxtrans-config --cflags`
			REXX_LIBS=`rexxtrans-config --libs`
			REXX_INT="Rexx/Trans (using rexxtrans-config)"
		else
			AC_SEARCH_LIBS(dlopen,dl)
		fi
	;;
	objrexx)              dnl -------- Object Rexx
		AC_DEFINE(USE_OREXX)
		rexx_h="rexx.h"
		rexx_l="rexxapi"
		REXX_INT="Object Rexx"
		REXX_TARGET="ObjectRexx"
		extra_rexx_libs="-lrexx"
		orexx_incdirs="/opt/orexx /usr/local/orexx"
		orexx_libdirs="/opt/orexx/lib /usr/local/orexx/lib"
		case "$target" in
			*linux*)
			extra_rexx_defines="-DLINUX"
			;;
			*)
			;;
		esac
	AC_SEARCH_LIBS(pthread_create,pthread pthreads thread)
	;;
	rexximc)              dnl -------- Rexx/imc
		AC_DEFINE(USE_REXXIMC)
		rexx_h="rexxsaa.h"
		rexx_l="rexx"
		REXX_INT="REXX/imc"
		REXX_TARGET="REXXimc"
		AC_SEARCH_LIBS(dlopen,dl)
	;;
	rexx6000)             dnl -------- REXX6000
dnl
dnl Check that the OS supports REXX/6000
dnl Only supported it on AIX
dnl
		if test "$with_rexx6000" = yes ; then
		case "$target" in
			*aix*)
				;;
			*)
					AC_MSG_ERROR(REXX/6000 support only available on AIX; cannot configure)
					;;
		esac
	fi
		AC_DEFINE(USE_REXX6000)
		rexx_h="rexxtype.h"
		rexx_l="rexx"
		REXX_INT="REXX/6000"
		REXX_TARGET="REXX6000"
	;;
	unirexx)              dnl -------- uni-REXX
		AC_DEFINE(USE_UNIREXX)
		rexx_h="rexxsaa.h"
		rexx_l="rx"
		REXX_INT="uni-REXX"
		REXX_TARGET="uni-REXX"
		AC_SEARCH_LIBS(dlopen,dl)
	;;
	none)                 dnl -------- No Rexx interpreter
		AC_DEFINE(NOREXX)
		rexx_h=""
		rexx_l=""
		REXX_INT="No Rexx Support"
		REXX_TARGET=""
	;;
	*)
		AC_MSG_ERROR(No Rexx interpreter specified with --with-rexx=int : must be one of: regina rexximc objrexx unirexx rexx6000 rexxtrans none)
	;;
esac
AC_SUBST(REXX_TARGET)

dnl look for REXX header and library, exit if not found

if test "xx$rexx_h" != "xx" -a "x$REXX_INCLUDES" = "x"; then
dnl look for REXX header and library, exit if not found
	AC_MSG_CHECKING(for location of Rexx header file: $rexx_h)
	mh_rexx_inc_dir=""
	mh_inc_dirs="\
	    ${REXXINCDIR}             \
	    ${orexx_incdirs}          \
	    ${HOME}/include           \
	    /usr/local/include        \
	    /usr/contrib/include      \
	    /opt/include              \
	    /usr/include              \
	    /usr/unsupported/include"
dnl
dnl Provide for user supplying directory
dnl
	if test "$with_rexxincdir" != no ; then
		mh_inc_dirs="$with_rexxincdir $mh_inc_dirs"
	fi
dnl
dnl Try to determine the directory containing Rexx header
dnl
	for ac_dir in $mh_inc_dirs ; do
	  if test -r $ac_dir/$rexx_h; then
	    mh_rexx_inc_dir=$ac_dir
	    break
	  fi
	done
	if test "x$mh_rexx_inc_dir" != "x" ; then
		REXX_INCLUDES="-I$mh_rexx_inc_dir $extra_rexx_defines"
		AC_MSG_RESULT(found in $mh_rexx_inc_dir)
	else
		AC_MSG_ERROR(Cannot find Rexx header file: $rexx_h; cannot configure)
	fi
fi
AC_SUBST(REXX_INCLUDES)

if test "xx$rexx_h" != "xx" -a "x$REXX_LIBS" = "x"; then
	AC_MSG_CHECKING(for location of Rexx library file: $rexx_l)
	mh_rexx_lib_dir=""
	mh_lib_dirs="\
	    ${REXXLIBDIR}             \
	    ${orexx_libdirs}          \
	    ${HOME}/lib               \
	    /usr/local/lib            \
	    /usr/contrib/lib          \
	    /opt/lib                  \
	    /opt/sfw/lib              \
	    /usr/lib                  \
	    /usr/unsupported/lib"
dnl
dnl Provide for user supplying directory
dnl
	if test "$with_rexxlibdir" != no ; then
		mh_lib_dirs="$with_rexxlibdir $mh_lib_dirs"
	fi
dnl
dnl Try to determine the directory containing Rexx library
dnl
	for ac_dir in $mh_lib_dirs ; do
		for mh_ext in lib${rexx_l}.a lib${rexx_l}.so lib${rexx_l}.sl ${rexx_l}.lib lib${rexx_l}.dylib; do
		  if test -r $ac_dir/$mh_ext; then
		     mh_rexx_lib_dir=$ac_dir
		   break 2
		  fi
		done
	done
	if test "x$mh_rexx_lib_dir" != "x" ; then
		REXX_LIBS="-L$mh_rexx_lib_dir -l$rexx_l $extra_rexx_libs"
		AC_MSG_RESULT(found in $mh_rexx_lib_dir)
		AC_SUBST(rexxtrans_lib_name)
	else
		AC_MSG_ERROR(Cannot find Rexx library file: $rexx_l; cannot configure)
	fi
fi
AC_SUBST(REXX_LIBS)
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine if the system has System V IPC. ie sys/ipc.h and sys/shm.h
dnl headers.
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_IPC],
[
AC_MSG_CHECKING(for System V IPC support)
AC_CHECK_HEADER(sys/ipc.h)
if test $ac_cv_header_sys_ipc_h = no; then
	AC_MSG_ERROR(Cannot find required header file sys/ipc.h; XCurses cannot be configured)
fi
])dnl

dnl ---------------------------------------------------------------------------
dnl Set up the correct X header file location
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_INC],
[
AC_MSG_CHECKING(for location of X headers)
mh_x11_dir=""
mh_x11_xaw_dir=""
dnl
dnl specify latest release of X directories first
dnl
mh_inc_dirs="\
    $HOME/include/X11         \
    $HOME/include             \
    /tmp/include/X11          \
    /tmp/include              \
    /usr/X11R6/include        \
    /usr/include/X11R6        \
    /usr/local/X11R6/include  \
    /usr/local/include/X11R6  \

    /usr/X11R5/include        \
    /usr/include/X11R5        \
    /usr/local/X11R5/include  \
    /usr/local/include/X11R5  \
    /usr/local/x11r5/include  \

    /usr/X11R4/include        \
    /usr/include/X11R4        \
    /usr/local/X11R4/include  \
    /usr/local/include/X11R4  \
                              \
    /usr/X11/include          \
    /usr/include/X11          \
    /usr/local/X11/include    \
    /usr/local/include/X11    \
                              \
    /usr/X386/include         \
    /usr/x386/include         \
    /usr/XFree86/include/X11  \
                              \
    /usr/include              \
    /usr/local/include        \
    /usr/unsupported/include  \
    /usr/athena/include       \
    /usr/lpp/Xamples/include  \
                              \
    /usr/openwin/include      \
    /usr/openwin/share/include"
dnl
dnl Provide for user supplying directory
dnl
if test "x$x_includes" != xNONE ; then
	mh_inc_dirs="$x_includes $mh_inc_dirs"
fi

dnl
dnl Try to determine the directory containing X headers
dnl We will append X11 to all the paths above as an extra check
dnl
for ac_dir in $mh_inc_dirs ; do
  if test -r $ac_dir/Intrinsic.h; then
    mh_x11_dir=$ac_dir
    break
  fi
  if test -r $ac_dir/X11/Intrinsic.h; then
    mh_x11_dir="$ac_dir/X11"
    break
  fi
done

dnl
dnl Try to determine the directory containing Xaw headers
dnl We will append X11 to all the paths above as an extra check
dnl
if test "$with_xaw3d" = yes; then
	mh_xaw_dir="Xaw3d"
else
	if test "$with_nextaw" = yes; then
		mh_xaw_dir="neXtaw"
	else
		mh_xaw_dir="Xaw"
	fi
fi
for ac_dir in $mh_inc_dirs ; do
  if test -r $ac_dir/$mh_xaw_dir/Box.h; then
    mh_x11_xaw_dir=$ac_dir
    break
  fi
  if test -r $ac_dir/X11/$mh_xaw_dir/Box.h; then
    mh_x11_xaw_dir="$ac_dir/X11"
    break
  fi
done

if test "x$mh_x11_dir" != "x" ; then
	mh_x11_dir_no_x11=`echo $mh_x11_dir | sed 's/\/X11$//'`
dnl
dnl Test to see if $mh_x11_dir_no_x11 is /usr/include and we are using gcc
dnl under Solaris. If so, ignore it.
	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	mh_solaris_gcc_usr_include="no"
	case "$target" in
		*solaris*)
			if test "$ac_cv_prog_CC" = "gcc" -a "$mh_x11_dir_no_x11" = "/usr/include" ; then
				mh_solaris_gcc_usr_include="yes"
			fi
			;;
	esac
	if test "$mh_x11_dir_no_x11" != "$mh_x11_dir" -a "$mh_solaris_gcc_usr_include" = "no" ; then
		MH_XINC_DIR="-I$mh_x11_dir -I$mh_x11_dir_no_x11"
	else
		MH_XINC_DIR="-I$mh_x11_dir"
	fi
else
	AC_MSG_ERROR(Cannot find required header file Intrinsic.h; XCurses cannot be configured)
fi

if test "x$mh_x11_xaw_dir" != "x" ; then
	MH_XINC_DIR="-I$mh_x11_xaw_dir $MH_XINC_DIR"
else
	AC_MSG_ERROR(Cannot find required Xaw header file Box.h; XCurses cannot be configured)
fi

	AC_MSG_RESULT(found in $mh_x11_dir $mh_x11_xaw_dir)
	AC_SUBST(MH_XINC_DIR)

])dnl

dnl ---------------------------------------------------------------------------
dnl Set up the correct X library file location
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_LIB],
[
dnl
dnl Some systems require extra libraries...
dnl
mh_solaris_flag=no
mh_hpux9_flag=no
AC_REQUIRE([AC_CANONICAL_SYSTEM])
case "$target" in
	*solaris*)
		mh_solaris_flag=yes
		;;
	*pc-sco*)
		extra_x_libs="Xext"
		;;
	sparc*sunos*)
		extra_x_libs="Xext"
		if test "$ac_cv_prog_CC" = "gcc" ; then
			extra_ld_flags="-Wl,-Bstatic"
			extra_ld_flags2="-Wl,-Bdynamic"
		else
			extra_ld_flags="-Bstatic"
			extra_ld_flags2="-Bdynamic"
		fi
		;;
	*hpux9*)
		mh_hpux9_flag=yes
		;;
esac

AC_MSG_CHECKING(for location of X libraries)
if test "$with_xaw3d" = yes; then
	MH_X11_LIBS="Xaw3d Xmu Xt X11"
else
	if test "$with_nextaw" = yes; then
		MH_X11_LIBS="neXtaw Xmu Xt X11"
	else
		MH_X11_LIBS="Xaw Xmu Xt X11"
	fi
fi
MH_X11R6_LIBS="SM ICE Xext"
mh_x11r6=no
dnl
dnl specify latest release of X directories first
dnl
mh_lib_dirs="\
    $HOME/lib             \
    /tmp/lib              \
    /usr/X11R6/lib        \
    /usr/lib/X11R6        \
    /usr/local/X11R6/lib  \
    /usr/local/lib/X11R6  \

    /usr/X11R5/lib        \
    /usr/lib/X11R5        \
    /usr/local/X11R5/lib  \
    /usr/local/lib/X11R5  \
    /usr/local/x11r5/lib  \

    /usr/X11R4/lib        \
    /usr/lib/X11R4        \
    /usr/local/X11R4/lib  \
    /usr/local/lib/X11R4  \
                          \
    /usr/X11/lib          \
    /usr/lib/X11          \
    /usr/local/X11/lib    \
    /usr/local/lib/X11    \
                          \
    /usr/X386/lib         \
    /usr/x386/lib         \
    /usr/XFree86/lib/X11  \
                          \
    /usr/lib              \
    /usr/local/lib        \
    /usr/unsupported/lib  \
    /usr/athena/lib       \
    /usr/lpp/Xamples/lib  \
                          \
    /usr/openwin/lib      \
    /usr/openwin/share/lib"
dnl
dnl Provide for user supplying directory
dnl
if test "x$x_libraries" != xNONE ; then
	mh_lib_dirs="$x_libraries $mh_lib_dirs"
fi

dnl
dnl try to find libSM.[a,sl,so,dylib]. If we find it we are using X11R6
dnl
for ac_dir in $mh_lib_dirs ; do
	for mh_xsm in libSM.a libSM.so libSM.sl libSM.dylib; do
	  if test -r $ac_dir/$mh_xsm; then
	    mh_x11r6=yes
	    break 2
	  fi
	done
done

if test "$mh_x11r6" = yes ; then
	mh_libs="$MH_X11_LIBS $MH_X11R6_LIBS"
else
	mh_libs="$MH_X11_LIBS $extra_x_libs"
fi

dnl
dnl Ensure that all required X libraries are found
dnl
mh_prev_dir=""
mh_where_found=""
mh_where_found_dirs=""
mh_solaris_path=""
for mh_lib in $mh_libs; do
  mh_lib_found=no
  for ac_dir in $mh_lib_dirs ; do
    for mh_ext in a so sl; do
      if test -r $ac_dir/lib$mh_lib.$mh_ext; then
        if test "x$mh_prev_dir" != "x$ac_dir" ; then
          if test "x$mh_prev_dir" = "x" ; then
             mh_where_found="$mh_where_found found in $ac_dir"
          else
             mh_where_found="$mh_where_found and in $ac_dir"
          fi
          mh_prev_dir=$ac_dir
          mh_where_found_dirs="$mh_where_found_dirs $ac_dir"
          MH_XLIBS="$MH_XLIBS -L$ac_dir"
          mh_solaris_path="${mh_solaris_path}:$ac_dir"
        fi
        MH_XLIBS="$MH_XLIBS -l$mh_lib"
        mh_lib_found=yes
        break 2
      fi
    done
  done
  if test "$mh_lib_found" = no; then
    AC_MSG_ERROR(Cannot find required X library; lib$mh_lib. XCurses cannot be configured)
  fi
done
AC_MSG_RESULT($mh_where_found)
mh_solaris_path=`echo $mh_solaris_path | sed 's/^://'`
if test "$mh_solaris_flag" = yes ; then
	MH_XLIBS="-R$mh_solaris_path $extra_ld_flags $MH_XLIBS $extra_libs $extra_ld_flags2"
else
	MH_XLIBS="$extra_ld_flags $MH_XLIBS $extra_libs $extra_ld_flags2"
fi
if test "$mh_hpux9_flag" = yes ; then
  grep -q XtSetLanguageProc $mh_x11_dir/Intrinsic.h
  if test $? -eq 0 ; then
    mh_found_xtshellstrings=no
    for mh_acdir in $mh_where_found_dirs ; do
      for mh_xaw in `ls $mh_acdir/libXaw.*` ; do
        nm $mh_xaw | grep XtShellStrings | grep -qv extern
        if test $? -eq 0 ; then
          mh_found_xtshellstrings=yes
        fi
      done
    done
    if test "$mh_found_xtshellstrings" = no ; then
      AC_MSG_WARN(The X11 development environment has not been installed correctly.)
      AC_MSG_WARN(The header file; Intrinsic.h, is for X11R5 while the Athena Widget)
      AC_MSG_WARN(Set library; libXaw is for X11R4.  This is a common problem with)
      AC_MSG_WARN(HP-UX 9.x.)
      AC_MSG_WARN(A set of required X11R5 library files can be obtained from the)
      AC_MSG_WARN(anonymous ftp sites listed on the PDCurses WWW home page:)
      AC_MSG_WARN(http://www.lightlink.com/hessling/)
      AC_MSG_WARN(The file is called HPUX-9.x-libXaw-libXmu.tar.Z)
      AC_MSG_ERROR(X11 installation incomplete; cannot continue)
    fi
  fi
fi
	AC_SUBST(MH_XLIBS)
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine if C compiler handles ANSI prototypes
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_HAVE_PROTO],
[
AC_MSG_CHECKING(if compiler supports ANSI prototypes)
dnl
dnl override existing value of $ac_compile so we use the correct compiler
dnl SHOULD NOT NEED THIS
dnl
ac_compile='$ac_cv_prog_CC conftest.$ac_ext $CFLAGS $CPPFLAGS -c 1>&5 2>&5'
AC_TRY_COMPILE([#include <stdio.h>],
[extern int xxx(int, char *);],
  mh_have_proto=yes; AC_DEFINE(HAVE_PROTO), mh_have_proto=no )
AC_MSG_RESULT($mh_have_proto)
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine the best C compiler to use given a list
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_PROG_CC],
[
mh_sysv_incdir=""
mh_sysv_libdir=""
all_words="$CC_LIST"
ac_dir=""
AC_MSG_CHECKING(for one of the following C compilers: $all_words)
AC_CACHE_VAL(ac_cv_prog_CC,[
if test -n "$CC"; then
  ac_cv_prog_CC="$CC" # Let the user override the test.
else
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for mh_cc in $all_words; do
    for ac_dir in $PATH; do
      test -z "$ac_dir" && ac_dir=.
      if test -f $ac_dir/$mh_cc; then
        ac_cv_prog_CC="$mh_cc"
        if test "$ac_dir" = "/usr/5bin"; then
          mh_sysv_incdir="/usr/5include"
          mh_sysv_libdir="/usr/5lib"
        fi
        break 2
      fi
    done
  done
  IFS="$ac_save_ifs"
  test -z "$ac_cv_prog_CC" && ac_cv_prog_CC="cc"
fi
CC="$ac_cv_prog_CC"
])
AC_SUBST(CC)
if test "$ac_dir" = ""; then
   AC_MSG_RESULT(using $ac_cv_prog_CC specified in CC env variable)
else
   AC_MSG_RESULT(using $ac_dir/$ac_cv_prog_CC)
fi
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine if the supplied X headers exist.
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_HEADERS],
[
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $MH_XINC_DIR"
for mh_header in $1; do
	AC_CHECK_HEADERS($mh_header)
done
CPPFLAGS="$save_CPPFLAGS"
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine if various key definitions exist in keysym.h
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_KEYDEFS],
[
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $MH_XINC_DIR"
for mh_keydef in $1; do
	AC_MSG_CHECKING(for $mh_keydef in keysym.h)
	mh_upper_name="HAVE_`echo $mh_keydef | tr '[a-z]' '[A-Z]'`"
	AC_TRY_COMPILE([#include <keysym.h>],
[int i = $mh_keydef;],
  mh_have_key=yes; AC_DEFINE_UNQUOTED($mh_upper_name,1), mh_have_key=no )
	AC_MSG_RESULT($mh_have_key)
done
CPPFLAGS="$save_CPPFLAGS"
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine if supplied types have been typedefed
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_TYPEDEF],
[
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $SYS_DEFS $MH_XINC_DIR"
for td in $1 ; do
AC_MSG_CHECKING(if $td is typedefed:)
AC_TRY_COMPILE(
[#include "X11/Xlib.h"],
[$td fred],
	[mh_td=yes],
	[mh_td=no]
)
if test "$mh_td" = yes ; then
	TD_upper=`echo $td | tr a-z A-Z`
	AC_DEFINE_UNQUOTED(${TD_upper}_TYPEDEFED, 1)
fi
AC_MSG_RESULT($mh_td)
done
CPPFLAGS="$save_CPPFLAGS"
])dnl

dnl ---------------------------------------------------------------------------
dnl Provide our own AC_TRY_LINK
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_TRY_LINK],
[
mh_link='${LD-ld} -o conftest $CFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS 1>&AC_FD_CC'
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $MH_XINC_DIR"
for mh_keydef in $1; do
	AC_MSG_CHECKING(for $mh_keydef in keysym.h)
	mh_upper_name="HAVE_`echo $mh_keydef | tr '[a-z]' '[A-Z]'`"
	AC_TRY_COMPILE([#include <keysym.h>],
[int i = $mh_keydef;],
  mh_have_key=yes; AC_DEFINE_UNQUOTED($mh_upper_name,1), mh_have_key=no )
	AC_MSG_RESULT($mh_have_key)
done
CPPFLAGS="$save_CPPFLAGS"
])dnl

dnl ---------------------------------------------------------------------------
dnl Check for presense of various libraries
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_LIB],
[
MH_EXTRA_LIBS=''
for mh_lib in $1; do
	if test "$on_qnx" = yes; then
		AC_MSG_CHECKING(for library -l${mh_lib})
		if test -r /usr/lib/${mh_lib}3r.lib; then
			AC_MSG_RESULT(found)
			MH_EXTRA_LIBS="${MH_EXTRA_LIBS} -l${mh_lib}"
		else
		AC_MSG_RESULT(not found)
		fi
	else
		AC_CHECK_LIB($mh_lib,main,mh_lib_found=yes,mh_lib_found=no)
		if test "$mh_lib_found" = yes; then
			MH_EXTRA_LIBS="${MH_EXTRA_LIBS} -l${mh_lib}"
		fi
	fi
done
])dnl

dnl ---------------------------------------------------------------------------
dnl Work out how to create a shared library
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_HOWTO_SHARED_LIBRARY],
[
AC_MSG_CHECKING(how to create a shared library)
mh_compile='${CC-cc} -c $DYN_COMP conftest.$ac_ext 1>&AC_FD_CC'
cat > conftest.$ac_ext <<EOF
dnl [#]line __oline__ "[$]0"
[#]line __oline__ "configure"
int foo()
{
return(0);
}
EOF
if AC_TRY_EVAL(mh_compile) && test -s conftest.o; then
	mh_dyn_link='ld -shared -o conftest.so.1.0 conftest.o -lc 1>&AC_FD_CC'
#	mh_dyn_link='${CC} -Wl,-shared -o conftest.so.1.0 conftest.o -lc 1>&AC_FD_CC'
	if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
		SHL_LD="ld -shared -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'" -lc"
#		SHL_LD="${CC} -Wl,-shared -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'" -lc"
	else
		mh_dyn_link='ld -G -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
#		mh_dyn_link='${CC} -Wl,-G -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
		if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
			SHL_LD="ld -G -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#			SHL_LD="${CC} -Wl,-G -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
		else
			mh_dyn_link='ld -o conftest.so.1.0 -shared -no_archive conftest.o  -lc 1>&AC_FD_CC'
#			mh_dyn_link='${CC} -o conftest.so.1.0 -Wl,-shared,-no_archive conftest.o  -lc 1>&AC_FD_CC'
			if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
				SHL_LD="ld -o ${SHLPRE}${SHLFILE}${SHLPST} -shared -no_archive "'$('SHOFILES')'" -lc"
#				SHL_LD="${CC} -o ${SHLPRE}${LIBFILE}${SHLPST} -Wl,-shared,-no_archive "'$('SHOFILES')'" -lc"
			else
				mh_dyn_link='ld -b -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
#				mh_dyn_link='${CC} -Wl,-b -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
				if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
					SHL_LD="ld -b -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#					SHL_LD="${CC} -Wl,-b -o ${SHLPRE}${SHLFILE}.${SHLPST} "'$('SHOFILES')'
				else
					mh_dyn_link='ld -Bshareable -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
#					mh_dyn_link='${CC} -Wl,-Bshareable -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
					if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
						SHL_LD="ld -Bshareable -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#						SHL_LD="${CC} -Wl,-Bshareable -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
					else
						mh_dyn_link='ld -assert pure-text -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
#						mh_dyn_link='${CC} -Wl,-assert pure-text -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
						if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
							SHL_LD="ld -assert pure-text -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#							SHL_LD="${CC} -Wl,-assert pure-text -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
						else
							SHL_LD=""
						fi
					fi
				fi
			fi
		fi
	fi
fi
if test "$SHL_LD" = ""; then
	AC_MSG_RESULT(unknown)
else
	AC_MSG_RESULT(found)
fi
rm -f conftest*
])

dnl ---------------------------------------------------------------------------
dnl Work out how to create a dynamically loaded module
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_HOWTO_DYN_LINK],
[
mh_compile='${CC-cc} -c $DYN_COMP conftest.$ac_ext 1>&AC_FD_CC'
cat > conftest.$ac_ext <<EOF
dnl [#]line __oline__ "[$]0"
[#]line __oline__ "configure"
int foo()
{
return(0);
}
EOF
if AC_TRY_EVAL(mh_compile) && test -s conftest.o; then
	mh_dyn_link='ld -shared -o conftest.rxlib conftest.o -lc 1>&AC_FD_CC'
#	mh_dyn_link='${CC} -Wl,-shared -o conftest.rxlib conftest.o -lc 1>&AC_FD_CC'
	if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.rxlib; then
		LD_RXLIB1="ld -shared"
#		LD_RXLIB1="${CC} -Wl,-shared"
		LD_RXLIB2="${REXX_LIBS}"
		SHLPRE="lib"
		SHLPST=".so"
		RXLIBLEN="6"
	else
		mh_dyn_link='ld -G -o conftest.rxlib conftest.o 1>&AC_FD_CC'
#		mh_dyn_link='${CC} -Wl,-G -o conftest.rxlib conftest.o 1>&AC_FD_CC'
		if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.rxlib; then
			LD_RXLIB1="ld -G"
#			LD_RXLIB1="${CC} -Wl,-G"
			LD_RXLIB2="${REXX_LIBS}"
			SHLPRE="lib"
			SHLPST=".so"
			RXLIBLEN="6"
		else
			LD_RXLIB1=""
			LD_RXLIB2=""
			SHLPRE=""
			SHLPST=""
			RXLIBLEN="0"
		fi
	fi
fi
rm -f conftest*
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine how to build shared libraries etc..
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_SHARED_LIBRARY],
[
AC_REQUIRE([MH_SHLPST])
dnl
dnl If compiler is gcc, then flags should be the same for all platforms
dnl (just guessing on this)
dnl
OSAVE=".o.save"
OBJ="o"
EXE=""
GETOPT=""
STATIC_LDFLAGS=""
DYNAMIC_LDFLAGS=""
AIX_DYN="no"
BEOS_DYN="no"
SHLFILE="$1"
SHLFILES="$*"
RXPACKEXPORTS=""
BASE_INSTALL="installbase"
BASE_BINARY="binarybase"
SHLPRE="lib"
LD_RXLIB1=""
CAN_USE_ABI="no"
LIBEXE="ar"
LIBFLAGS="cr"
LIBPRE="lib"
LIBPST=".a"
# OTHER_INSTALLS is set to install a non-version numbered shared library
# by default; ie. no .\$(ABI) suffix. If the regina executable is not built,
# then there is no shared library. Set OTHER_INSTALLS="installabilib" if you
# are building a version numbered shared library.
OTHER_INSTALLS="installlib"
EXTRATARGET=""

AC_REQUIRE([AC_CANONICAL_SYSTEM])
case "$target" in
	*hp-hpux*)
		SYS_DEFS="-D_HPUX_SOURCE"
		EEXTRA="-Wl,-E"
		LD_RXLIB1="ld -b -q -n"
		DYNAMIC_LDFLAGS="-Wl,+s"
		;;
	*ibm-aix*)
		if test "$with_rexx6000" = yes; then
			mh_entry="-eInitFunc"
		else
			mh_entry="-bnoentry"
		fi
		SYS_DEFS="-D_ALL_SOURCE -DAIX"
		AIX_DYN="yes"
		DYN_COMP="-DDYNAMIC"
		STATIC_LDFLAGS="-bnso -bI:/lib/syscalls.exp"
		LD_RXLIB1="ld $mh_entry -bM:SRE"
		RXPACKEXPORTS="-bE:$SHLFILE.exp"
		RXPACKEXP="$SHLFILE.exp"
		;;
	*dec-osf*)
		if test "$ac_cv_prog_CC" = "gcc"; then
			SYS_DEFS="-D_POSIX_SOURCE -D_XOPEN_SOURCE"
		else
			SYS_DEFS="-D_POSIX_SOURCE -D_XOPEN_SOURCE -Olimit 800"
		fi
		LD_RXLIB1="ld -shared"
		;;
	*sequent-dynix*)
		LD_RXLIB1="ld -G"
		;;
	*solaris*)
		if test "$ac_cv_prog_CC" = "gcc"; then
			LD_RXLIB1="gcc -shared"
		else
			LD_RXLIB1="ld -G"
		fi
		;;
	*esix*)
		LD_RXLIB1="ld -G"
		;;
	*dgux*)
		LD_RXLIB1="ld -G"
		;;
	sparc*sunos*)
		SYS_DEFS="-DSUNOS -DSUNOS_STRTOD_BUG"
		LD_RXLIB1="ld"
		;;
	*linux*)
		LD_RXLIB1="${CC} -shared"
		CAN_USE_ABI="yes"
		if test "$USE_ABI" = "yes"; then
			OTHER_INSTALLS="installabilib"
		fi
		;;
	*atheos*)
		LD_RXLIB1="${CC} -shared"
		;;
	*freebsd*)
		LD_RXLIB1="ld -Bdynamic -Bshareable"
		;;
	*pc-sco*)
		LD_RXLIB1="ld -dy -G"
		;;
	*beos*)
		LD_RXLIB1="${CC} -Wl,-shared -nostart -Xlinker -soname=\$(@)"
		BEOS_DYN="yes"
		BASE_INSTALL="beosinstall"
		BASE_BINARY="beosbinary"
		OTHER_INSTALLS=""
		;;
	*nto-qnx*)
		LD_RXLIB1="${CC} -shared"
		;;
	*qnx*)
		LIBPRE=""
		LIBPST=".lib"
		SHLPRE=""
		DYN_COMP="-Q"   # force no check for dynamic loading
		SHLFILE=""
		EEXTRA="-mf -N0x20000 -Q"
		;;
	*cygwin)
		LIBPRE="lib"
		SHLPRE=""
		DYN_COMP="-DDYNAMIC"
		LIBPST=".a"
		EXE=".exe"
		LD_RXLIB1="dllwrap --def \$(srcdir)/\$(basename \$(@))w32.def --output-lib ${LIBPRE}\$(basename \$(@))${LIBPST} --target i386-cygwin32 --dllname \$(@)"
		BASE_INSTALL="cygwininstall"
		BASE_BINARY="cygwinbinary"
		;;
	*darwin*)
		DYN_COMP="-fno-common"
		LD_RXLIB1="${CC} -dynamiclib -install_name=\$(@)"
		;;
	*)
		;;
esac

dnl
dnl determine what switches our compiler uses for building objects
dnl suitable for inclusion in shared libraries
dnl Only call this if DYN_COMP is not set. If we have set DYN_COMP
dnl above, then we know how to compile AND link for dynamic libraries
dnl
if test "$DYN_COMP" = ""; then
AC_MSG_CHECKING(compiler flags for a dynamic object)

cat > conftest.$ac_ext <<EOF
dnl [#]line __oline__ "[$]0"
[#]line __oline__ "configure"
int a=0
EOF

	DYN_COMP=""
	mh_cv_stop=no
	save_cflags="$CFLAGS"
	mh_cv_flags="-fPIC -KPIC +Z"
	for a in $mh_cv_flags; do
		CFLAGS="-c $a"

		mh_compile='${CC-cc} -c $CFLAGS conftest.$ac_ext > conftest.tmp 2>&1'
		if AC_TRY_EVAL(mh_compile); then
			DYN_COMP=""
		else
			slash="\\"
			mh_dyncomp="`egrep -c $slash$a conftest.tmp`"
			if test "$mh_dyncomp" = "0"; then
				DYN_COMP="$a -DDYNAMIC"
				AC_MSG_RESULT($a)
				break
			else
				DYN_COMP=""
			fi
		fi
	done
	if test "$DYN_COMP" = ""; then
		AC_MSG_RESULT(none of $mh_cv_flags supported)
	fi
	if test "$LD_RXLIB1" = ""; then
		MH_HOWTO_DYN_LINK()
	fi
	CFLAGS=$save_cflags
	rm -f conftest.*
fi

if test "$ac_cv_header_dl_h" = "yes" -o "$ac_cv_header_dlfcn_h" = "yes" -o "$AIX_DYN" = "yes" -o "$BEOS_DYN" = "yes" -o "$DLFCNINCDIR" != "" -o "$DLFCNLIBDIR" != ""; then
	if test "$with_rexx" = "rexxtrans" -o "$with_rexx" = "regina" -o  "$with_rexx" = "objrexx" -o "$with_rexx" = "rexx6000"; then
		SHL_TARGETS=""
		for a in $SHLFILES
		do
			if test "$USE_ABI" = "yes" -a "$CAN_USE_ABI" = "yes"; then
				SHL_TARGETS="${SHL_TARGETS} ${SHLPRE}${a}${SHLPST}.\$(ABI)"
			else
				SHL_TARGETS="${SHL_TARGETS} ${SHLPRE}${a}${SHLPST}"
			fi
		done
	else
		SHL_TARGETS=""
	fi
else
	SHL_TARGETS=""
fi

AC_SUBST(EEXTRA)
AC_SUBST(CEXTRA)
AC_SUBST(OSAVE)
AC_SUBST(OBJ)
AC_SUBST(EXE)
AC_SUBST(GETOPT)
AC_SUBST(DYN_COMP)
AC_SUBST(LIBS)
AC_SUBST(SHLIBS)
AC_SUBST(LD_RXLIB1)
AC_SUBST(SHLPRE)
AC_SUBST(SHLPST)
AC_SUBST(LIBPST)
AC_SUBST(LIBPRE)
AC_SUBST(LIBEXE)
AC_SUBST(LIBFLAGS)
AC_SUBST(DYNAMIC_LDFLAGS)
AC_SUBST(STATIC_LDFLAGS)
AC_SUBST(SHL_TARGETS)
AC_SUBST(O2SAVE)
AC_SUBST(O2SHO)
AC_SUBST(CC2O)
AC_SUBST(BASE_INSTALL)
AC_SUBST(OTHER_INSTALLS)
AC_SUBST(BASE_BINARY)
AC_SUBST(SAVE2O)
AC_SUBST(RXPACKEXPORTS)
AC_SUBST(RXPACKEXP)
AC_SUBST(CAN_USE_ABI)
])dnl

dnl ---------------------------------------------------------------------------
dnl Check if C compiler supports -c -o file.ooo
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_CC_O],
[
AC_MSG_CHECKING(whether $CC understand -c and -o together)
set dummy $CC; ac_cc="`echo [$]2 |
changequote(, )dnl
		       sed -e 's/[^a-zA-Z0-9_]/_/g' -e 's/^[0-9]/_/'`"
changequote([, ])dnl
AC_CACHE_VAL(ac_cv_prog_cc_${ac_cc}_c_o,
[echo 'foo(){}' > conftest.c
# We do the test twice because some compilers refuse to overwrite an
# existing .o file with -o, though they will create one.
eval ac_cv_prog_cc_${ac_cc}_c_o=no
ac_try='${CC-cc} -c conftest.c -o conftest.ooo 1>&AC_FD_CC'
if AC_TRY_EVAL(ac_try) && test -f conftest.ooo && AC_TRY_EVAL(ac_try);
then
  ac_try='${CC-cc} -c conftest.c -o conftest.ooo 1>&AC_FD_CC'
  if AC_TRY_EVAL(ac_try) && test -f conftest.ooo && AC_TRY_EVAL(ac_try);
  then
    eval ac_cv_prog_cc_${ac_cc}_c_o=yes
  fi
fi
rm -f conftest*
])dnl
if eval "test \"`echo '$ac_cv_prog_cc_'${ac_cc}_c_o`\" = yes"; then
	O2SHO=""
	O2SAVE=""
	SAVE2O=""
	CC2O="-o $"'@'
	AC_MSG_RESULT(yes)
else
	O2SHO="-mv \`basename "$'@'" .sho\`.o "$'@'
	O2SAVE="-mv \`basename "$'@'" .sho\`.o \`basename "$'@'" .sho\`.o.save"
	SAVE2O="-mv \`basename "$'@'" .sho\`.o.save \`basename "$'@'" .sho\`.o"
	CC2O=""
	AC_MSG_RESULT(no)
fi
])

dnl ---------------------------------------------------------------------------
dnl Work out if functions in dynamically loadable libraries need leading _
dnl Tests based on glib code and only valid for dlopen() mechanism
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_LEADING_USCORE],
[
if test "$ac_cv_header_dlfcn_h" = "yes" -o "$HAVE_DLFCN_H" = "1"; then
   AC_MSG_CHECKING(if symbols need underscore prepended in loadable modules)
   tmpLIBS="$LIBS"
   save_cflags="$CFLAGS"
   LIBS="$LIBS $DLFCNLIBDIR"
   CFLAGS="$CFLAGS $DLFCNINCDIR"
   AC_CACHE_VAL(mh_cv_uscore,[
   AC_TRY_RUN_NATIVE([
   #include <dlfcn.h>
   int mh_underscore_test (void) { return 42; }
   int main() {
     void *f1 = (void*)0, *f2 = (void*)0, *handle;
     handle = dlopen ((void*)0, 0);
     if (handle) {
       f1 = dlsym (handle, "mh_underscore_test");
       f2 = dlsym (handle, "_mh_underscore_test");
     } return (!f2 || f1);
   }],
   mh_cv_uscore=yes,
   mh_cv_uscore=no
   )
   ])
   AC_MSG_RESULT($mh_cv_uscore)
   if test "x$mh_cv_uscore" = "xyes"; then
     AC_DEFINE(MODULES_NEED_USCORE)
   fi
   LIBS="$tmpLIBS"
   CFLAGS="$save_cflags"
fi
])

dnl ---------------------------------------------------------------------------
dnl Determines the file extension for shared libraries
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_SHLPST],
[
AC_MSG_CHECKING(shared library/external function extensions)
SHLPST=".so"
MODPST=".so"
AC_REQUIRE([AC_CANONICAL_SYSTEM])
case "$target" in
        *hp-hpux*)
                SHLPST=".sl"
                MODPST=".sl"
                ;;
        *ibm-aix*)
                SHLPST=".a"
                MODPST=".a"
                ;;
        *qnx*)
                SHLPST=""
                MODPST=""
                ;;
        *cygwin*)
                SHLPST=".dll"
                MODPST=".dll"
                ;;
        *darwin*)
                SHLPST=".dylib"
                MODPST=".dylib"
                ;;
esac
AC_SUBST(SHLPST)
AC_SUBST(MODPST)
AC_MSG_RESULT($SHLPST/$MODPST)
])
