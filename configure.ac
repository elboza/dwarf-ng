AC_INIT([dwarf],[0.1])
dwarf_version=`cat VERSION`
VERSION=$dwarf_version
AM_INIT_AUTOMAKE(dwarf,$dwarf_version)
AM_CONFIG_HEADER(config.h)
AC_PROG_CC
AC_LANG_C
AH_TEMPLATE([HAVE_MREMAP],[checks if have mremap function])
AC_PROG_MAKE_SET
AC_HEADER_STDC
AC_PROG_LEX
if test "$LEX" != flex; then
AC_MSG_ERROR(flex is required)
fi
AC_PROG_YACC
if test "$YACC" = ""; then 
AC_MSG_ERROR(bison is required)
fi
dnl AC_CHECK_FUNC(mremap,AC_DEFINE([HAVE_MREMAP]))
AC_CHECK_FUNC(mmap,,AC_MSG_ERROR(mmap is required))
AC_CHECK_FUNC(munmap,,AC_MSG_ERROR(munmap is required))
AC_CHECK_FUNC(malloc,,AC_MSG_ERROR(malloc is required))
AC_CHECK_FUNC(open,,AC_MSG_ERROR(open is required))
AC_CHECK_FUNC(lseek,,AC_MSG_ERROR(lseek is required))
AC_CHECK_FUNC(fopen,,AC_MSG_ERROR(fopen is required))
AC_CHECK_FUNC(fclose,,AC_MSG_ERROR(fclose is required))
AC_CHECK_FUNC(getopt,,AC_MSG_ERROR(getopt is required))
AC_CHECK_FUNC(getopt_long,,AC_MSG_ERROR(getopt_long is required))
AC_CHECK_FUNC(ftruncate,,AC_MSG_ERROR(ftruncate is required))
AC_CHECK_FUNC(strlen,,AC_MSG_ERROR(strlen is required))
AC_CHECK_FUNC(strdup,,AC_MSG_ERROR(strdup is required))
AC_CHECK_FUNC(free,,AC_MSG_ERROR(free is required))
AC_CHECK_FUNC(strcmp,,AC_MSG_ERROR(strcmp is required))
AC_CHECK_FUNC(isalnum,,AC_MSG_ERROR(isalnum is required))
AC_CHECK_FUNC(isdigit,,AC_MSG_ERROR(isdigit is required))
AC_CHECK_FUNCS(mremap)
AC_CHECK_LIB(readline,main,,AC_MSG_ERROR(readline ilbrary required))
AC_OUTPUT(Makefile src/Makefile doc/Makefile)

