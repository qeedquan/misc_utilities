program print
    !implicit none

    ! declare a variable range (i, j, k, ..., z) to be integer type
    ! disallowed if implicit none is used
    implicit integer(i-z)

    integer :: my_int
    real :: my_real
    character :: my_char

    ! old style fixed length size buffer
    character*20 :: my_char_20

    ! format style, %5d %8.2f %3c
    character(len=20) :: fmt_style = '(I5, F8.2, A3)'

    my_int = 1234
    my_real = 43.23
    my_char = 'X'

    ! No format string
    print *, 'Hello, World!'
    ! Print newline
    print *
    
    ! 1. Inline format string
    print '(I5, F8.2, A3)', my_int, my_real, my_char

    ! 2. Reference a labeled FORMAT statement
    print 100, my_int, my_real, my_char
    100 format (I5, F8.2, A3)

    ! 3. Use format stored in a character variable
    print fmt_style, my_int, my_real, my_char
end program
