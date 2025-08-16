#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>

class Mesh
{
public:
    Mesh() noexcept;

    bool create(const struct VulkanData& api) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    uint32_t getIndexCount() const noexcept;
    
    struct VkBuffer_T*       vertexBuffer;
    struct VkDeviceMemory_T* vertexBufferMemory;
    struct VkBuffer_T*       indexBuffer;
    struct VkDeviceMemory_T* indexBufferMemory;
};

#endif