#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP

#include <span>


class GraphicsPipeline
{
public:
    GraphicsPipeline() noexcept;
    ~GraphicsPipeline();

    bool create(struct VkDevice_T* logicalDevice, std::span<const class ShaderModule> shaders, const class Swapchain& swapchain) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    struct VkDescriptorSetLayout_T* descriptorSetLayout;
    struct VkPipelineLayout_T*      layout;
    struct VkPipeline_T*            handle;
};

#endif // !GRAPHICS_PIPELINE_HPP