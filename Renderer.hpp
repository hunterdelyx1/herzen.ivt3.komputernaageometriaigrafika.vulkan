#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class Renderer {
    private:
        GLFWwindow* window;
        
        VkApplicationInfo applicationInfo;
        
        VkInstance instance;
        bool initInstance();
        void destroyInstance();

        VkDevice device;
        bool initDevice();
        void destroyDevice();
        
        VkSurfaceKHR surface;
        bool initSurface(GLFWwindow* window);
        void destroySurface();
    public:
        Renderer(GLFWwindow* window);
        ~Renderer();
        
        void waitReady() {
            vkDeviceWaitIdle(device);
        }
        
        void update() {
            glfwPollEvents();
        }
};
