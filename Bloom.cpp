#include "Bloom.h"

Bloom::Bloom() {
	this->quad.createQuad();
}

void Bloom::_init(int windowWidth, int windowHeight) {
	glGenFramebuffers(2, this->FBO);
	glGenTextures(2, this->colorBuffers);

	for (uint i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[i]);

		glBindTexture(GL_TEXTURE_2D, this->colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffers[i], 0);
	}

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Bloom framebuffer error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::processFramebuffer(int iterations, GLuint colorBufferID, GLuint frameBufferID) {
	bool flipFlag = true;
	
	glUseProgram(this->shader.getProgramID());

	for (int i = 0; i < iterations; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[this->blurFlag]);
		
		glUniform1i(this->shader.getUniformHorizontalFlag(), this->blurFlag);

		glBindTexture(GL_TEXTURE_2D, flipFlag ? colorBufferID : this->colorBuffers[!this->blurFlag]);
		this->quad.renderQuad();

		this->blurFlag = !this->blurFlag;
		if (flipFlag)
			flipFlag = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}