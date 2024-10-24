#pragma once

#include "Core.h"
#include "Quad.h"
#include "BloomShader.h"

class Bloom {
private:
	GLuint FBO[2] = { 0, 0 };
	GLuint colorBuffers[2] = { 0, 0 };
	bool blurFlag = true;

	BloomShader shader;
	Quad quad;

public:
	Bloom();

	inline GLuint getFrameBufferID(int index) const { return this->FBO[index]; }
	inline GLuint* getColorBuffers() { return this->colorBuffers; }
	inline bool getBlurFlag() const { return this->blurFlag; }

	void _init(int windowWidth, int windowHeight);
	void processFramebuffer(int iterations, GLuint colorBufferID, GLuint frameBufferID = 0);

	~Bloom() {
		if (this->FBO[0] != 0 || this->FBO[1] != 0)
			glDeleteFramebuffers(2, this->FBO);

		if (this->colorBuffers[0] != 0 || this->colorBuffers[1] != 0)
			glDeleteTextures(2, this->colorBuffers);
	}
};

