#include "vulkan_api/wrappers/pipeline/stages/uniform/DescriptorSetLayout.hpp"


void DescriptorSetLayout::addDescriptor(VkDescriptorType type, VkShaderStageFlagBits shaderStage) noexcept
{
    const uint32_t binding = m_bindings.size();
    const VkShaderStageFlags flags = shaderStage;

    m_bindings.emplace_back(VkDescriptorSetLayoutBinding
        {
            .binding            = binding,
            .descriptorType     = type,
            .descriptorCount    = 1,
            .stageFlags         = flags,
            .pImmutableSamplers = nullptr
        }
    );
}


void DescriptorSetLayout::reset() noexcept
{
    m_bindings.clear();
}


VkDescriptorSetLayoutCreateInfo DescriptorSetLayout::getInfo() const noexcept
{
    return VkDescriptorSetLayoutCreateInfo
    {
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = 0,
        .bindingCount = static_cast<uint32_t>(m_bindings.size()),
        .pBindings    = m_bindings.data()
    };
}
