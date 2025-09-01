#ifndef COMMAND_BUFFER_POOL_HPP
#define COMMAND_BUFFER_POOL_HPP

#include <array>

#include <vulkan/vulkan.h>

#include "vulkan_api/utils/Defines.hpp"

class CommandBufferPool
{
public:
    CommandBufferPool() noexcept;

    bool create(VkDevice device, uint32_t mainQueueFamilyIndex) noexcept;
    void destroy(VkDevice device) noexcept;

    VkCommandPool handle;
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> commandBuffers;
};

#endif // !COMMAND_BUFFER_POOL_HPP