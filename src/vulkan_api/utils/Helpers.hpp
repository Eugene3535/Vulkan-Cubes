#ifndef VULKAN_HELPERS_HPP
#define VULKAN_HELPERS_HPP

#include "vulkan_api/utils/Defines.hpp"

BEGIN_NAMESPACE_VK

bool check_validation_layer_support() noexcept;
uint32_t get_main_queue_family_index(void* physicalDevice) noexcept;

END_NAMESPACE_VK

#endif // !VULKAN_HELPERS_HPP