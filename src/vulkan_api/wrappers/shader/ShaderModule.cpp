#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/shader/ShaderModule.hpp"


ShaderModule::ShaderModule() noexcept:
    handle(nullptr)
{

}


ShaderModule::~ShaderModule() = default;


bool ShaderModule::loadFromFile(struct VkDevice_T* logicalDevice, const std::filesystem::path& filepath) noexcept
{
    if(handle)
        return true;

    std::ifstream stream;
    stream.open(filepath, std::ios::ate | std::ios::binary);

    if (stream.is_open())
    {
        size_t fileSize = (size_t)stream.tellg();
        std::vector<char> byte_code(fileSize);

        stream.seekg(0);
        stream.read(byte_code.data(), fileSize);
        stream.close();

        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = byte_code.size();
        shaderModuleCreateInfo.pCode    = reinterpret_cast<const uint32_t*>(byte_code.data());

        return (vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &handle) == VK_SUCCESS);
    } 

    return false;
}


void ShaderModule::destroy(VkDevice logicalDevice) noexcept
{
    if(handle)
    {
        vkDestroyShaderModule(logicalDevice, handle, nullptr);
        handle = nullptr;
    }
}