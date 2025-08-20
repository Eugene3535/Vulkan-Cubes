#ifndef SHADER_MODULE_HPP
#define SHADER_MODULE_HPP

#include <filesystem>

#include <vulkan/vulkan.h>


class ShaderModule
{
public:
    ShaderModule() noexcept;

    VkResult loadFromFile(VkDevice device, VkShaderStageFlagBits stage, const std::filesystem::path& filepath) noexcept;
    void destroy(VkDevice device) noexcept;

    VkPipelineShaderStageCreateInfo getInfo() const noexcept;

private:
    VkShaderModule m_handle;
    VkShaderStageFlagBits m_stage;
};

#endif // !SHADER_MODULE_HPP