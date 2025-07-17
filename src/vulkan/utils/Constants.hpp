#ifndef VULKAN_CONSTANTS_HPP
#define VULKAN_CONSTANTS_HPP

#include <array>

#include <vulkan/vulkan.h>

constexpr std::array<const char*, 1> VALIDATION_LAYERS = 
{
    "VK_LAYER_KHRONOS_validation"
};

constexpr std::array<const char*, 1> DEVICE_EXTENSIONS = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#endif // !VULKAN_CONSTANTS_HPP