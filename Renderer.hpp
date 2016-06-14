#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class Renderer {
    private:
        GLFWwindow* window;
        
        VkApplicationInfo applicationInfo;
        
        VkInstance instance = VK_NULL_HANDLE;
        bool initInstance();
        void destroyInstance();
        
        VkPhysicalDevice gpu = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        bool initDevice();
        void destroyDevice();
        
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        bool initSurface(GLFWwindow* window);
        void destroySurface();
    public:
        Renderer(GLFWwindow* window);
        ~Renderer();
        
        void waitReady() {
            if (device != VK_NULL_HANDLE) {
                vkDeviceWaitIdle(device);
            }
        }
        
        void update() {
            glfwPollEvents();
        }
};
