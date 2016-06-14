#include "Renderer.hpp"
#include <iostream>

#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


Renderer::Renderer(GLFWwindow* window){
    if (!glfwVulkanSupported()) exit(1);    
    
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pApplicationName = "test";
    
    this->window = window;

    if (!initInstance()) exit(1);
    //if (!initSurface(window)) exit(1);
    if (!initDevice()) exit(1);
}

Renderer::~Renderer(){ 
    destroyDevice();
    //destroySurface();
    destroyInstance();   
}


bool Renderer::initInstance() {
    VkInstanceCreateInfo instanceCreateInfo {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    
    instanceCreateInfo.pNext = NULL;
    
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = NULL;
    instanceCreateInfo.enabledExtensionCount = 0;
    instanceCreateInfo.ppEnabledExtensionNames = NULL;

    //uint32_t count;
    //instanceCreateInfo.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&count);
    //instanceCreateInfo.enabledExtensionCount = count;

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
    VkResult result;
    
    // Get GPU
    // {{
    uint32_t gpu_count = 0;
    result = vkEnumeratePhysicalDevices(instance, &gpu_count, NULL);
    
    if (result != VK_SUCCESS) return false;
    if (gpu_count == 0) return false;

    std::vector<VkPhysicalDevice> gpu_list(gpu_count);
    result = vkEnumeratePhysicalDevices(instance, &gpu_count, gpu_list.data());
    if (result != VK_SUCCESS) return false;
    
    this->gpu = gpu_list[0];
    // }}
    
    // Get index of queue family that supports graphics
    // {{
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(this->gpu, &family_count, NULL);
    std::vector<VkQueueFamilyProperties> family_property_list(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(this->gpu, &family_count, family_property_list.data());
    
    uint32_t family_index;
    for (family_index = 0; family_index < family_count; family_index++) {
        if (family_property_list[family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            break;
        }
    }
    if (family_index >= family_count) return false;
    // }}
    
    float queue_priorities[] { 1.0f };
    VkDeviceQueueCreateInfo deviceQueueCreateInfo {}; 
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.queueFamilyIndex = family_index;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = queue_priorities;
    
    VkDeviceCreateInfo deviceCreateInfo {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    
    result = vkCreateDevice(this->gpu, &deviceCreateInfo, NULL, &device);
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
