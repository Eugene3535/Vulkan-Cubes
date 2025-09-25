#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "vulkan_api/utils/Defines.hpp"
#include "vulkan_api/wrappers/presentation/MainView.hpp"
#include "vulkan_api/wrappers/pipeline/GraphicsPipeline.hpp"
#include "vulkan_api/wrappers/command_pool/CommandBufferPool.hpp"
#include "vulkan_api/wrappers/sync/SyncManager.hpp"
#include "vulkan_api/wrappers/texture/Texture2D.hpp"
#include "vulkan_api/wrappers/mesh/Mesh.hpp"
#include "vulkan_api/wrappers/uniform/UniformBufferHolder.hpp"


class Application
{
public:
    int run() noexcept;

private:
    void initWindow() noexcept;
    bool initVulkan() noexcept;
    void mainLoop() noexcept;
    void cleanup() noexcept;
    void recreateSwapChain() noexcept;
    void updateUniformBuffer(uint32_t currentImage) noexcept;

    void writeCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const Mesh& mesh, VkDescriptorSet descriptorSet) noexcept;
    void drawFrame() noexcept;

    struct GLFWwindow* window;

    VulkanApi m_api;
    MainView  m_mainView;
    GraphicsPipeline  m_pipeline;
    
    CommandBufferPool m_commandPool;
    SyncManager       m_sync;

    Texture2D m_texture;
    Mesh      m_mesh;

    UniformBufferHolder m_uniformBuffers;

    bool framebufferResized = false;
    int32_t m_width = 0;
    int32_t m_height = 0;
};

#endif // !APPLICATION_HPP