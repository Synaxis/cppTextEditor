#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <fstream>
#include <sstream>

static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void SaveTextToFile(const char* text, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << text;
        file.close();
    } else {
        std::cerr << "Failed to save file: " << filename << std::endl;
    }
}

bool LoadTextFromFile(const std::string& filename, char* text, size_t bufferSize) {
    std::ifstream file(filename);
    if (file.is_open()) {
        file.read(text, bufferSize);
        text[file.gcount()] = '\0'; // Ensure null-termination
        file.close();
        return true;
    } else {
        std::cerr << "Failed to load file: " << filename << std::endl;
        return false;
    }
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return 1;
    }

    const char* glsl_version = "#version 150"; // Match GLSL version to OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); // Specify at least OpenGL version 3.2
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); // Specify at least OpenGL version 3.2
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Simple Text Editor", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static char textBuffer[1024 * 16] = ""; // 16KB text buffer, increase if you need more space

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Handle window resize
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Here you can do all your ImGui rendering
        ImGui::SetNextWindowSize(ImVec2(display_w, display_h)); // Set the next window size to cover the entire framebuffer
        ImGui::SetNextWindowPos(ImVec2(0, 0)); // Set the next window position to the top-left corner
        ImGui::Begin("Text Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::InputTextMultiline("##Editor", textBuffer, sizeof(textBuffer), ImVec2(-FLT_MIN, display_h - 40)); // Adjust height for the buttons
        if (ImGui::Button("Save")) {
            SaveTextToFile(textBuffer, "output.txt");
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            if (!LoadTextFromFile("output.txt", textBuffer, sizeof(textBuffer))) {
                memset(textBuffer, 0, sizeof(textBuffer));
            }
        }
        ImGui::End();

        // End of ImGui rendering before rendering to the screen
        ImGui::Render();
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
