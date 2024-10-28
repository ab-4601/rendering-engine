#pragma once

#include "Core.h"
#include "HDRShader.h"
#include "Quad.h"

#pragma once

#include "Core.h"
#include "HDRShader.h"
#include "Quad.h"

class HDR {
private:
	GLuint FBO = 0;
	GLuint colorBuffers[2] = { 0, 0 };
	GLuint depthBuffer = 0;

	GLuint intermediateFBO = 0;
	GLuint screenBuffer = 0;

	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	HDRShader shader;
	Quad quad;

	void _initIntermediateFBO(int width, int height);

public:
	HDR();

	void _init(int width, int height);
	void _initMSAA(int width, int height);

	inline GLuint getFramebufferID() const { return this->FBO; }
	inline GLuint getColorbufferID(int index) const { return this->colorBuffers[index]; }

	inline void enableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, this->FBO); }
	inline void disableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	inline void setDrawBuffers() const { glDrawBuffers(2, this->attachments); }

	void renderToDefaultBuffer(float exposure = 1.f, const GLuint* bloomBuffer = nullptr, bool id = false,
		bool enableBloom = false);
	void renderToDefaultBufferMSAA(
		float exposure = 1.f, const GLuint* bloomBuffer = nullptr, bool id = false, int width = 0, int height = 0,
		bool enableBloom = false
	);

	~HDR();
};