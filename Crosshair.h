#pragma once

#include "Core.h"
#include "Camera.h"
#include "Texture.h"
#include "CrosshairShader.h"

class Crosshair {
private:
	GLuint VAO, VBO;

	GLfloat vertices[12] {
		-1.f,  1.f,  0.f,
		-1.f, -1.f,  0.f,
		 1.f,  1.f,  0.f,
		 1.f, -1.f,  0.f
	};

	CrosshairShader crosshairShader;
	Texture crosshairTexture{ "Textures/crosshair.png" };

public:
	Crosshair();

	void drawCrosshair();

	void clearMesh();

	~Crosshair() {
		this->clearMesh();
	}
};