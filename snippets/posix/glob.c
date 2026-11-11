#include <stdio.h>
#include <glob.h>

void
ls(const char *dir)
{
	glob_t gl;
	size_t i;

	if (glob(dir, GLOB_MARK, NULL, &gl) != 0)
		return;

	for (i = 0; i < gl.gl_pathc; i++)
		printf("%s\n", gl.gl_pathv[i]);

	globfree(&gl);
}

int
main(void)
{
	ls("/etc/*");
	ls("./*");
	ls("../*");
	return 0;
}
