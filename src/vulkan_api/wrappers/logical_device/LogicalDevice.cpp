#include <vulkan/vulkan.h>

#include "vulkan_api/utils/Constants.hpp"
#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/logical_device/LogicalDevice.hpp"


LogicalDevice::LogicalDevice() noexcept:
    handle(nullptr),
    queue(nullptr)
{

}


LogicalDevice::~LogicalDevice() = default;


bool LogicalDevice::create(VkPhysicalDevice_T* physicalDevice) noexcept
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
    
	if(uint32_t mainQueueFamilyIndex = vk::get_main_queue_family_index(static_cast<void*>(physicalDevice)); mainQueueFamilyIndex != UINT32_MAX)
    {
        const float queuePriority = 1.0f;

    	VkDeviceQueueCreateInfo queueCreateInfo = {};
    	queueCreateInfo.sType			 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    	queueCreateInfo.queueFamilyIndex = mainQueueFamilyIndex;
    	queueCreateInfo.queueCount		 = 1;
    	queueCreateInfo.pQueuePriorities = &queuePriority;

        const std::array<const char*, 2> deviceExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_MEMORY_BUDGET_EXTENSION_NAME
        };

        VkDeviceCreateInfo deviceCreateInfo = {};
    	deviceCreateInfo.sType				     = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    	deviceCreateInfo.queueCreateInfoCount	 = 1;
    	deviceCreateInfo.pQueueCreateInfos	     = &queueCreateInfo;
    	deviceCreateInfo.pEnabledFeatures		 = &enabledFeatures;
    	deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
    	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
#ifdef _DEBUG
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