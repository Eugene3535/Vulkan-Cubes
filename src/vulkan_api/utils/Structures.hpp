#ifndef STRUCTURS_HPP
#define STRUCTURS_HPP

#include <array>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vulkan/vulkan.h>


struct SwapChainSupportDetails 
{
    VkPresentModeKHR   getPresentMode()   const noexcept;
    VkSurfaceFormatKHR getSurfaceFormat() const noexcept;

    VkSurfaceCapabilitiesKHR		capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>	presentModes;
};


struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() noexcept;
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() noexcept;
};


struct VulkanApi
{
    VkPhysicalDevice physicalDevice = nullptr;
    VkDevice         logicalDevice = nullptr;
    VkQueue          queue = nullptr;
    VkCommandPool    commandPool = nullptr;
};


#endif // !STRUCTURS_HPP