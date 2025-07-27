#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP


class GraphicsPipeline
{
public:
    GraphicsPipeline() noexcept;
    ~GraphicsPipeline();

    bool create(struct VkDevice_T* logicalDevice) noexcept;
    void destroy() noexcept;

    struct VkDescriptorSetLayout_T* descriptorSetLayout;
    struct VkPipelineLayout_T* layout;
    struct VkPipeline_T* handle;
};

#endif // !GRAPHICS_PIPELINE_HPP