#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class vkApplication
{
public:
#ifdef NDEBUG
	static constexpr bool kDebugBuild = false;
#else
	static constexpr bool kDebugBuild = true;
#endif // NDEBUG
	static constexpr int kWidth = 800;
	static constexpr int kHeight = 600;


	vkApplication();
	~vkApplication();
	
	void run();
private:
	GLFWwindow* _window;
	VkInstance _vkInstance;
	VkPhysicalDevice _vkPhysDevice;
	VkDevice _vkDevice;
};
