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
		std::cout << '\n';
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
	}
}

static GLFWwindow* makeWindow()
{
	auto glfwStatus = glfwInit();
	assert(glfwStatus == GLFW_TRUE);
	if (glfwStatus != GLFW_TRUE)
	{
		throw std::runtime_error("glfwInit failed");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return glfwCreateWindow(vkApplication::kWidth, vkApplication::kHeight, "Vulkan window", nullptr, nullptr);
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
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
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

static VkPhysicalDevice findPhysicalDevice(VkInstance vkInstance)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

	std::cout << "Topology\n";
	for (const VkPhysicalDevice& device : devices)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		std::cout << "Device name: \t" << deviceProperties.deviceName << '\n';

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		std::cout << "\tSupported queue families\n";

		for (uint32_t i = 0; i != queueFamilyCount; ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
			std::cout << "\t\tFamily #" << i << '\n';
			std::cout << "\t\t\tFlags\n";
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) std::cout << "\t\t\t\tVK_QUEUE_GRAPHICS_BIT\n";
			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) std::cout << "\t\t\t\tVK_QUEUE_COMPUTE_BIT\n";
			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) std::cout << "\t\t\t\tVK_QUEUE_TRANSFER_BIT\n";
			if (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) std::cout << "\t\t\t\tVK_QUEUE_SPARSE_BINDING_BIT\n";
			if (queueFamily.queueFlags & VK_QUEUE_PROTECTED_BIT) std::cout << "\t\t\t\tVK_QUEUE_PROTECTED_BIT\n";
			if (queueFamily.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) std::cout << "\t\t\t\tVK_QUEUE_VIDEO_DECODE_BIT_KHR\n";
			if (queueFamily.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) std::cout << "\t\t\t\tVK_QUEUE_VIDEO_ENCODE_BIT_KHR\n";
			if (queueFamily.queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) std::cout << "\t\t\t\tVK_QUEUE_OPTICAL_FLOW_BIT_NV\n";
			if (queueFamily.queueFlags & VK_QUEUE_DATA_GRAPH_BIT_ARM) std::cout << "\t\t\t\tVK_QUEUE_DATA_GRAPH_BIT_ARM\n";

			std::cout << "\t\t\tQueue count: " << queueFamily.queueCount << '\n';

			std::cout << '\n';
		}
	}

	return devices.front();
}

static VkDevice createLogicalDevice(VkPhysicalDevice vkPhysDevice)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysDevice, &queueFamilyCount, queueFamilies.data());

	auto it = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& prop)
	{
		return (prop.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) != 0;
	});
	assert(it != queueFamilies.end());
	if (it == queueFamilies.end())
	{
		throw std::runtime_error("finding VK_QUEUE_GRAPHICS queue family failed");
	}

	static const float kQueuePriorities[] = { 1.0f };
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = static_cast<uint32_t>(std::distance(queueFamilies.begin(), it));
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = kQueuePriorities;


	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(vkPhysDevice, &deviceFeatures);

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.pEnabledFeatures = &deviceFeatures;

	VkDevice vkDevice;
	VkResult vkResult = vkCreateDevice(vkPhysDevice, &createInfo, nullptr, &vkDevice);
	assert(vkResult == VK_SUCCESS);
	if (vkResult != VK_SUCCESS)
	{
		throw std::runtime_error("vkCreateDevice failed");
	}

	return vkDevice;
}

vkApplication::vkApplication()
{
	debugPrintExtensions();

	_window = makeWindow();
	_vkInstance = makeVulkanInstance();
	_vkPhysDevice = findPhysicalDevice(_vkInstance);
	_vkDevice = createLogicalDevice(_vkPhysDevice);
}

vkApplication::~vkApplication()
{
	vkDestroyDevice(_vkDevice, nullptr);
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
