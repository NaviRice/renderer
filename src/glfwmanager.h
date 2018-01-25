#ifndef GLFWMANAGERHEADER
#define GLFWMANAGERHEADER
	int glfw_resizeWindow(int c, int width, int height, int bpp, int debugmode);
	int glfw_resizeWindow_current(int width, int height, int bpp, int debugmode);
	int glfw_init(int width, int height, int bpp, int debugmode);
	void glfw_swapBuffers(void);
	int glfw_checkEvent(void);
	const char ** glfw_extensions;
	unsigned int glfw_extensions_count;


	int glfw_shutdown(void);

#endif
