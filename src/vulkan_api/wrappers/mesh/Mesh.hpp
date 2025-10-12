#ifndef MESH_HPP
#define MESH_HPP

#include <cstdint>

#include <vulkan/vulkan.h>

class Mesh
{
public:
    Mesh() noexcept;

    bool create(VkPhysicalDevice GPU, VkDevice device, VkCommandPool pool, VkQueue queue) noexcept;
    void destroy(VkDevice device) noexcept;

    uint32_t getIndexCount() const noexcept;
    
    VkBuffer       vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer       indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

#endif