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

dnl
dnl include our own common routines
dnl
sinclude(common/accommon.m4)dnl
