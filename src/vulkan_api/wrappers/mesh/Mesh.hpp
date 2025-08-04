#ifndef MESH_HPP
#define MESH_HPP


class Mesh
{
public:
    Mesh() noexcept;

    bool create(const struct VulkanApi& api) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    unsigned getIndexCount() noexcept;
    
    struct VkBuffer_T*       vertexBuffer;
    struct VkDeviceMemory_T* vertexBufferMemory;
    struct VkBuffer_T*       indexBuffer;
    struct VkDeviceMemory_T* indexBufferMemory;
};

#endif