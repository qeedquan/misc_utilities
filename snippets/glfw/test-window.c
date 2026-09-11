#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

void
error_callback(int error, const char *description)
{
	printf("Error (%d): %s\n", error, description);
}

void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	(void)scancode;
	(void)mods;
}

GLFWwindow *
init_glfw(void)
{
	GLFWwindow *window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

	window = glfwCreateWindow(1024, 768, "GLFW Test", NULL, NULL);
	if (!window)
		exit(1);

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	return window;
}

int
main(void)
{
	GLFWwindow *window;
	int wx, wy, ww, wh;
	double mx, my;

	window = init_glfw();
	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();

		glfwGetWindowPos(window, &wx, &wy);
		glfwGetWindowSize(window, &ww, &wh);
		glfwGetCursorPos(window, &mx, &my);
		printf("window pos:  %d %d\n", wx, wy);
		printf("window size: %d %d\n", ww, wh);
		printf("cursor pos:  %f %f\n", mx, my);
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
