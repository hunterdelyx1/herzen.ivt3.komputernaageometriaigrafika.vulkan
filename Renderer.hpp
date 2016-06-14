#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class Renderer {
    private:
        GLFWwindow* window;
        
        VkApplicationInfo applicationInfo;
        
        VkInstance instance = VK_NULL_HANDLE;
        bool initInstance();
        void destroyInstance();
        
        uint32_t queueFamilyIndex = -1;
        VkQueue queue = VK_NULL_HANDLE;
        VkPhysicalDevice gpu = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        bool initDevice();
        void destroyDevice();
        
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        bool initSurface(GLFWwindow* window);
        void destroySurface();
        
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkFence fence = VK_NULL_HANDLE;
        bool initCommands();
        void destroyCommands();
    public:
        Renderer(GLFWwindow* window);
        ~Renderer();
        
        void waitReady() {
            if (device != VK_NULL_HANDLE) {
                vkDeviceWaitIdle(device);
            }
        }
        
        void draw();
        
        void update() {
            glfwPollEvents();
        }
};
