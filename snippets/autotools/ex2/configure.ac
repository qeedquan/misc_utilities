AC_INIT(program, 1.0)

dnl Switch to a C++ compiler, and check if it works.
dnl Do compilation tests using the compiler, preprocessor, and file extensions for the specified language.
dnl Supported languages: C/C++, Fortran, Erlang, Go, etc
AC_LANG(C++)
AC_PROG_CXX

dnl Specify the compiler to use
dnl Can be used to cross compile this way
dnl If --host is used, what it does is it expands the C compiler command to value-cc so --host avr will make it look for avr-cc/avr-g++ and so on as the compiler command name
dnl ./configure --host avr && make
dnl ./configure CXX=g++-5 && make 

dnl The configure script CXXFLAGS can be passed like so:
dnl ./configure CXXFLAGS="-Wall -Wextra" && make

dnl Process Makefile.in to create Makefile
AC_OUTPUT(Makefile)
