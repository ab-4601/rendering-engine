#include "HDR.h"

HDR::HDR() {
	this->quad.createQuad();
}

void HDR::_init(int windowWidth, int windowHeight) {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(2, this->colorBuffers);

	for (uint i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, this->colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->colorBuffers[i], 0);
	}

	glGenRenderbuffers(1, &this->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
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

void HDR::renderToDefaultBuffer(float exposure, const GLuint* bloomBuffer, bool id) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(this->shader.getProgramID());

	glUniform1f(this->shader.getUniformExposure(), exposure);

	glUniform1i(glGetUniformLocation(this->shader.getProgramID(), "hdrBuffer"), 0);
	glUniform1i(glGetUniformLocation(this->shader.getProgramID(), "bloomBuffer"), 1);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->colorBuffers[0]);

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

	if (this->colorBuffers[0] != 0 || this->colorBuffers[1] != 0)
		glDeleteTextures(2, this->colorBuffers);

	if (this->depthBuffer != 0)
		glDeleteRenderbuffers(1, &this->depthBuffer);
}