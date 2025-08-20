#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/shader/ShaderModule.hpp"


ShaderModule::ShaderModule() noexcept:
    handle(nullptr)
{

}


VkResult ShaderModule::loadFromFile(VkDevice device, const std::filesystem::path& filepath) noexcept
{
    if (handle)
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

        return vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &handle);
    } 

    return VK_INCOMPATIBLE_SHADER_BINARY_EXT;
}


void ShaderModule::destroy(VkDevice device) noexcept
{
    if (handle)
    {
        vkDestroyShaderModule(device, handle, nullptr);
        handle = nullptr;
    }
}