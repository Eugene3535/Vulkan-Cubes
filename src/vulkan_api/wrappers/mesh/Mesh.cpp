#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/mesh/Mesh.hpp"


static const std::vector<Vertex> vertices = 
{
    {{ -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
    {{  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
    {{  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }},
    {{ -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }}
};


static const std::array<uint16_t, 6> indices = 
{
    0, 1, 2, 2, 3, 0
};



Mesh::Mesh() noexcept:
    vertexBuffer(nullptr),
    vertexBufferMemory(nullptr),
    indexBuffer(nullptr),
    indexBufferMemory(nullptr)
{

}


bool Mesh::create(const VulkanApi& api) noexcept
{
    {// Vertex buffer
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        vk::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, api);

        void *data;
        vkMapMemory(api.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(api.logicalDevice, stagingBufferMemory);

        vk::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, api);

        vk::copyBuffer(stagingBuffer, vertexBuffer, bufferSize, api);

        vkDestroyBuffer(api.logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(api.logicalDevice, stagingBufferMemory, nullptr);
    }

    {// Index buffer
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        vk::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, api);

        void *data;
        vkMapMemory(api.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(api.logicalDevice, stagingBufferMemory);

        vk::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory, api);

        vk::copyBuffer(stagingBuffer, indexBuffer, bufferSize, api);

        vkDestroyBuffer(api.logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(api.logicalDevice, stagingBufferMemory, nullptr);
    }

    return true;
}


void Mesh::destroy(VkDevice logicalDevice) noexcept
{
    vkDestroyBuffer(logicalDevice, indexBuffer, nullptr);
    vkFreeMemory(logicalDevice, indexBufferMemory, nullptr);

    vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);
}


uint32_t Mesh::getIndexCount() const noexcept
{
    return static_cast<uint32_t>(indices.size());
}