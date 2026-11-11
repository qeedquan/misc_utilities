#link against .lib, still need .dll for it to run
#dll can link against static libs too
all:
	cl /c util.c
	lib util.obj
	cl /EHsc /LD dll1.c dll2.c dll3.cpp util.lib /o mydll
	cl /EHsc main.cpp util.lib /link mydll.lib /OUT:test.exe

clean:
	del *.exe *.dll *.obj *.lib *.exp