#ifndef RENDER_PASS_HPP
#define RENDER_PASS_HPP


class RenderPass
{
public:
    RenderPass() noexcept;
    ~RenderPass();

    bool create(struct VkDevice_T* logicalDevice, int format) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    struct VkRenderPass_T* handle;
};

#endif // !RENDER_PASS_HPP