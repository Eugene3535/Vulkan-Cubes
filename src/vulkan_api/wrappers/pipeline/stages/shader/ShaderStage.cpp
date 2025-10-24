#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/pipeline/stages/shader/ShaderStage.hpp"


ShaderStage::ShaderStage() noexcept:
    m_handle(nullptr),
    m_stage(VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
{

}


VkResult ShaderStage::loadFromFile(VkDevice device, VkShaderStageFlagBits stage, const std::filesystem::path& filepath) noexcept
{
    if (m_handle)
        destroy(device);

    std::ifstream stream;
    stream.open(filepath, std::ios::ate | std::ios::binary);

    if (stream.is_open())
    {
        size_t fileSize = (size_t)stream.tellg();
        std::vector<char> byte_code(fileSize);

        stream.seekg(0);
        stream.read(byte_code.data(), fileSize);
        stream.close();

        const VkShaderModuleCreateInfo shaderModuleInfo = 
        {
            .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = 0,
            .codeSize = byte_code.size(),
            .pCode    = reinterpret_cast<const uint32_t*>(byte_code.data())
        };

        if (auto result = vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &m_handle); result == VK_SUCCESS)
        {
            m_stage = stage;

            return result;
        }
    } 

    return VK_ERROR_INITIALIZATION_FAILED;
}


void ShaderStage::destroy(VkDevice device) noexcept
{
    if (m_handle)
    {
        vkDestroyShaderModule(device, m_handle, nullptr);
        m_handle = nullptr;
        m_stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }
}


VkPipelineShaderStageCreateInfo ShaderStage::getInfo() const noexcept
{
    if(m_handle && m_stage != VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
    {
        VkPipelineShaderStageCreateInfo info
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0,
            .stage               = m_stage,
            .module              = m_handle,
            .pName               = "main",
            .pSpecializationInfo = nullptr
        };

        return info;
    }

    return {};
}