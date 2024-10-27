#include "Bloom.h"

Bloom::Bloom(bool isAntiAliased) : aaFlag{ isAntiAliased } {
	this->quad.createQuad();
}

void Bloom::_init(int width, int height) {
	glGenFramebuffers(2, this->FBO);
	glGenTextures(2, this->colorBuffers);

	for (uint i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[i]);

		glBindTexture(GL_TEXTURE_2D, this->colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
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

	if (this->aaFlag) {
		this->_initIntermediateFBO(width, height);
	}
}

void Bloom::_initIntermediateFBO(int width, int height) {
	glGenFramebuffers(1, &this->intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->intermediateFBO);

	glGenTextures(1, &this->screenBuffer);

	glBindTexture(GL_TEXTURE_2D, this->screenBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->screenBuffer, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Intermediate framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::processFramebuffer(int iterations, GLuint colorBufferID, GLuint frameBufferID) {
	bool flipFlag = true;
	
	glUseProgram(this->shader.getProgramID());

	glActiveTexture(GL_TEXTURE0);

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

void Bloom::processFramebufferMSAA(int iterations, GLuint colorBufferID, GLuint frameBufferID, int width, int height) {
	bool flipFlag = true;

	glUseProgram(this->shader.getProgramID());

	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->intermediateFBO);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glActiveTexture(GL_TEXTURE0);

	for (int i = 0; i < iterations; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[this->blurFlag]);

		glUniform1i(this->shader.getUniformHorizontalFlag(), this->blurFlag);

		glBindTexture(GL_TEXTURE_2D, flipFlag ? this->screenBuffer : this->colorBuffers[!this->blurFlag]);

		this->quad.renderQuad();

		this->blurFlag = !this->blurFlag;
		if (flipFlag)
			flipFlag = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}