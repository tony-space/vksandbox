#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class vkApplication
{
public:
	vkApplication();
	~vkApplication();
	
	void run();
private:
	GLFWwindow* _window;
	VkInstance _vkInstance;
};
