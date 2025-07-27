#ifndef SHADER_MODULE_HPP
#define SHADER_MODULE_HPP

#include <filesystem>


class ShaderModule
{
public:
    ShaderModule() noexcept;
    ~ShaderModule();

    bool loadFromFile(struct VkDevice_T* logicalDevice, const std::filesystem::path& filepath) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    struct VkShaderModule_T* handle;
};

#endif // !SHADER_MODULE_HPP