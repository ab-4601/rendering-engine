#include "Overlay.h"

ImGuiIO& Overlay::_init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    io.Fonts->AddFontDefault();
    mainfont = io.Fonts->AddFontFromFileTTF("Fonts/JetBrainsMonoNL-ExtraLight.ttf",
        18.5f, NULL);
    IM_ASSERT(this->mainfont != NULL);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 1.f;
    style.WindowRounding = 8.f;
    style.Colors[ImGuiCol_Border] = ImVec4(253.f / 255.f, 106.f / 255.f, 0.f / 255.f, 1.f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(253.f / 255.f, 106.f / 255.f, 0.f / 255.f, 1.f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 0.8f);

    return io;
}

void Overlay::_newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::GetMainViewport();
}

void Overlay::_updateTransformOperation(const Window& window) {
    if (window.getKeyPress(GLFW_KEY_1))
        transformOperation = ImGuizmo::TRANSLATE;

    else if (window.getKeyPress(GLFW_KEY_2)) 
        transformOperation = ImGuizmo::ROTATE;

    else if (window.getKeyPress(GLFW_KEY_3))
        transformOperation = ImGuizmo::SCALE;
}

void Overlay::renderGUIWindow(ImGuiIO& io, bool& drawSkybox) {
    ImGui::PushFont(this->mainfont);
    ImGui::Begin("ImGui window");
    ImGui::Text("Scene Information and modifiers");
    ImGui::Checkbox("Render Skybox", &drawSkybox);
    ImGui::ColorEdit3("Mesh Color", this->colorBuffer);
    ImGui::NewLine();
    ImGui::LabelText("", "Transform");
    ImGui::SliderFloat3("Translation", this->translation, -100.f, 100.f);
    ImGui::SliderFloat3("Rotation", this->rotation, -360.f, 360.f);
    ImGui::SliderFloat3("Scale", this->scale, 0.f, 100.f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::PopFont();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Overlay::renderTransformWidget(int windowWidth, int windowHeight, glm::mat4& projection, glm::mat4& view, 
    glm::mat4& model)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(true);
    ImGuizmo::AllowAxisFlip(false);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, windowWidth, windowHeight);
    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
        transformOperation, ImGuizmo::WORLD, glm::value_ptr(model));
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), this->translation, this->rotation, this->scale);
}

Overlay::~Overlay() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}