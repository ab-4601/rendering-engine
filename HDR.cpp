#include "HDR.h"

HDR::HDR() {
	this->quad.createQuad();
}

void HDR::_init(int width, int height) {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(2, this->colorBuffers);

	for (uint i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, this->colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->colorBuffers[i], 0);
	}

	glGenRenderbuffers(1, &this->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthBuffer);

	glDrawBuffers(2, this->attachments);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "HDR framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDR::_initMSAA(int width, int height) {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(2, this->colorBuffers);

	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->colorBuffers[i]);

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, width, height, GL_TRUE);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, this->colorBuffers[i], 0
		);
	}

	glGenRenderbuffers(1, &this->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthBuffer);

	glDrawBuffers(2, this->attachments);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "HDR MSAA Framebuffer initialization error" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->_initIntermediateFBO(width, height);
}

void HDR::_initIntermediateFBO(int width, int height) {
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

void HDR::renderToDefaultBuffer(float exposure, const GLuint* bloomBuffer, bool id) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(this->shader.getProgramID());

	glUniform1f(this->shader.getUniformExposure(), exposure);

	glUniform1i(this->shader.getUniformHDRbuffer(), 0);
	glUniform1i(this->shader.getUniformBloomBuffer(), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->colorBuffers[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomBuffer[!id]);

	this->quad.renderQuad();

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void HDR::renderToDefaultBufferMSAA(float exposure, const GLuint* bloomBuffer, bool id, int width, int height) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->intermediateFBO);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(this->shader.getProgramID());

	glUniform1i(this->shader.getUniformHDRbuffer(), 0);
	glUniform1i(this->shader.getUniformBloomBuffer(), 1);

	glUniform1f(this->shader.getUniformExposure(), exposure);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->screenBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomBuffer[!id]);

	this->quad.renderQuad();

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

HDR::~HDR() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->intermediateFBO != 0)
		glDeleteFramebuffers(1, &this->intermediateFBO);

	if (this->screenBuffer != 0)
		glDeleteTextures(1, &this->screenBuffer);

	if (this->colorBuffers[0] != 0 || this->colorBuffers[1] != 0)
		glDeleteTextures(2, this->colorBuffers);

	if (this->depthBuffer != 0)
		glDeleteRenderbuffers(1, &this->depthBuffer);
}