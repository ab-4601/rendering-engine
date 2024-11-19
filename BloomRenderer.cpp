#include "BloomRenderer.h"

BloomRenderer::BloomRenderer(int windowWidth, int windowHeight) {
	quad.createQuad();

	this->_init(windowWidth, windowHeight);
}

void BloomRenderer::renderDownsamples(GLuint srcTexture) {
	const std::vector<BloomMip>& mipChain = this->mFBO.mipChain();

	glUseProgram(this->downsampleShader.getProgramID());

	glUniform1i(this->downsampleShader.getUniformSrcTexture(), 0);
	glUniform2fv(this->downsampleShader.getUniformSrcResolution(), 1, glm::value_ptr(this->srcViewportSizeFLT));
	glUniform1i(this->downsampleShader.getUniformMipLevel(), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	for (size_t i = 0; i < mipChain.size(); i++) {
		const BloomMip& mip = mipChain[i];
		
		glViewport(0, 0, mip.size.x, mip.size.y);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0
		);

		quad.renderQuad();

		glUniform2fv(this->downsampleShader.getUniformSrcResolution(), 1, glm::value_ptr(mip.size));

		glBindTexture(GL_TEXTURE_2D, mip.texture);
	}
}

void BloomRenderer::renderUpsamples(float filterRadius) {
	const std::vector<BloomMip>& mipChain = this->mFBO.mipChain();

	glUseProgram(this->upsampleShader.getProgramID());
	glUniform1i(this->upsampleShader.getUniformSrcTexture(), 0);
	glUniform1f(this->upsampleShader.getUniformFilterRadius(), filterRadius);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	for (int i = (int)mipChain.size() - 1; i > 0; i--) {
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i - 1];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		glViewport(0, 0, nextMip.size.x, nextMip.size.y);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0
		);

		quad.renderQuad();
	}

	glDisable(GL_BLEND);
}

void BloomRenderer::_initIntermediateFBO(int width, int height) {
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

bool BloomRenderer::_init(int windowWidth, int windowHeight)
{
	if (this->isInit) return true;

	this->srcViewportSize = glm::ivec2(windowWidth, windowHeight);
	this->srcViewportSizeFLT = glm::vec2((float)windowWidth, (float)windowHeight);

	const uint iterations = 6;
	bool status = mFBO._init(windowWidth, windowHeight, iterations);

	if (!status) {
		exit(-1);
	}

	this->_initIntermediateFBO(windowWidth, windowHeight);

	this->isInit = true;
	return true;
}

void BloomRenderer::renderBloomTexture(GLuint srcTexture, float filterRadius, GLuint currFramebuffer) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, currFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->intermediateFBO);
	glBlitFramebuffer(
		0, 0, this->srcViewportSize.x, this->srcViewportSize.y,
		0, 0, this->srcViewportSize.x, this->srcViewportSize.y,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);

	this->mFBO.enableWriting();

	this->renderDownsamples(this->screenBuffer);
	this->renderUpsamples(filterRadius);

	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);

	glViewport(0, 0, this->srcViewportSize.x, this->srcViewportSize.y);
}