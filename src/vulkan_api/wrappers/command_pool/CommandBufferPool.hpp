#ifndef COMMAND_BUFFER_POOL_HPP
#define COMMAND_BUFFER_POOL_HPP

#include <array>

#include "vulkan_api/utils/Defines.hpp"

class CommandBufferPool
{
public:
    CommandBufferPool() noexcept;
    ~CommandBufferPool();

    bool create(struct VkDevice_T* logicalDevice, uint32_t mainQueueFamilyIndex) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    struct VkCommandPool_T* handle;
    std::array<struct VkCommandBuffer_T*, MAX_FRAMES_IN_FLIGHT> commandBuffers;
};

#endif // !COMMAND_BUFFER_POOL_HPP