#pragma once

#include "Core.h"
#include "Quad.h"
#include "BloomShader.h"

class Bloom {
private:
	GLuint FBO[2] = { 0, 0 };
	GLuint colorBuffers[2] = { 0, 0 };

	GLuint intermediateFBO = 0;
	GLuint screenBuffer = 0;

	bool blurFlag = true;
	bool aaFlag = false;

	BloomShader shader;
	Quad quad;

	void _initIntermediateFBO(int width, int height);

public:
	Bloom(bool isAntiAliased = false);

	inline GLuint getFrameBufferID(int index) const { return this->FBO[index]; }
	inline GLuint* getColorBuffers() { return this->colorBuffers; }
	inline bool getBlurFlag() const { return this->blurFlag; }

	void _init(int width, int height);

	void processFramebuffer(int iterations, GLuint colorBufferID, GLuint frameBufferID = 0);
	void processFramebufferMSAA(
		int iterations, GLuint colorBufferID, GLuint frameBufferID = 0, int width = 0, int height = 0
	);

	~Bloom() {
		if (this->FBO[0] != 0 || this->FBO[1] != 0)
			glDeleteFramebuffers(2, this->FBO);

		if (this->colorBuffers[0] != 0 || this->colorBuffers[1] != 0)
			glDeleteTextures(2, this->colorBuffers);
	}
};

