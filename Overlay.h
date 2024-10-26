#pragma once

#include "Core.h"
#include "Window.h"

class Overlay {
private:
	ImGuizmo::OPERATION transformOperation;
	ImFont* mainfont;

	float colorBuffer[3] = { 0.f, 0.f, 0.f };
	float translation[3] = { 0.f, 0.f, 0.f };
	float rotation[3] = { 0.f, 0.f, 0.f };
	float scale[3] = { 0.f, 0.f, 0.f };

public:
	Overlay() : transformOperation{ ImGuizmo::TRANSLATE }, mainfont{ nullptr } {};

	ImGuiIO& _init(GLFWwindow* window);

	void _newFrame();
	void _updateTransformOperation(const Window& window);
	void renderGUIWindow(ImGuiIO& io, bool& drawSkybox);
	void renderTransformWidget(int bufferWidth, int bufferHeight, glm::mat4& projection, glm::mat4& view, glm::mat4& model);

	~Overlay();
};

