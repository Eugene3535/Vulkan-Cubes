#ifndef VULKAN_API_HPP
#define VULKAN_API_HPP

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

class VulkanApi
{
public:
    VulkanApi() noexcept;
    ~VulkanApi();

    bool initialize() noexcept;

private:
    bool createInstance()       noexcept;
    bool selectPhysicalDevice() noexcept;

    VkInstance       m_instance;
    VkPhysicalDevice m_physicalDevice;
};


#endif // !VULKAN_API_HPP