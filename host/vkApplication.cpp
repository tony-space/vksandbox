#include "vkApplication.hpp"

static void fillGlfwRequiredInstanceExtensions(VkInstanceCreateInfo& createInfo)
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
}

static VkInstance makeVulkanInstance()
{
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	fillGlfwRequiredInstanceExtensions(createInfo);

	VkInstance vkInstance;
	VkResult vkResult = vkCreateInstance(&createInfo, nullptr, &vkInstance);
	assert(vkResult == VK_SUCCESS);
	if (vkResult != VK_SUCCESS)
	{
		throw std::runtime_error("vkCreateInstance failed");
	}
	
	return vkInstance;
}

vkApplication::vkApplication()
{
	auto glfwStatus = glfwInit();
	assert(glfwStatus == GLFW_TRUE);
	if (glfwStatus != GLFW_TRUE)
	{
		throw std::runtime_error("glfwInit failed");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	_window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
	_vkInstance = makeVulkanInstance();
}

vkApplication::~vkApplication()
{
	vkDestroyInstance(_vkInstance, nullptr);
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void vkApplication::run()
{
	while (!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();
	}
}
