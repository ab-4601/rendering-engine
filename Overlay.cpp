#include "Overlay.h"
#include <imgui_internal.h>

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
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.f, 0.9f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.f);

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

void Overlay::DrawVec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());

    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, 4.f));

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
    ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.05f, 0.05f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.f));
    if (ImGui::Button("X", buttonSize))
        values[0] = resetValue;

    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##x", &values[0], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 1.f, 0.05f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.9f, 0.2f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.8f, 0.1f, 1.f));
    if(ImGui::Button("Y", buttonSize))
        values[1] = resetValue;
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##y", &values[1], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.05f, 1.f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.9f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.15f, 0.8f, 1.f));
    if(ImGui::Button("Z", buttonSize))
        values[2] = resetValue;

    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##z", &values[2], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    ImGui::Columns(1);
    ImGui::PopID();
}

void Overlay::renderGUIWindow(ImGuiIO& io, float& exposure, bool& drawSkybox,bool& enableBloom, 
    bool& enableWireframe, bool& enableShadows, glm::vec3& dirLightLocation, Mesh* currMesh) 
{
    ImGui::PushFont(this->mainfont);
    ImGui::Begin("ImGui window");
    ImGui::Text("Scene Information and modifiers");
    ImGui::Checkbox("Render Skybox", &drawSkybox);
    ImGui::Checkbox("Bloom", &enableBloom);
    ImGui::Checkbox("Wireframe", &enableWireframe);
    ImGui::Checkbox("Shadows", &enableShadows);
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 120.f);
    ImGui::Text("Mesh Color");
    ImGui::NextColumn();
    ImGui::ColorEdit4("##color", this->colorBuffer);
    ImGui::Columns(1);

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 120.f);
    ImGui::Text("Exposure");
    ImGui::NextColumn();
    ImGui::DragFloat("##exposure", &exposure, 0.01f, 0.f, 0.f, "%.2f");
    ImGui::Columns(1);

    ImGui::NewLine();
    ImGui::Text("Transforms");
    ImGui::NewLine();

    this->DrawVec3Control("Translation", this->translation);
    this->DrawVec3Control("Rotation", this->rotation);
    this->DrawVec3Control("Scale", this->scale, 1.f);

    ImGui::NewLine();
    this->DrawVec3Control("SkyLight", glm::value_ptr(dirLightLocation));
    ImGui::NewLine();
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::PopFont();
    ImGui::End();

    if (currMesh != nullptr) {
        glm::mat4 model = glm::mat4(1.f);
        
        ImGuizmo::RecomposeMatrixFromComponents(
            this->translation,
            this->rotation,
            this->scale,
            glm::value_ptr(model)
        );

        currMesh->setModelMatrix(model);

        glm::vec3 color{ this->colorBuffer[0], this->colorBuffer[1], this->colorBuffer[2] };

        currMesh->setColor(color);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Overlay::renderTransformWidget(int windowWidth, int windowHeight, glm::mat4& projection, glm::mat4& view, 
    Mesh* mesh)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(true);
    ImGuizmo::AllowAxisFlip(false);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, windowWidth, windowHeight);
    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
        transformOperation, ImGuizmo::WORLD, glm::value_ptr(mesh->getModelMatrix()));
    ImGuizmo::DecomposeMatrixToComponents(
        glm::value_ptr(mesh->getModelMatrix()), this->translation, this->rotation, this->scale
    );

    this->colorBuffer[0] = mesh->getColor().x;
    this->colorBuffer[1] = mesh->getColor().y;
    this->colorBuffer[2] = mesh->getColor().z;
}

Overlay::~Overlay() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}