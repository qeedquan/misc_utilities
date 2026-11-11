@echo off

rem Tests call
 
call :main
goto :eof

:main
	echo main
	echo.
	for %%i in (1, 2, 3, 4) do (echo Value && echo %%i)
