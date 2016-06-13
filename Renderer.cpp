#include "Renderer.hpp"
#include <iostream>

#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


Renderer::Renderer(GLFWwindow* window){
    if (!glfwVulkanSupported()) exit(1);    
    
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = NULL;
    applicationInfo.pApplicationName = "test";
    applicationInfo.pEngineName = NULL;
    applicationInfo.engineVersion = 1;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;
    
    this->window = window;
    
    if (!initInstance()) exit(1);
    if (!initSurface(window)) exit(1);
    if (!initDevice()) exit(1);
}

Renderer::~Renderer(){ 

    destroyDevice();
    destroySurface();
    destroyInstance();   
}


bool Renderer::initInstance() {
    VkInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = NULL;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = NULL;
    
    uint32_t count;
    instanceCreateInfo.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&count);
    instanceCreateInfo.enabledExtensionCount = count;

    VkResult result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create instance: " << result << std::endl;
        return false;
    }
    
    return true;
}

void Renderer::destroyInstance() {
    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, NULL);
    }
}

bool Renderer::initDevice() {
    uint32_t deviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    
    if (result != VK_SUCCESS) return false;
    if (deviceCount == 0) return false;

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
    if (result != VK_SUCCESS) return false;
    
    float queuePriorities[] = { 1.0f };
    VkDeviceQueueCreateInfo deviceQueueInfo; 
    deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueInfo.pNext = NULL;
    deviceQueueInfo.flags = 0;
    deviceQueueInfo.queueFamilyIndex = 0;
    deviceQueueInfo.queueCount = 1;
    deviceQueueInfo.pQueuePriorities = queuePriorities;
    
    VkDeviceCreateInfo deviceInfo;
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.flags = 0;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = NULL;
    std::vector<const char*> vextensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(vextensions.size());
    deviceInfo.ppEnabledExtensionNames = &vextensions[0];
    deviceInfo.pEnabledFeatures = NULL;

    
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &deviceQueueInfo;

    result = vkCreateDevice(physicalDevices[0], &deviceInfo, NULL, &device);
    if (result != VK_SUCCESS) return false;
    
    return true;
}

void Renderer::destroyDevice() {
    if (device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, NULL);
    }
}

bool Renderer::initSurface(GLFWwindow* window) {
    VkResult result = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if (result != VK_SUCCESS) return false;
    
    return true;
}

void Renderer::destroySurface() {
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, NULL);
    }
}
