dnl ---------------------------------------------------------------------------
dnl This file offers the following macros...
dnl ---------------------------------------------------------------------------
dnl MH_IPC
dnl MH_CHECK_X_INC
dnl MH_CHECK_X_LIB
dnl MH_CHECK_X_HEADERS
dnl MH_CHECK_X_KEYDEFS
dnl MH_CHECK_X_TYPEDEF
dnl MH_CHECK_LIB
dnl MH_SHARED_LIBRARY
dnl MH_HOWTO_DYN_LINK
dnl MH_CHECK_CC_O
dnl MH_SHLPST
dnl MH_CHECK_MAX_SIGNALS
dnl MH_CHECK_OSX_ARCH
dnl MH_GET_DISTRO_NAME
dnl MH_CHECK_RPM
dnl MH_CHECK_ARCH
dnl AC_DEFINE_DIR

dnl ---------------------------------------------------------------------------
dnl Determine if the system has System V IPC. ie sys/ipc.h and sys/shm.h
dnl headers.
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_IPC],
[
AC_CHECK_HEADER(sys/ipc.h)
if test $ac_cv_header_sys_ipc_h = no; then
    AC_MSG_ERROR(Cannot find required header file sys/ipc.h; PDCurses cannot be configured)
fi
])dnl

dnl ---------------------------------------------------------------------------
dnl Set up the correct X header file location
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_INC],
[
mh_x11_dir=""
mh_x11_xaw_dir=""

mh_inc_dirs="$ac_x_header_dirs"

dnl Provide for user supplying directory

if test "x$x_includes" != xNONE ; then
    mh_inc_dirs="$x_includes $mh_inc_dirs"
fi

dnl Try to determine the directory containing X headers
dnl We will append X11 to all the paths above as an extra check

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

dnl Try to determine the directory containing Xaw headers
dnl We will append X11 to all the paths above as an extra check

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

    if test "$mh_x11_dir_no_x11" != "$mh_x11_dir" ; then
        MH_XINC_DIR="-I$mh_x11_dir -I$mh_x11_dir_no_x11"
    else
        MH_XINC_DIR="-I$mh_x11_dir"
    fi
else
    AC_MSG_ERROR(Cannot find required header file Intrinsic.h; PDCurses cannot be configured)
fi

if test "x$mh_x11_xaw_dir" != "x"; then
    if test "$mh_x11_xaw_dir" != "$mh_x11_dir" ; then
        MH_XINC_DIR="-I$mh_x11_xaw_dir $MH_XINC_DIR"
    fi
else
    AC_MSG_ERROR(Cannot find required Xaw header file Box.h; PDCurses cannot be configured)
fi

AC_SUBST(MH_XINC_DIR)

])dnl

dnl ---------------------------------------------------------------------------
dnl Set up the correct X library file location
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_LIB],
[
dnl Some systems require extra libraries...

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

which dpkg-architecture > /dev/null
if test $? -eq 0; then
   multiarch_libdir="/usr/lib/`dpkg-architecture -qDEB_HOST_MULTIARCH`"
else
   multiarch_libdir=""
fi

mh_lib_dirs="$multiarch_libdir /usr/lib64 /usr/lib/x86_64-linux-gnu /usr/lib/i386-linux-gnu $x_libraries `echo "$ac_x_includes $ac_x_header_dirs" | sed s/include/lib/g`"

dnl try to find libSM.[a,sl,so,dylib]. If we find it we are using X11R6
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


dnl Ensure that all required X libraries are found

mh_prev_dir=""
mh_where_found=""
mh_where_found_dirs=""
mh_solaris_path=""
for mh_lib in $mh_libs; do
  mh_lib_found=no
  for ac_dir in $mh_lib_dirs ; do
    for mh_ext in a so sl dll.a dylib; do
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
    AC_MSG_ERROR(Cannot find required X library; lib$mh_lib. PDCurses cannot be configured)
  fi
done

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
      AC_MSG_ERROR(X11 installation incomplete; cannot continue)
    fi
  fi
fi

AC_SUBST(MH_XLIBS)

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
dnl Check for presence of various libraries
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
#   mh_dyn_link='${CC} -Wl,-shared -o conftest.rxlib conftest.o -lc 1>&AC_FD_CC'
    if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.rxlib; then
        LD_RXLIB1="ld -shared"
#       LD_RXLIB1="${CC} -Wl,-shared"
        SHLPRE="lib"
        SHLPST=".so"
    else
        mh_dyn_link='ld -G -o conftest.rxlib conftest.o 1>&AC_FD_CC'
#       mh_dyn_link='${CC} -Wl,-G -o conftest.rxlib conftest.o 1>&AC_FD_CC'
        if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.rxlib; then
            LD_RXLIB1="ld -G"
#           LD_RXLIB1="${CC} -Wl,-G"
            SHLPRE="lib"
            SHLPST=".so"
        else
            LD_RXLIB1=""
            SHLPRE=""
            SHLPST=""
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
AIX_DYN="no"
BEOS_DYN="no"
SHLFILE="$1"
SHLFILES="$*"
RXPACKEXPORTS=""
SHLPRE="lib"
LD_RXLIB1=""
BASE_INSTALL="installbase"
BASE_BINARY="binarybase"
# OTHER_INSTALLS is set to install a non-version numbered shared library
# by default; ie. no .\$(ABI) suffix. Set OTHER_INSTALLS="installabilib" if you
# are building a version numbered shared library.
OTHER_INSTALLS="installlib"

AC_REQUIRE([AC_CANONICAL_SYSTEM])
case "$target" in
    *hp-hpux*)
        SYS_DEFS="-D_HPUX_SOURCE"
        LD_RXLIB1="ld -b -q -n"
        ;;
    *ibm-aix*)
        SYS_DEFS="-D_ALL_SOURCE -DAIX"
        AIX_DYN="yes"
        DYN_COMP="-DDYNAMIC"
        LD_RXLIB1="ld -bnoentry -bM:SRE"
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
    *sequent-dynix*|*esix*|*dgux*)
        LD_RXLIB1="ld -G"
        ;;
    *solaris*)
        if test "$ac_cv_prog_CC" = "gcc"; then
            LD_RXLIB1="gcc -shared"
        else
            LD_RXLIB1="ld -G"
        fi
        ;;
    sparc*sunos*)
        SYS_DEFS="-DSUNOS -DSUNOS_STRTOD_BUG"
        LD_RXLIB1="ld"
        ;;
    *atheos*|*nto-qnx*)
        LD_RXLIB1="${CC} -shared"
        ;;
    *linux*|*kfreebsd*-gnu*)
      LD_RXLIB1="${CC} -shared  -Wl,-soname,\$(@)"
      CAN_USE_ABI="yes"
      if test "$USE_ABI" = "yes"; then
         OTHER_INSTALLS="installabilib"
      fi
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
        ;;
    *qnx*)
        SHLPRE=""
        DYN_COMP="-Q"   # force no check for dynamic loading
        SHLFILE=""
        ;;
    *cygwin)
        SHLPRE=""
        DYN_COMP="-DDYNAMIC"
        LD_RXLIB1="dllwrap --def \$(srcdir)/../win32/pdcurses.def --target i386-cygwin32 --dllname \$(@)"
# cygwininstall target MUST install the shared library itself because
# it puts it into $(bindir) not $(libdir) as all other platforms
        ;;
    *darwin*)
        DYN_COMP="-fno-common"
        LD_RXLIB1="${CC} -flat_namespace -undefined suppress -dynamiclib -install_name=\$(@)"
        ;;
    *)
        ;;
esac

dnl determine what switches our compiler uses for building objects
dnl suitable for inclusion in shared libraries
dnl Only call this if DYN_COMP is not set. If we have set DYN_COMP
dnl above, then we know how to compile AND link for dynamic libraries

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


SHL_TARGETS=""

if test "$AIX_DYN" = "yes"; then
    aix_exports="config.exports.aix"
    echo "" > $aix_exports
fi

if test "$ac_cv_header_dl_h" = "yes" -o "$ac_cv_header_dlfcn_h" = "yes" -o "$AIX_DYN" = "yes" -o "$BEOS_DYN" = "yes" -o "$DLFCNINCDIR" != "" -o "$DLFCNLIBDIR" != ""; then
    EXPS="1,2,3,4,5,6,7,8,9"
    for a in $SHLFILES
    do
        if test "$USE_ABI" = "yes" -a "$CAN_USE_ABI" = "yes"; then
           SHL_TARGETS="${SHL_TARGETS} ${SHLPRE}${a}${SHLPST}.\$(ABI)"
        else
           SHL_TARGETS="${SHL_TARGETS} ${SHLPRE}${a}${SHLPST}"
        fi
        this=`echo $EXPS | cut -d, -f1`
        EXPS=`echo $EXPS | cut -d, -f2-`
        if test "$AIX_DYN" = "yes"; then
            echo "RXPACKEXP$this=$a.exp" >> $aix_exports
            echo "RXPACKEXPORTS$this=-bE:$a.exp" >> $aix_exports
        fi
    done
fi

AC_SUBST(DYN_COMP)
AC_SUBST(LIBS)
AC_SUBST(LD_RXLIB1)
AC_SUBST(SHLPRE)
AC_SUBST(SHLPST)
AC_SUBST(SHL_TARGETS)
AC_SUBST(O2SAVE)
AC_SUBST(CC2O)
AC_SUBST(SAVE2O)
AC_SUBST(RXPACKEXPORTS)
AC_SUBST(BASE_INSTALL)
AC_SUBST(OTHER_INSTALLS)
AC_SUBST(BASE_BINARY)
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
    O2SAVE=""
    SAVE2O=""
    CC2O="-o $"'@'
    AC_MSG_RESULT(yes)
else
    O2SAVE="-mv \`basename "$'@'" .sho\`.o \`basename "$'@'" .sho\`.o.save;"
    SAVE2O="-mv \`basename "$'@'" .sho\`.o "$'@'"; mv \`basename "$'@'" .sho\`.o.save \`basename "$'@'" .sho\`.o"
    CC2O=""
    AC_MSG_RESULT(no)
fi
])

dnl ---------------------------------------------------------------------------
dnl Determines the file extension for shared libraries
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_SHLPST],
[
AC_MSG_CHECKING(shared library extension)
SHLPST=".so"
AC_REQUIRE([AC_CANONICAL_SYSTEM])
case "$target" in
        *hp-hpux*)
                SHLPST=".sl"
                ;;
        *ibm-aix*)
                SHLPST=".a"
                ;;
        *qnx*)
                SHLPST=""
                ;;
        *cygwin*)
                SHLPST=".dll"
                ;;
        *darwin*)
                SHLPST=".dylib"
                ;;
esac
AC_SUBST(SHLPST)
AC_MSG_RESULT($SHLPST)
])

dnl ---------------------------------------------------------------------------
dnl Determine the system limit for number of signals
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_MAX_SIGNALS],
[
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $SYS_DEFS"
AC_MSG_CHECKING(for maximum signal specifier:)
AC_CACHE_VAL(mh_cv_max_signals,
mh_found="no"
for mh_sigs in $1; do
    AC_TRY_COMPILE([#include <signal.h>],
[return $mh_sigs;],
  mh_found="yes"; mh_cv_max_signals="$mh_sigs" )
    if test "$mh_found" = "yes"; then
        break;
    fi
done)
CPPFLAGS="$save_CPPFLAGS"
if test "$mh_found" = "no"; then
    AC_MSG_ERROR(Cannot find a system limit for number of signals. PDCurses cannot be configured on this machine.)
else
    AC_DEFINE_UNQUOTED(PDC_MAX_SIGNALS,$mh_cv_max_signals)
    AC_MSG_RESULT($mh_cv_max_signals)
fi
])dnl

dnl ---------------------------------------------------------------------------
dnl Determines which -arch flags valid on Mac OSX with gcc
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_OSX_ARCH],
[
valid_arch_flags=""
found_arch_flags=""
AC_ARG_WITH(arch,
   [  --with-arch=option      build universal binaries on OS X, option: one of "all 32bit 64bit intel ppc ppc64 x86_64 i386"],
   [with_arch=$withval],
   [with_arch=none],
)
case "$with_arch" in
   none)
      arch_flags=""
      ;;
   all)
      arch_flags="-arch ppc -arch ppc64 -arch x86_64 -arch i386"
      ;;
   32bit)
      arch_flags="-arch ppc -arch i386"
      ;;
   64bit)
      arch_flags="-arch ppc64 -arch x86_64"
      ;;
   intel)
      arch_flags="-arch i386 -arch x86_64"
      ;;
   *)
      arch_flags="-arch $with_arch"
      ;;
esac

valid_arch_flags=""
found_arch_flags=""
if test $on_osx = "yes"; then
  AC_MSG_CHECKING(for which Mac OSX -arch flags are supported for universal binaries)
  for a in $arch_flags; do
    save_ldflags="$LDFLAGS"
    LDFLAGS="$LDFLAGS -arch $a"
    AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <stdio.h>],
    [exit(0)])],
    [valid_arch_flags="$valid_arch_flags -arch $a";found_arch_flags="$found_arch_flags $a"],
    [a="$a"]
    )
    LDFLAGS="$save_ldflags"
  done
  AC_MSG_RESULT($found_arch_flags)
  AC_SUBST(valid_arch_flags)
fi
])

dnl ---------------------------------------------------------------------------
dnl Determines the Linux distribution name
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_GET_DISTRO_NAME],
[
AC_CHECK_PROG(lsb_release, [lsb_release], yes, no)
if test "$ac_cv_prog_lsb_release" = yes; then
   AC_MSG_CHECKING(OS distribution name)
   MYDISTRO="`lsb_release -i | cut -f 2`-`lsb_release -r | cut -f 2`"
   MYDISTRO="`echo $MYDISTRO | sed \"s/ /_/g\"`"
   AC_MSG_RESULT($MYDISTRO)
else
   case "$target" in
      *freebsd* | *openbsd*)
         MYDISTRO="`echo $target | cut -f3 -d-`"
      ;;
      *darwin*)
         MYDISTRO="`echo $target | cut -f2-3 -d-`"
      ;;
      *pc-solaris2*)
         MYDISTRO="`echo $target | cut -f2- -d-`"
      ;;
      *cygwin*)
         MYDISTRO="`echo $target | cut -f2- -d-`"
      ;;
      *)
         MYDISTRO="$target"
      ;;
   esac
fi
AC_SUBST(MYDISTRO)
])

dnl ---------------------------------------------------------------------------
dnl Determines where rpm build files are located
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_RPM],
[
AC_CHECK_PROG(mh_rpm_exists, rpm, yes, no )
if test "$mh_rpm_exists" = yes; then
   AC_MSG_CHECKING(where rpms are built)
   RPMTOPDIR=`rpm --eval "%{_topdir}"`
   AC_MSG_RESULT($RPMTOPDIR)
   AC_SUBST(RPMTOPDIR)
fi
])

dnl
dnl Determine if the platform can handle 64 bit binaries
dnl
dnl Assumes AC_CANONICAL_SYSTEM has already been called
dnl assumes we are on a 32bit system

AC_DEFUN([MH_CHECK_ARCH],
[
dnl ---------------------- check 32/64 bit -----------------------
dnl allow --with-32bit to ensure that 32bit libraries are used
AC_ARG_ENABLE(32bit,
   [  --enable-32bit          specify if 32bit libraries are to be used],
   [bitflag32=yes],
   [bitflag32=no],
)
dnl allow --with-64bit to ensure that 64bit libraries are used
AC_ARG_ENABLE(64bit,
   [  --enable-64bit          specify if 64bit libraries are to be used],
   [bitflag64=yes],
   [bitflag64=no],
)

#
# Now do platform specific tests
#
gcc_64bit="-m64"
gcc_32bit="-m32"
on_osx="no"
osis64bit=no
bitflag=""
case "$target" in
   *hp-hpux*)
      ;;
   *ibm-aix*)
      rc=`lsconf -k | grep -c 64-bit`
      if test $rc -eq 1; then
         bitflag="64"
         osis64bit=yes
      fi
      #
      # AIX gcc has its own special bit switches :-(
      #
      gcc_64bit="-maix64"
      gcc_32bit="-maix32"
      ;;
   *dec-osf4*)
      ;;
   *dec-osf*)
      ;;
   *sequent-dynix*)
      ;;
   i*86*solaris*)
      ;;
   *solaris*)
      rc=`isainfo -v | grep -c 64-bit`
      if test $rc -eq 1; then
         bitflag="64"
         osis64bit=yes
      fi
      ;;
   sparc*sunos*)
      ;;
   *linux*|*kfreebsd*-gnu*)
      mach="`uname -m`"
      if test "$mach" = "x86_64" -o "$mach" = "ia86" -o "$mach" = "alpha" -o "$mach" = "ppc64" -o "$mach" = "s390x"; then
         bitflag="64"
         osis64bit=yes
      fi
      ;;
   *-freebsd*)
      mach="`uname -m`"
      if test "$mach" = "amd64"; then
         bitflag="64"
         osis64bit=yes
      fi
      ;;
   *nto-qnx*)
      ;;
   *qnx*)
      ;;
   *beos*)
      ;;
   *cygwin*)
      ;;
   *apple-darwin*)
      on_osx="yes"
      osx_64bit=`sysctl hw.cpu64bit_capable | cut -f2 -d' '`
      if test $osx_64bit -eq 1; then
         bitflag="64"
         osis64bit=yes
      fi
      ;;
esac

if test "x$bitflag32" = "xyes" -a "x$bitflag64" = "xyes"; then
   AC_MSG_ERROR(--enable-32bit and --enable-64bit flags cannot both be specified.)
fi

if test "x$bitflag64" = "xyes" -a "$osis64bit" = "no"; then
   AC_MSG_ERROR(--enable-64bit cannot be specified on non-64bit OS.)
fi

binarybitprefix=""
if test "x$bitflag32" = "xyes"; then
   bitflag="32"
   if test "$osis64bit" = "yes"; then
      binarybitprefix="32"
   fi
elif test "x$bitflag64" = "xyes"; then
   bitflag="64"
fi
MACH_ARCH=`$ac_config_guess | cut -f1 -d-`
AC_SUBST(MACH_ARCH)
dnl
dnl following variable used to name 32bit binaries on a 64bit system
dnl allows 32bit and 64bit binaries t co-exist on a 64bit system
AC_SUBST(binarybitprefix)

dnl --------------- allow --with-arch to specify which architectures to build universal binaries
dnl
if test "$on_osx" = "yes"; then
   MH_CHECK_OSX_ARCH()
else
dnl
dnl Now set the correct compiler flags
dnl
   if test "$ac_cv_prog_CC" = "gcc" -o "$ac_cv_prog_CC" = "g++"; then
      if test "$bitflag" = "32"; then
         CFLAGS="${CFLAGS} $gcc_32bit"
         LDFLAGS="${LDFLAGS} $gcc_32bit"
      elif test "$bitflag" = "64"; then
         CFLAGS="${CFLAGS} $gcc_64bit"
         LDFLAGS="${LDFLAGS} $gcc_64bit"
      else
         bitflag="32"
      fi
   elif test "$ac_cv_prog_CC" = "xlc" -o "$ac_cv_prog_CC" = "xlC"; then
      if test "$bitflag" = "32"; then
         CFLAGS="${CFLAGS} -q32"
         LDFLAGS="${LDFLAGS} -q32"
      elif test "$bitflag" = "64"; then
         CFLAGS="${CFLAGS} -q64"
         LDFLAGS="${LDFLAGS} -q64"
      else
         bitflag="32"
      fi
   fi
fi

dnl allow --disable-libdir-change to ensure that building a .deb works
#libdir_change=yes
AC_ARG_ENABLE(libdir-change,
   [  --enable-libdir-change  specify --disable-libdir-change if building a .deb],
   [libdir_change=$enableval],
   [libdir_change=yes],
)
echo "libdir: $libdir_change"
if test "$libdir_change" = yes; then
   dnl
   dnl If the user hasn't specified libdir, check for specific lib dirs
   dnl
   if test "${libdir}" = '${exec_prefix}/lib'; then
   dnl
   dnl expand the active $prefix
   dnl
      myprefix=$prefix
      AC_DEFINE_DIR(MY_PREFIX,"$myprefix")
      if test "${myprefix}" = "NONE"; then
         myprefix="$ac_default_prefix"
      fi
   dnl
   dnl check for 64bit libdir
   dnl
      if test -d "${myprefix}/lib/x86_64-linux-gnu"; then
         libdir64="${myprefix}/lib/x86_64-linux-gnu"
      elif test -d "${myprefix}/lib64"; then
         libdir64="${myprefix}/lib64"
      else
         libdir64="${myprefix}/lib"
      fi
   dnl
   dnl check for 32bit libdir
   dnl
      if test -d "${myprefix}/lib/i386-linux-gnu"; then
         libdir32="${myprefix}/lib/i386-linux-gnu"
      elif test -d "${myprefix}/lib32"; then
         libdir32="${myprefix}/lib32"
      else
         libdir32="${myprefix}/lib"
      fi
   dnl
   dnl Depending on the build type, set the default libdir
   dnl
      if test "$bitflag" = "64"; then
         libdir="$libdir64"
      else
         libdir="$libdir32"
      fi
   else
      libdir64="$libdir"
      libdir32="$libdir"
   fi
else
   libdir64="$libdir"
   libdir32="$libdir"
fi
AC_SUBST(libdir64)
AC_SUBST(libdir32)
AC_SUBST(bitflag)
])
dnl @synopsis AC_DEFINE_DIR(VARNAME, DIR [, DESCRIPTION])
dnl
dnl This macro sets VARNAME to the expansion of the DIR variable,
dnl taking care of fixing up ${prefix} and such.
dnl
dnl VARNAME is then offered as both an output variable and a C
dnl preprocessor symbol.
dnl
dnl Modified by Mark Hessling to fix eval bug
dnl
dnl Example:
dnl
dnl    AC_DEFINE_DIR([DATADIR], [datadir], [Where data are placed to.])
dnl
dnl @category Misc
dnl @author Stepan Kasal <kasal@ucw.cz>
dnl @author Andreas Schwab <schwab@suse.de>
dnl @author Guido U. Draheim <guidod@gmx.de>
dnl @author Alexandre Oliva
dnl @version 2006-10-13
dnl @license AllPermissive

AC_DEFUN([AC_DEFINE_DIR], [
  prefix_NONE=
  exec_prefix_NONE=
  test "x$prefix" = xNONE && prefix_NONE=yes && prefix=$ac_default_prefix
  test "x$exec_prefix" = xNONE && exec_prefix_NONE=yes && exec_prefix=$prefix
dnl In Autoconf 2.60, ${datadir} refers to ${datarootdir}, which in turn
dnl refers to ${prefix}.  Thus we have to use `eval' twice.
  eval ac_define_dir="\"$2\""
  eval ac_define_dir="\"$ac_define_dir\""
  AC_SUBST($1, "$ac_define_dir")
  AC_DEFINE_UNQUOTED($1, "$ac_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])

