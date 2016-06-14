#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Renderer.hpp"

namespace App {
    GLFWwindow* window;
    
    Renderer* renderer;
    
    void init(int width, int height) {
        if (!glfwInit()) exit(1);
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(width, height, "test", NULL, NULL);
        
        renderer = new Renderer(window);
    }

    void destroy() {
        delete(renderer);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void start() {

        while (!glfwWindowShouldClose(window)) {
            renderer->update();
            
            renderer->waitReady();
        }
    }
};

int main() {
    App::init(640, 480);
    App::start();
    App::destroy();
}
