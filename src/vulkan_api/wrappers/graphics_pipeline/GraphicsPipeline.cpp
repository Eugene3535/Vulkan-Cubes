#include <array>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/graphics_pipeline/GraphicsPipeline.hpp"


GraphicsPipeline::GraphicsPipeline() noexcept:
    descriptorSetLayout(nullptr),
    layout(nullptr),
    handle(nullptr)
{

}


GraphicsPipeline::~GraphicsPipeline() = default;


bool GraphicsPipeline::create(VkDevice logicalDevice) noexcept
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = 
    { 
        uboLayoutBinding, 
        samplerLayoutBinding 
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        return false;

    return true;
}


void GraphicsPipeline::destroy() noexcept
{

}