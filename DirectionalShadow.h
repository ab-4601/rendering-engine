#pragma once

#include "Core.h"
#include "Mesh.h"
#include "DirectionalShadowShader.h"

class DirectionalShadow {
private:
	static const int SHADOW_WIDTH = 3840, SHADOW_HEIGHT = 2160;

	GLuint FBO = 0;
	GLuint depthMap = 0;

	float nearPlane = 1.f;
	float farPlane = 25.f;

	glm::mat4 projection = glm::mat4(1.f);
	glm::mat4 view = glm::mat4(1.f);

	DirectionalShadowShader shader{};

public:
	DirectionalShadow(float nearPlane = 1.f, float farPlane = 25.f);

	void _init();
	void calculateShadows(int windowWidth, int windowHeight, std::vector<Mesh*>& meshes, 
		glm::vec3 lightPosition, GLuint currentFramebuffer = 0);

	inline glm::mat4 getLightSpaceMatrix() const { return this->projection * this->view; };
	inline GLuint getDirectionalShadowMap() const { return this->depthMap; }

	~DirectionalShadow();
};

