#ifndef STRUCTURS_HPP
#define STRUCTURS_HPP

#include <array>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/texture/Texture2D.hpp"


struct VulkanData
{
    VkPhysicalDevice physicalDevice = nullptr;
    VkDevice         logicalDevice = nullptr;
    VkQueue          queue = nullptr;
    VkCommandPool    commandPool = nullptr;
    VkDescriptorSetLayout descriptorSetLayout = nullptr;
    Texture2D* texture = nullptr;
};

#endif // !STRUCTURS_HPP