#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Shader.h"

class CascadedShadows {
private:
	static const int shadowWidth = 1024, shadowHeight = 1024;
	static const uint numCascades = 3;

	GLuint FBO[numCascades];
	GLuint shadowMaps[numCascades];

	float cascadeSplits[numCascades];
	glm::mat4 lightSpaceMatrices[numCascades];

	Shader shader;

	bool checkFramebufferStatus();
	void calcSplitDepths(float lambda);

public:
	CascadedShadows(float lambda = 0.5f);

	void _init();

	std::vector<glm::vec3> calcFrustumCorners(const glm::mat4& view, const glm::mat4& projection);
	void calcLightSpaceBoundingBox(
		const std::vector<glm::vec3>& frustumCorners, glm::vec3& lightDirection,glm::vec3& outMin, glm::vec3& outMax
	);
	void calcLightSpaceMatrices(const glm::vec3& lightDirection, const glm::mat4& view, const glm::mat4& projection);

	void calculateShadows(int windowWidth, int windowHeight, std::vector<Mesh*>& meshes, glm::vec3 lightPosition,
		GLuint currentFramebuffer = 0);

	~CascadedShadows();
};

