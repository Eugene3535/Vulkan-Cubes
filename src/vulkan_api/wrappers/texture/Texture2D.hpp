#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include <vulkan/vulkan.h>

class Texture2D
{
public:
    Texture2D() noexcept;

    bool loadFromFile(const char* filepath, VkPhysicalDevice GPU, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept;
    void destroy(VkDevice device) noexcept;

    VkImage        textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView    textureImageView;
    VkSampler      textureSampler;
};

#endif // !TEXTURE2D_HPP