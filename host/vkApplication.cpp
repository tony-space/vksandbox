#include "vkApplication.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

static const char* kValidationLayers[] { "VK_LAYER_KHRONOS_validation" };

static void debugPrintExtensions()
{
	try
	{
		VkResult vkResult;

		uint32_t extensionCount = 0;
		vkResult = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		assert(vkResult == VK_SUCCESS);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkResult = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		assert(vkResult == VK_SUCCESS);

		for (const VkExtensionProperties& prop : extensions)
		{
			std::cout << prop.extensionName << " : " << prop.specVersion << '\n';
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
	}
}

static bool checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	return std::any_of(availableLayers.begin(), availableLayers.end(), [](const VkLayerProperties& layer)
	{
		return layer.layerName == std::string_view{ kValidationLayers[0]};
	});
}

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
	if constexpr (vkApplication::kDebugBuild)
	{
		if (checkValidationLayerSupport())
		{
			createInfo.enabledLayerCount = 1;
			createInfo.ppEnabledLayerNames = kValidationLayers;
		}
	}

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
	debugPrintExtensions();

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
