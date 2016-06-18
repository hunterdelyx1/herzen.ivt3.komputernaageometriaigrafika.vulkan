#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class Renderer {
    private:
        GLFWwindow* window;
        
        VkApplicationInfo applicationInfo;
        
        std::string getVulkanErrorString(VkResult result);
        
        VkInstance instance = VK_NULL_HANDLE;
        bool initInstance();
        void destroyInstance();
        
        uint32_t queueFamilyIndex = -1;
        VkQueue queue = VK_NULL_HANDLE;
        VkPhysicalDevice gpu = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        bool initDevice();
        void destroyDevice();
        
        VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
        VkSurfaceFormatKHR surfaceFormat = {};
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        bool initSurface(GLFWwindow* window);
        void destroySurface();
        
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkFence fence = VK_NULL_HANDLE;
        bool initCommands();
        void destroyCommands();
        
        uint32_t swapchainImageCount = 2;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        bool initSwapchain();
        void destroySwapchain();

        uint32_t currentImage = 0;
        std::vector<VkImage> swapchainImages = {};
        std::vector<VkImageView> swapchainImageViews = {};
        std::vector<VkFramebuffer> swapchainFramebuffers = {};
        bool initSwapchainImages();
        void destroySwapchainImages();
        
        VkRenderPass renderPass = VK_NULL_HANDLE;
        bool initRenderPass();
        void destroyRenderPass();
        
        void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
            VkImageAspectFlags aspects,
            VkImageLayout oldLayout,
            VkImageLayout newLayout);
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
