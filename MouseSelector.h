#pragma once

#include "Window.h"
#include "Mesh.h"
#include "SelectionShader.h"
#include "SelectionTexture.h"

class MouseSelector {
private:
	SelectionShader shader;
	SelectionTexture pixelInfo;

public:
	MouseSelector(unsigned int windowWidth, unsigned int windowHeight);

	void pickingPhase(std::vector<Mesh*>& meshes, const glm::mat4& projection, const glm::mat4& view);
	int mouseSelectionResult(int windowHeight, int x, int y);

	~MouseSelector() = default;
};