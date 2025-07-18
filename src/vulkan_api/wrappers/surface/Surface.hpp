#ifndef SURFACE_HPP
#define SURFACE_HPP


class Surface
{
public:
    Surface() noexcept;
    ~Surface();

    bool create(struct VkInstance_T* instance, struct GLFWwindow* window) noexcept;
    void destroy(struct VkInstance_T* instance) noexcept;


    struct VkSurfaceKHR_T* handle;
};

#endif // !SURFACE_HPP