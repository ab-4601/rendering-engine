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
	GLuint colorBuffer = 0;
	GLuint depthBuffer = 0;

	GLuint intermediateFBO = 0;
	GLuint screenBuffer = 0;

	HDRShader shader;
	Quad quad;

	void _initIntermediateFBO(int width, int height);

public:
	HDR();

	void _init(int width, int height);
	void _initMSAA(int width, int height);

	inline GLuint getFramebufferID() const { return this->FBO; }
	inline GLuint getColorbufferID() const { return this->colorBuffer; }

	inline void enableHDRWriting() const { glBindFramebuffer(GL_FRAMEBUFFER, this->FBO); }

	void renderToDefaultBuffer(float exposure = 1.f, GLuint bloomBuffer = 0, bool enableBloom = false);
	void renderToDefaultBufferMSAA(
		float exposure = 1.f, GLuint bloomBuffer = 0, int windowWidth = 800,
		int windowHeight = 800, bool enableBloom = false
	);

	~HDR();
};