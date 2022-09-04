#include <stdexcept>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "client.hpp"

class glfw_context {
private:
    static inline glfw_context * current{};
public:
    glfw_context() {
        if(current)
            throw std::logic_error{"double context-creation"};
        if(!glfwInit())
            throw std::runtime_error{"glfwInit failed"};
        current = this;
    }

    ~glfw_context() {
        glfwTerminate();
        current = nullptr;
    }

    static bool exists() noexcept { return current; }
};

class imgui_context {
private:
    static inline imgui_context * current{};
public:
    imgui_context(char const * title, int width, int height) {
        if(!glfw_context::exists())
            throw std::logic_error{"no glfw-context"};
        if(current)
            throw std::logic_error{"double context-creation"};
        window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
        glfwMakeContextCurrent(window_);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init();
        current = this;
    }

    ~imgui_context() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        current = nullptr;
    }

    static void loop(auto f) {
        while(!glfwWindowShouldClose(current->window_)) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            f(current->window_);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(current->window_);
            glfwPollEvents();
        }
    }

    static bool exists() noexcept { return current; }
private:
    GLFWwindow * window_;
};

int main() {
    glfw_context glfw_ctx;
    imgui_context ig_ctx{"Template-Titel", 1280, 720};
    ImGui::GetIO().IniFilename = nullptr;
    glClearColor(.4f, .6f, 1.f, 1.f);
    ig_ctx.loop([](auto) {
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::ShowDemoWindow();
    });
}
