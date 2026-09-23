AC_INIT([program],[1.0])

dnl Switch to a C++ compiler, and check if it works.
AC_LANG(C++)
AC_PROG_CXX

dnl Most of the tests in autoconf are to do with compiled code.
dnl To do these, autoconf emits small programs to a file, then runs that file through the compiler and possibly linker,
dnl and checks the resulting error codes.
dnl For example:
dnl AC_CHECK_LIB(m, cos)
dnl Will generate a program looking something like this:
dnl char cos (); 
dnl int main ()
dnl {   
dnl     cos ();
dnl 	return 0;
dnl }
dnl Then attempt to compile and link it with libm.
dnl Autoconf does similar things for AC_CHECK_HEADERS, but only runs the compiler, not the linker too.
dnl If you want to see more details, then create a test which you know will fail, run the script and and look in config.log
AC_CHECK_LIB(m, cos)
AC_CHECK_HEADERS(zlib.h, [], [AC_MSG_ERROR(A working zlib is required)])
AC_SEARCH_LIBS(deflate, z, [], [AC_MSG_ERROR(A working zlib is required)])

dnl Process Makefile.in to create Makefile
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
