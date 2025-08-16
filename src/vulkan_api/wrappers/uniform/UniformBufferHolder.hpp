#ifndef UNIFORM_BUFFER_HOLDER_HPP
#define UNIFORM_BUFFER_HOLDER_HPP

#include <array>

#include "vulkan_api/utils/Defines.hpp"

class UniformBufferHolder
{
public:
    UniformBufferHolder() noexcept;

    bool create(struct VulkanData& api) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    std::array<struct VkBuffer_T*, MAX_FRAMES_IN_FLIGHT>       uniformBuffers;
    std::array<struct VkDeviceMemory_T*, MAX_FRAMES_IN_FLIGHT> uniformBuffersMemory;
    std::array<void*, MAX_FRAMES_IN_FLIGHT>                    uniformBuffersMapped;

    struct VkDescriptorPool_T* descriptorPool;
    std::array<struct VkDescriptorSet_T*, MAX_FRAMES_IN_FLIGHT> descriptorSets;
};

#endif // !UNIFORM_BUFFER_HOLDER_HPP