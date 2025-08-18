#ifndef STRUCTURS_HPP
#define STRUCTURS_HPP

#include <array>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/texture/Texture2D.hpp"


struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() noexcept;
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() noexcept;
};


struct VulkanData
{
    VkPhysicalDevice physicalDevice = nullptr;
    VkDevice         logicalDevice = nullptr;
    VkQueue          queue = nullptr;
    VkCommandPool    commandPool = nullptr;
    VkDescriptorSetLayout descriptorSetLayout = nullptr;
    Texture2D* texture = nullptr;
};


struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


#endif // !STRUCTURS_HPP