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
dnl Check for presense of various libraries
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_LIB],
[
MH_EXTRA_LIBS=""
for mh_lib in $1; do
#	AC_MSG_CHECKING(for library -l${mh_lib})
	AC_CHECK_LIB($mh_lib,main,mh_lib_found=yes,mh_lib_found=no)
	if test "$mh_lib_found" = yes; then
#		AC_MSG_RESULT(found)
		MH_EXTRA_LIBS="${MH_EXTRA_LIBS} -l${mh_lib}"
#	else
#		AC_MSG_RESULT(not found)
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
	if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.rxlib; then
		LD_RXLIB1="ld -shared"
		RXLIBPRE="lib"
		RXLIBPST=".so"
	else
		mh_dyn_link='ld -G -o conftest.rxlib conftest.o 1>&AC_FD_CC'
		if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.rxlib; then
			LD_RXLIB1="ld -G"
			RXLIBPRE="lib"
			RXLIBPST=".so"
		else
			LD_RXLIB1=""
			RXLIBPRE=""
			RXLIBPST=""
		fi
	fi
fi
rm -f conftest*
])dnl

dnl ---------------------------------------------------------------------------
dnl Work out how to create a shared library
dnl ---------------------------------------------------------------------------
AC_DEFUN(MH_HOWTO_SHARED_LIBRARY,
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
	if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
		SHL_LD="ld -shared -o ${LIBPRE}${LIBFILE}.${SHL}."'$('VERDOT')'" "'$('SHOFILES')'" -lc"
	else
		mh_dyn_link='ld -G -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
		if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
			SHL_LD="ld -G -o ${LIBPRE}${LIBFILE}.${SHL}."'$('VERDOT')'" "'$('SHOFILES')'
		else
			mh_dyn_link='ld -o conftest.so.1.0 -shared -no_archive conftest.o  -lc 1>&AC_FD_CC'
			if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
				SHL_LD="ld -o ${LIBPRE}${LIBFILE}.${SHL}."'$('VERDOT')'" -shared -no_archive "'$('SHOFILES')'" -lc"
			else
				mh_dyn_link='ld -b -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
				if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
					SHL_LD="ld -b -o ${LIBPRE}${LIBFILE}.${SHL}."'$('VERDOT')'" "'$('SHOFILES')'
				else
					mh_dyn_link='ld -Bshareable -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
					if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
						SHL_LD="ld -Bshareable -o ${LIBPRE}${LIBFILE}.${SHL}."'$('VERDOT')'" "'$('SHOFILES')'
					else
						mh_dyn_link='ld -assert pure-text -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
						if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
							SHL_LD="ld -assert pure-text -o ${LIBPRE}${LIBFILE}.${SHL}."'$('VERDOT')'" "'$('SHOFILES')'
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
dnl Determine how to build shared libraries etc..
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_SHARED_LIBRARY],
[
dnl
dnl If compiler is gcc, then flags should be the same for all platforms
dnl (just guessing on this)
dnl
OSAVE=".o.save"
OBJ="o"
EXE=""
GETOPT=""
STATIC_LDFLAGS=""
AIX_DYN="no"
SHLFILE="$1"

AC_REQUIRE([AC_CANONICAL_SYSTEM])
case "$target" in
	*hp-hpux10*)
		SYS_DEFS="-D_HPUX_SOURCE"
		;;
	*hp-hpux*) 
		SYS_DEFS="-D_HPUX_SOURCE"
		;;
	*ibm-aix4.2*)
		SYS_DEFS="-D_ALL_SOURCE -DAIX"
		AIX_DYN="yes"
		DYN_COMP="-DDYNAMIC"
		;;
	*ibm-aix4.3*)
		SYS_DEFS="-D_ALL_SOURCE -DAIX"
		AIX_DYN="yes"
		DYN_COMP="-DDYNAMIC"
		;;
	*ibm-aix*)
		SYS_DEFS="-D_ALL_SOURCE -DAIX"
		AIX_DYN="yes"
		DYN_COMP="-DDYNAMIC"
		;;
	*dec-osf*)
		SYS_DEFS="-D_POSIX_SOURCE -D_XOPEN_SOURCE -Olimit 800"
		;;
	*sequent-dynix*)
		;;
	*solaris*)
		;;
	*esix*)
		;;
	sparc*sunos*)
		SYS_DEFS="-DSUNOS -DSUNOS_STRTOD_BUG"
		;;
	*linux*)
		SYS_DEFS=""
		;;
esac

dnl
dnl set variables for compiling if we don't already have it
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
	CFLAGS=$save_cflags
	rm -f conftest.*
fi

if test "$with_rexx6000" = yes; then
	mh_entry="-eInitFunc"
else
	mh_entry="-bnoentry"
fi

dnl
dnl set variables for linking
dnl
RXPACKEXPORTS=""
case "$target" in
	*hp-hpux*) 
		EEXTRA="-Wl,-E"
		LD_RXLIB1="ld -b -q -n"
		RXLIBPRE="lib"
		RXLIBPST=".sl"
		;;
#	*ibm-aix4.2*)
#		STATIC_LDFLAGS="-bnso -bI:/lib/syscalls.exp"
#		LD_RXLIB1="ld -G"
#		RXLIBPRE="lib"
#		RXLIBPST=".so"
#		RXPACKEXPORTS="-bE:$SHLFILE.exp"
#		RXPACKEXP="$SHLFILE.exp"
#		;;
	*ibm-aix3*)
		STATIC_LDFLAGS="-bnso -bI:/lib/syscalls.exp"
		LD_RXLIB1="ld $mh_entry -bM:SRE -bT:512 -bH:512 -bI:regina.exp"
		RXLIBPRE="lib"
		RXLIBPST=".so"
		RXPACKEXPORTS="-bE:$SHLFILE.exp"
		RXPACKEXP="$SHLFILE.exp"
		;;
	*ibm-aix4*)
		STATIC_LDFLAGS="-bnso -bI:/lib/syscalls.exp"
		LD_RXLIB1="ld $mh_entry -bM:SRE"
		RXLIBPRE="lib"
		RXLIBPST=".so"
		RXPACKEXPORTS="-bE:$SHLFILE.exp"
		RXPACKEXP="$SHLFILE.exp"
		;;
	*dec-osf*)
		LD_RXLIB1="ld -shared"
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	*esix*)
		LD_RXLIB1="ld -G"
		DYNAMIC_LDFLAGS=""
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	*dgux*)
		LD_RXLIB1="ld -G"
		DYNAMIC_LDFLAGS=""
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	*pc-sco*)
		LD_RXLIB1="ld -dy -G"
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	*solaris*)
		LD_RXLIB1="ld -G"
		DYNAMIC_LDFLAGS=""
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	sparc*sunos*)
		LD_RXLIB1="ld -assert pure-text"
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	*freebsd*)
		LD_RXLIB1="ld -Bdynamic -Bshareable"
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	*linux*)
		LD_RXLIB1="${CC} -shared"
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	*nto-qnx*)
		LD_RXLIB1="${CC} -shared"
		RXLIBPRE="lib"
		RXLIBPST=".so"
		;;
	*qnx*)
		LD_RXLIB1=""
		RXLIBPRE=""
		RXLIBPST=".lib"
		EEXTRA="-mf -N0x20000 -Q"
		;;
	*)
		MH_HOWTO_DYN_LINK()
		;;
esac

if test "$ac_cv_header_dl_h" = "yes" -o "$ac_cv_header_dlfcn_h" = "yes" -o "$AIX_DYN" = "yes"; then
	SHL_TARGETS="${RXLIBPRE}${SHLFILE}${RXLIBPST}"
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
AC_SUBST(RXLIBPRE)
AC_SUBST(RXLIBPST)
AC_SUBST(DYNAMIC_LDFLAGS)
AC_SUBST(STATIC_LDFLAGS)
AC_SUBST(SHL_TARGETS)
AC_SUBST(O2SAVE)
AC_SUBST(O2SHO)
AC_SUBST(CC2O)
AC_SUBST(SAVE2O)
AC_SUBST(RXPACKEXPORTS)
AC_SUBST(RXPACKEXP)
])dnl

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
