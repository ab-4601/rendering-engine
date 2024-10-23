#pragma once

#include "Core.h"
#include "HDRShader.h"

struct Quad {
	float vertices[20] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	GLuint VAO{ 0 }, VBO{ 0 };

	Quad() = default;

	void createQuad() {
		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), &this->vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void renderQuad() const {
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

class HDR {
private:
	GLuint FBO = 0;
	GLuint colorBuffer = 0;
	GLuint depthBuffer = 0;

	HDRShader shader;
	Quad quad;

public:
	HDR();

	void _init(int windowWidth, int windowHeight);

	inline GLuint getFramebufferID() const { return this->FBO; }
	inline void enableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, this->FBO); }
	inline void disableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void renderToDefaultBuffer();

	~HDR();
};

