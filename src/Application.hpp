#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "vulkan_api/utils/Defines.hpp"
#include "vulkan_api/utils/Constants.hpp"
#include "vulkan_api/wrappers/instance/VulkanInstance.hpp"
#include "vulkan_api/wrappers/physical_device/PhysicalDevice.hpp"
#include "vulkan_api/wrappers/logical_device/LogicalDevice.hpp"
#include "vulkan_api/wrappers/surface/Surface.hpp"
#include "vulkan_api/wrappers/swapchain/Swapchain.hpp"
#include "vulkan_api/wrappers/graphics_pipeline/GraphicsPipeline.hpp"
#include "vulkan_api/wrappers/command_pool/CommandBufferPool.hpp"
#include "vulkan_api/wrappers/sync/SyncManager.hpp"
#include "vulkan_api/wrappers/texture/Texture2D.hpp"
#include "vulkan_api/wrappers/mesh/Mesh.hpp"
#include "vulkan_api/wrappers/uniform/UniformBufferHolder.hpp"


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

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) noexcept;
    void updateUniformBuffer(uint32_t currentImage) noexcept;
    void drawFrame() noexcept;

    struct GLFWwindow* window;

    VulkanInstance    m_instance;
    PhysicalDevice    m_physicalDevice;
    LogicalDevice     m_logicalDevice;
    Surface           m_surface;
    Swapchain         m_swapchain;
    GraphicsPipeline  m_pipeline;
    CommandBufferPool m_commandPool;
    SyncManager       m_sync;

    Texture2D m_texture;
    Mesh      m_mesh;

    UniformBufferHolder m_uniformBuffers;

    bool framebufferResized = false;
};

#endif // !APPLICATION_HPP