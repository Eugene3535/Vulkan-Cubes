#include <vector>
#include <unordered_set>
#include <string>

#include <vulkan/vulkan.h>

#include "vulkan_api/utils/Constants.hpp"
#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/logical_device/LogicalDevice.hpp"


LogicalDevice::LogicalDevice() noexcept:
    handle(nullptr),
    queue(nullptr),
    mainQueueFamilyIndex(0)
{

}


LogicalDevice::~LogicalDevice() = default;


bool LogicalDevice::create(VkPhysicalDevice physicalDevice) noexcept
{
    if(handle)
        return true;

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

    VkPhysicalDeviceFeatures enabledFeatures = {};

    if (supportedFeatures.samplerAnisotropy)
        enabledFeatures.samplerAnisotropy = VK_TRUE;

    if (supportedFeatures.fillModeNonSolid)
        enabledFeatures.fillModeNonSolid = VK_TRUE;
    
	if(mainQueueFamilyIndex = vk::get_main_queue_family_index(static_cast<void*>(physicalDevice)); mainQueueFamilyIndex != UINT32_MAX)
    {
        const float queuePriority = 1.0f;

    	VkDeviceQueueCreateInfo queueCreateInfo = {};
    	queueCreateInfo.sType			 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    	queueCreateInfo.queueFamilyIndex = mainQueueFamilyIndex;
    	queueCreateInfo.queueCount		 = 1;
    	queueCreateInfo.pQueuePriorities = &queuePriority;

        const std::array<const char*, 2> requiredExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::unordered_set<std::string> deviceExtensions;

        for (const auto& it : availableExtensions)
            deviceExtensions.insert(it.extensionName);

        for (const auto& extension : requiredExtensions)
            if(deviceExtensions.find(extension) == deviceExtensions.end())
                return false;

        constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature = 
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
            .dynamicRendering = VK_TRUE
        };

        VkDeviceCreateInfo deviceCreateInfo = {};
    	deviceCreateInfo.sType				     = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext                   = &dynamic_rendering_feature;
    	deviceCreateInfo.queueCreateInfoCount	 = 1;
    	deviceCreateInfo.pQueueCreateInfos	     = &queueCreateInfo;
    	deviceCreateInfo.pEnabledFeatures		 = &enabledFeatures;
    	deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size());
    	deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
#ifdef DEBUG
    	deviceCreateInfo.enabledLayerCount       = static_cast<uint32_t>(VALIDATION_LAYERS.size());
    	deviceCreateInfo.ppEnabledLayerNames     = VALIDATION_LAYERS.data();
#endif

        if(auto result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &handle); result == VK_SUCCESS)
        {
            vkGetDeviceQueue(handle, mainQueueFamilyIndex, 0, &queue);

            return true;
        }
    }

    return false;
}


void LogicalDevice::destroy() noexcept
{
    if(handle)
    {
        vkDestroyDevice(handle, nullptr);
        handle = nullptr;
    }
}