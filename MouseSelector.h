#pragma once

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "SelectionTexture.h"

class MouseSelector {
private:
	Shader shader{ "selection.vert", "selection.frag" };
	SelectionTexture pixelInfo;

public:
	MouseSelector(unsigned int windowWidth, unsigned int windowHeight);

	void pickingPhase(std::vector<Mesh*>& meshes, const glm::mat4& projection,
		const glm::mat4& view, GLuint framebuffer = 0);
	int mouseSelectionResult(int windowHeight, int x, int y);

	~MouseSelector() = default;
};