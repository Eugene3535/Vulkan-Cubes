#ifndef DESCRIPTOR_POOL_HPP
#define DESCRIPTOR_POOL_HPP

#include <span>
#include <vector>

#include <vulkan/vulkan.h>


class DescriptorPool
{
public:
    DescriptorPool(VkDevice device) noexcept;
    DescriptorPool(const DescriptorPool&) noexcept = delete;
    DescriptorPool(DescriptorPool&&) noexcept = delete;
    DescriptorPool& operator = (const DescriptorPool&) noexcept = delete;
    DescriptorPool& operator = (DescriptorPool&&) noexcept = delete;

    VkResult create(std::span<const VkDescriptorPoolSize> poolSizes) noexcept;
    VkResult allocateDescriptorSets(std::span<VkDescriptorSet> descriptorSets, std::span<const VkDescriptorSetLayout> layouts) noexcept;
    void writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept;

    void destroy() noexcept;

    const VkDescriptorPool            getPool()  const noexcept;
    std::span<const VkDescriptorType> getTypes() const noexcept;

private:
    VkDevice m_device;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorType> m_types;
};

#endif // !DESCRIPTOR_POOL_HPP