#ifndef VULKAN_HELPERS_HPP
#define VULKAN_HELPERS_HPP

#include <cstdint>
#include <memory>

#include "vulkan_api/utils/Structures.hpp"
#include "vulkan_api/utils/Defines.hpp"

BEGIN_NAMESPACE_VK

bool check_validation_layer_support() noexcept;
uint32_t get_main_queue_family_index(void* physicalDevice) noexcept;

std::unique_ptr<SwapChainSupportDetails> 
query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface) noexcept;

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VulkanData& api) noexcept;
VkCommandBuffer beginSingleTimeCommands(const VulkanData& api) noexcept;
void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VulkanData& api) noexcept;
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const VulkanData& api) noexcept;
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VulkanData& api) noexcept;

END_NAMESPACE_VK

#endif // !VULKAN_HELPERS_HPP