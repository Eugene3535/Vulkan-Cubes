#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <glm/gtc/matrix_transform.hpp>

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
    void updateUniformBuffer(uint32_t currentImage, bool b = false) noexcept;

    void writeCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkDescriptorSet descriptorSet) noexcept;
    void drawFrame() noexcept;

    struct GLFWwindow* window;

    VulkanApi m_api;
    MainView  m_mainView;
    GraphicsPipeline  m_pipeline;
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> m_descriptorSets {};
    std::unique_ptr<DescriptorPool> m_descriptorPool;

    VkImage        m_depthImage       = VK_NULL_HANDLE;
    VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;
    VkImageView    m_depthImageView   = VK_NULL_HANDLE;
    
    CommandBufferPool m_commandPool;
    SyncManager       m_sync;

    Texture2D m_texture;

    std::unique_ptr<VkResourceHolder> m_holder;
    Buffer m_vertices;
    Buffer m_indices;

    glm::mat4 m_mvp = glm::mat4(1.f);

    bool framebufferResized = false;
    int32_t m_width = 0;
    int32_t m_height = 0;
};

#endif // !APPLICATION_HPP