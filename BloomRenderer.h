#pragma once

#include "BloomFBO.h"
#include "PhysBloomShader.h"
#include "Quad.h"

class BloomRenderer {
private:
	bool isInit = false;
	BloomFBO mFBO;

	GLuint intermediateFBO{ 0 }, screenBuffer{ 0 };

	glm::ivec2 srcViewportSize{ 0 };
	glm::vec2 srcViewportSizeFLT{ 0.f };

	PhysBloomShader upsampleShader{ "bloom.vert", "upsample.frag", true };
	PhysBloomShader downsampleShader{ "bloom.vert", "downsample.frag" };

	void renderDownsamples(GLuint srcTexture);
	void renderUpsamples(float filterRadius);

	void _initIntermediateFBO(int width, int height);

	Quad quad;
	
public:
	BloomRenderer(int windowWidth, int windowHeight);

	bool _init(int windowWidth, int windowHeight);
	void renderBloomTexture(GLuint srcTexture, float filterRadius, GLuint currFramebuffer = 0);

	GLuint bloomTexture() {
		return this->mFBO.mipChain()[0].texture;
	}

	~BloomRenderer() = default;
};

