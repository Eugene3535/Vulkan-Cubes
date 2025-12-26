#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <cglm/struct/vec3.h>
#include <cglm/call/mat4.h>

#include "vulkan_api/utils/Defines.hpp"
#include "vulkan_api/wrappers/presentation/MainView.hpp"
#include "vulkan_api/wrappers/pipeline/GraphicsPipeline.hpp"
#include "vulkan_api/wrappers/pipeline/descriptors/DescriptorPool.hpp"
#include "vulkan_api/wrappers/command_pool/CommandBufferPool.hpp"
#include "vulkan_api/wrappers/sync/SyncManager.hpp"
#include "vulkan_api/wrappers/texture/Texture2D.hpp"
#include "vulkan_api/resources/VkResourceHolder.hpp"

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
    void updateUniformBuffer(vec3s pos, float angle) noexcept;

    void writeCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkDescriptorSet descriptorSet) noexcept;
    void drawFrame() noexcept;

    struct GLFWwindow* window;

    VulkanContext m_context;
    MainView  m_mainView;
    GraphicsPipeline  m_pipeline;
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> m_descriptorSets {};
    std::unique_ptr<DescriptorPool> m_descriptorPool;
    
    CommandBufferPool m_commandPool;
    SyncManager       m_sync;

    Texture2D m_texture;

    std::unique_ptr<VkResourceHolder> m_holder;
    Buffer m_vertices;
    Buffer m_indices;

    mat4s m_mvp;

    bool framebufferResized = false;
    int32_t m_width = 0;
    int32_t m_height = 0;
};

#endif // !APPLICATION_HPP