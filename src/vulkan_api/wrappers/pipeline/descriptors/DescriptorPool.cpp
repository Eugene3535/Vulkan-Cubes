#include "vulkan_api/utils/Defines.hpp"
#include "vulkan_api/wrappers/pipeline/descriptors/DescriptorPool.hpp"


DescriptorPool::DescriptorPool(VkDevice device) noexcept:
    m_device(device),
    m_descriptorPool(nullptr)
{

}


VkResult DescriptorPool::create(std::span<const VkDescriptorPoolSize> poolSizes) noexcept
{
    if(poolSizes.empty())
        return VK_ERROR_INITIALIZATION_FAILED;

    if(m_descriptorPool)
        return VK_SUCCESS;

    const VkDescriptorPoolCreateInfo poolInfo = 
    {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .maxSets       = MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes    = poolSizes.data()
    };

    if(auto result = vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool); result == VK_SUCCESS)
    {
        for(auto poolSize : poolSizes)
            m_types.push_back(poolSize.type);
       
        return result;
    }

    return VK_ERROR_INITIALIZATION_FAILED;
}


VkResult DescriptorPool::allocateDescriptorSets(std::span<VkDescriptorSet> descriptorSets, std::span<const VkDescriptorSetLayout> layouts) noexcept
{
    if(m_descriptorPool)
    {
        VkDescriptorSetAllocateInfo allocateInfo = 
        {
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext              = nullptr,
            .descriptorPool     = m_descriptorPool,
            .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
            .pSetLayouts        = layouts.data()
        };

        return vkAllocateDescriptorSets(m_device, &allocateInfo, descriptorSets.data());
    }

    return VK_ERROR_OUT_OF_POOL_MEMORY;
}


void DescriptorPool::writeCombinedImageSampler(const VkDescriptorImageInfo* imageInfo, VkDescriptorSet descriptorSet, uint32_t dstBinding) noexcept
{
    VkWriteDescriptorSet descriptorWrite = 
    {
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = descriptorSet,
        .dstBinding       = dstBinding,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = imageInfo,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr
    };

    vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
}


void DescriptorPool::destroy() noexcept
{
    if(m_descriptorPool)
    {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
        m_descriptorPool = nullptr;
        m_types.clear();
    }
}


const VkDescriptorPool DescriptorPool::getPool() const noexcept
{
    return m_descriptorPool;
}


std::span<const VkDescriptorType> DescriptorPool::getTypes() const noexcept
{
    return m_types;
}