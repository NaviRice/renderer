#ifndef GLFWMANAGERHEADER
#define GLFWMANAGERHEADER
	int glfw_resizeWindow(int width, int height, int bpp, int debugmode);
	int glfw_resizeWindow2(int width, int height, int bpp, int debugmode);
	int glfw_init(int width, int height, int bpp, int debugmode);
	void glfw_swapBuffers(void);
	void glfw_swapBuffers2(void);
	void glfw_context1(void);
	void glfw_context2(void);
	int glfw_checkEvent(void);
	const char ** glfw_extensions;
	unsigned int glfw_extensions_count;


	int glfw_shutdown(void);

#endif
