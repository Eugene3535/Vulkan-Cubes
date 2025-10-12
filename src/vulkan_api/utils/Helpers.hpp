#ifndef VULKAN_HELPERS_HPP
#define VULKAN_HELPERS_HPP

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

#include "vulkan_api/utils/Defines.hpp"
#include "vulkan_api/utils/Structures.hpp"

BEGIN_NAMESPACE_VK

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice GPU) noexcept;

VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool pool) noexcept;
void endSingleTimeCommands(VkCommandBuffer cmd, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept;

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDevice device, VkPhysicalDevice GPU) noexcept;
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept;

END_NAMESPACE_VK

#endif // !VULKAN_HELPERS_HPP