#pragma once

#include "Core.h"
#include "GridShader.h"

class Grid {
private:
	GLfloat vertices[12]{
		-1.f,  0.f, -1.f,
		 1.f,  0.f, -1.f,
		 1.f,  0.f,  1.f,
		-1.f,  0.f,  1.f
	};

	unsigned int indices[6]{
		0, 2, 1,
		2, 0, 3
	};

	GLuint VAO, VBO, IBO;
	GridShader shader;

public:
	Grid();

	void renderGrid(glm::mat4& model, const glm::mat4& projection, const glm::mat4& view, glm::vec3 camPos);

	~Grid();
};

