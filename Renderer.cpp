#include "Renderer.hpp"
#include <iostream>

#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <climits>
#include <string>

std::string Renderer::getVulkanErrorString(VkResult result) {
    switch(result) {
        case VK_ERROR_OUT_OF_HOST_MEMORY: 
            return "VK_ERROR_OUT_OF_HOST_MEMORY"; 
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: 
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY"; 
            break;
        case VK_ERROR_INITIALIZATION_FAILED: 
            return "VK_ERROR_INITIALIZATION_FAILED"; 
            break;
        case VK_ERROR_DEVICE_LOST: 
            return "VK_ERROR_DEVICE_LOST"; 
            break;
        case VK_ERROR_MEMORY_MAP_FAILED: 
            return "VK_ERROR_MEMORY_MAP_FAILED"; 
            break;
        case VK_ERROR_LAYER_NOT_PRESENT: 
            return "VK_ERROR_LAYER_NOT_PRESENT"; 
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT: 
            return "VK_ERROR_EXTENSION_NOT_PRESENT"; 
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT: 
            return "VK_ERROR_FEATURE_NOT_PRESENT"; 
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER: 
            return "VK_ERROR_INCOMPATIBLE_DRIVER"; 
            break;
        case VK_ERROR_TOO_MANY_OBJECTS: 
            return "VK_ERROR_TOO_MANY_OBJECTS"; 
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED: 
            return "VK_ERROR_FORMAT_NOT_SUPPORTED"; 
            break;
        default: 
            return "No error, code: " +  std::to_string(result);
    }
}

// Draw
void Renderer::draw() {
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
    VkViewport viewport {};
    viewport.maxDepth = 1.0f;
    viewport.minDepth = 0.0f;
    viewport.width = 512;
    viewport.height = 512;
    viewport.x = 0;
    viewport.y = 0;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    
    vkEndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    vkQueueSubmit(queue, 1, &submitInfo, fence);
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT_MAX);
 }

// Init
Renderer::Renderer(GLFWwindow* window){
    if (!glfwVulkanSupported()) exit(1);    
    
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pApplicationName = "test";
    
    this->window = window;

    if (!initInstance()) exit(1);
    if (!initDevice()) exit(1);
    if (!initSurface(window)) exit(1);

    if (!initCommands()) exit(1);

    if (!initSwapchain()) exit(1);
    if (!initSwapchainImages()) exit(1);
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

    uint32_t count;
    instanceCreateInfo.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&count);
    instanceCreateInfo.enabledExtensionCount = count;

    VkResult result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create instance: " << getVulkanErrorString(result) << std::endl;
        return false;
    }
    
    return true;
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
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create device: " << getVulkanErrorString(result) << std::endl;
        return false;
     }
        
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

    if (family_index >= family_count) {
        std::cout << "Cannot get family index." << std::endl;
        return false;
     }
    
    this->queueFamilyIndex = family_index;
    // }}
    
    float queue_priorities[] { 1.0f };
    VkDeviceQueueCreateInfo deviceQueueCreateInfo {}; 
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.queueFamilyIndex = this->queueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = queue_priorities;
    
    VkDeviceCreateInfo deviceCreateInfo {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    
    result = vkCreateDevice(this->gpu, &deviceCreateInfo, NULL, &device);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create logical device: " << getVulkanErrorString(result) << std::endl;
        return false;
     }
    
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
    
    return true;
}

bool Renderer::initSurface(GLFWwindow* window) {
    VkResult result = glfwCreateWindowSurface(instance, window, NULL, &surface);
    
    if (result != VK_SUCCESS) {
        std::cout << "Failed to init surface: " << getVulkanErrorString(result) << std::endl;
        return false;
    }    
    
    VkBool32 wsiSupported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queueFamilyIndex, surface, &wsiSupported);
    if (!wsiSupported) {
        std::cout << "Failed to init surface: " << "WSI is not supported" << std::endl;
        return false;
    }
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceCapabilities);
    
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, NULL);
    if (formatCount == 0) {
        std::cout << "Failed to init surface: " << "Surface formats missing." << std::endl;
        return false;
    }
    
    std::vector<VkSurfaceFormatKHR> formats (formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, formats.data());
    if (formats[0].format == VK_FORMAT_UNDEFINED) {
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    } else {
        surfaceFormat = formats[0];
    }
    
    return true;
}

bool Renderer::initCommands() {
    VkResult result;
    
    // Create fence
    VkFenceCreateInfo fenceCreateInfo {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    
    result = vkCreateFence(device, &fenceCreateInfo, NULL, &fence); 
    if (result != VK_SUCCESS) return false;

    // Create command pool
    VkCommandPoolCreateInfo poolCreateInfo {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.queueFamilyIndex = this->queueFamilyIndex ;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    result = vkCreateCommandPool(device, &poolCreateInfo, NULL, &commandPool);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create command pool: " << getVulkanErrorString(result) << std::endl;
        return false;
     }
     
    // Create command buffer
    VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    
    result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create command buffer: " << getVulkanErrorString(result) << std::endl;
        return false;
    }
     
    return true;
}

bool Renderer::initSwapchain() {
    VkResult result;

    if (swapchainImageCount > surfaceCapabilities.maxImageCount) 
        swapchainImageCount = surfaceCapabilities.maxImageCount;
        
    if (swapchainImageCount < surfaceCapabilities.minImageCount + 1) 
        swapchainImageCount = surfaceCapabilities.minImageCount + 1;
    
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    
    uint32_t presentModesCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModesCount, NULL);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create swapchain: " << getVulkanErrorString(result) << std::endl;
        return false;
    }
    
    std::vector<VkPresentModeKHR> presentModes(presentModesCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModesCount, presentModes.data());
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create swapchain: " << getVulkanErrorString(result) << std::endl;
        return false;
    }
    
    for(VkPresentModeKHR mode: presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }
    
    VkSwapchainCreateInfoKHR swapchainCreateInfo {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = swapchainImageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent= surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = NULL;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    
    result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create swapchain: " << getVulkanErrorString(result) << std::endl;
        return false;
    }
    
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL);
    return true;
}

bool Renderer::initSwapchainImages() {
    VkResult result;

    swapchainImages.resize(swapchainImageCount);
    swapchainImageViews.resize(swapchainImageCount);
    
    result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create swapchain image view: " << getVulkanErrorString(result) << std::endl;
        return false;
    }
    
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        VkImageViewCreateInfo imageViewCreateInfo {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = surfaceFormat.format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        
        result = vkCreateImageView(device, &imageViewCreateInfo, NULL, &swapchainImageViews[i]);
        if (result != VK_SUCCESS) {
            std::cout << "Failed to create swapchain image view[" <<  i << "]: " << getVulkanErrorString(result) << std::endl;
            return false;
        }
        

    }
    
    return true;
}

// Destroy
Renderer::~Renderer(){
    destroyCommands();
    destroySwapchainImages();
    destroySwapchain();
    destroySurface();
    destroyDevice();
    destroyInstance();  
    
}

void Renderer::destroyInstance() {
    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, NULL);
        std::cout << "Instance deleted" << std::endl;
    }
}

void Renderer::destroyDevice() {
    if (device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, NULL);
        std::cout << "Device deleted" << std::endl;
    }
}

void Renderer::destroyCommands() {
    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, commandPool, NULL);
        std::cout << "Command pool deleted" << std::endl;
    }
    
    if (fence != VK_NULL_HANDLE) {
        vkDestroyFence(device, fence, NULL);        
        std::cout << "Fence deleted" << std::endl;
    }
}

void Renderer::destroySurface() {
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, NULL);
        std::cout << "Surface deleted" << std::endl;
    }
}

void Renderer::destroySwapchain() {
    if (swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain, NULL);
        std::cout << "Swapchain deleted" << std::endl;
    }
}

void Renderer::destroySwapchainImages() {
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        if (swapchainImageViews[i] != VK_NULL_HANDLE) {
            vkDestroyImageView(device, swapchainImageViews[i], NULL);
            std::cout << "ImageView" << i <<" deleted" << std::endl;
        }
    }
    
    std::cout << "All ImageViews deleted" << std::endl;
}
