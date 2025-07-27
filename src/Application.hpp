#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <unordered_set>
#include <optional>
#include <set>

#include "vulkan_api/utils/Defines.hpp"
#include "vulkan_api/utils/Constants.hpp"
#include "vulkan_api/wrappers/instance/VulkanInstance.hpp"
#include "vulkan_api/wrappers/physical_device/PhysicalDevice.hpp"
#include "vulkan_api/wrappers/logical_device/LogicalDevice.hpp"
#include "vulkan_api/wrappers/surface/Surface.hpp"
#include "vulkan_api/wrappers/swapchain/Swapchain.hpp"
#include "vulkan_api/wrappers/render_pass/RenderPass.hpp"


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;


const std::array<const char*, 1> validationLayers = 
{
    "VK_LAYER_KHRONOS_validation"
};


const std::array<const char*, 1> deviceExtensions = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) noexcept;
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) noexcept;


struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() noexcept
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() noexcept
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
};


struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


const std::vector<Vertex> vertices = 
{
    {{ -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
    {{  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
    {{  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }},
    {{ -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }}
};


const std::array<uint16_t, 6> indices = 
{
    0, 1, 2, 2, 3, 0
};


class Application
{
public:
    void run() noexcept;

private:
    void initWindow() noexcept;
    bool initVulkan() noexcept;
    void mainLoop() noexcept;
    void cleanup() noexcept;
    void recreateSwapChain() noexcept;
    void createDescriptorSetLayout() noexcept;
    void createGraphicsPipeline() noexcept;
    void createCommandPool() noexcept;
    void createTextureImage() noexcept;
    void createTextureImageView() noexcept;
    void createTextureSampler() noexcept;
    VkImageView createImageView(VkImage image, VkFormat format) noexcept;
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) noexcept;
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) noexcept;
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) noexcept;
    void createVertexBuffer() noexcept;
    void createIndexBuffer() noexcept;
    void createUniformBuffers() noexcept;
    void createDescriptorPool() noexcept;
    void createDescriptorSets() noexcept;
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) noexcept;

    VkCommandBuffer beginSingleTimeCommands() noexcept;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) noexcept;
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) noexcept;
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) noexcept;
    void createCommandBuffers() noexcept;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) noexcept;
    void createSyncObjects() noexcept;
    void updateUniformBuffer(uint32_t currentImage) noexcept;
    void drawFrame() noexcept;


    GLFWwindow* window;

    VulkanInstance m_instance;
    PhysicalDevice m_physicalDevice;
    LogicalDevice  m_logicalDevice;
    Surface        m_surface;
    Swapchain      m_swapchain;
    RenderPass     m_renderPass;


    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    bool framebufferResized = false;
};

#endif // !APPLICATION_HPP