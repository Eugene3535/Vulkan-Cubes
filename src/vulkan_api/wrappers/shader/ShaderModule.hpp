#ifndef SHADER_MODULE_HPP
#define SHADER_MODULE_HPP

#include <filesystem>

#include <vulkan/vulkan.h>


class ShaderModule
{
public:
    ShaderModule() noexcept;

    VkResult loadFromFile(VkDevice device, const std::filesystem::path& filepath) noexcept;
    void destroy(VkDevice device) noexcept;

    VkShaderModule handle;
};

#endif // !SHADER_MODULE_HPP