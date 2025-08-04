#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

class Texture2D
{
public:
    Texture2D() noexcept;
    ~Texture2D();

    bool loadFromFile(const char* filepath, const struct VulkanApi& api) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    struct VkImage_T*        textureImage;
    struct VkDeviceMemory_T* textureImageMemory;
    struct VkImageView_T*    textureImageView;
    struct VkSampler_T*      textureSampler;
};

#endif // !TEXTURE2D_HPP