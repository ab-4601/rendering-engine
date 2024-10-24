#pragma once

#include "Core.h"
#include "HDRShader.h"
#include "Quad.h"

class HDR {
private:
	GLuint FBO = 0;
	GLuint colorBuffers[2] = { 0, 0 };
	GLuint depthBuffer = 0;
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	HDRShader shader;
	Quad quad;

public:
	HDR();

	void _init(int windowWidth, int windowHeight);

	inline GLuint getFramebufferID() const { return this->FBO; }
	inline GLuint getColorbufferID(int index) const { return this->colorBuffers[index]; }

	inline void enableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, this->FBO); }
	inline void disableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void renderToDefaultBuffer(float exposure = 1.f, const GLuint* bloomBuffer = nullptr, bool id = false);

	~HDR();
};

