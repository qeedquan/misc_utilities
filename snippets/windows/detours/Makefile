INC="Detours\include"
LNK="Detours\lib.X86\detours.lib"

all:
	cl test.c /I $(INC) /link /libpath $(LNK) /OUT:test.exe
	test.exe

clean:
	del *.exe *.obj *.dll