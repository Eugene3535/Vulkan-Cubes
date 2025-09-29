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

    struct VkDescriptorPool_T* descriptorPool;
    std::array<struct VkDescriptorSet_T*, MAX_FRAMES_IN_FLIGHT> descriptorSets;
};

#endif // !UNIFORM_BUFFER_HOLDER_HPP