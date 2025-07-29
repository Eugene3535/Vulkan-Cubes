#ifndef VULKAN_HELPERS_HPP
#define VULKAN_HELPERS_HPP

#include <cstdint>
#include <memory>

#include "vulkan_api/utils/Defines.hpp"

BEGIN_NAMESPACE_VK

bool check_validation_layer_support() noexcept;
uint32_t get_main_queue_family_index(void* physicalDevice) noexcept;

std::unique_ptr<struct SwapChainSupportDetails> 
query_swapchain_support(struct VkPhysicalDevice_T* device, struct VkSurfaceKHR_T* surface) noexcept;

END_NAMESPACE_VK

#endif // !VULKAN_HELPERS_HPP