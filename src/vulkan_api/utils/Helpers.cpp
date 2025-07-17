#include <vector>

#include "vulkan_api/utils/Constants.hpp"
#include "vulkan_api/utils/Helpers.hpp"


BEGIN_NAMESPACE_VK


bool check_validation_layer_support() noexcept
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, VK_NULL_HANDLE);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : VALIDATION_LAYERS)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}


uint32_t get_main_queue_family_index(void* physicalDevice) noexcept
{
    if(physicalDevice)
    {
        auto device = static_cast<VkPhysicalDevice>(physicalDevice);

        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (size_t i = 0; i < queueFamilies.size(); ++i)
        {
            if (queueFamilies[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))
            {
                return static_cast<uint32_t>(i);
            }
        }
    }

    return UINT32_MAX;
}

END_NAMESPACE_VK