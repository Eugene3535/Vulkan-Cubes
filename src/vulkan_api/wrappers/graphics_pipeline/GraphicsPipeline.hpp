#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP

#include <span>


class GraphicsPipeline
{
public:
    GraphicsPipeline() noexcept;
    ~GraphicsPipeline();

    bool create(struct VkDevice_T* logicalDevice, std::span<const class ShaderModule> shaders, const class Swapchain& swapchain) noexcept;
    bool writeCommandBuffer(struct VkCommandBuffer_T* commandBuffer, uint32_t currentFrame, uint32_t imageIndex, const class Mesh& mesh, struct VkDescriptorSet_T* descriptorSet) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    struct VkDescriptorSetLayout_T* descriptorSetLayout;
    struct VkPipelineLayout_T*      layout;
    struct VkPipeline_T*            handle;

private:
    const class Swapchain* m_swapchain;
};

#endif // !GRAPHICS_PIPELINE_HPP