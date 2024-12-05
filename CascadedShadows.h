#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

class CascadedShadows {
private:
	static const int mapResolution = 2048;
	static const uint numCascades = 4;

	GLuint FBO{ 0 }, UBO{ 0 };
	GLuint shadowMaps{ 0 }, randomOffset{ 0 };

	std::vector<float> cascadeSplits;
	std::vector<float> offsetData;
	std::vector<glm::mat4> lightSpaceMatrices;
	std::vector<glm::vec4> frustumCorners;

	Shader shader{ "CSM.vert", "CSM.frag", "CSM.geom" };

	float jitter(float x, float y) {
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> randomFloat(x, y);

		return randomFloat(generator);
	}

	bool checkFramebufferStatus();
	void calcSplitDepths(float lambda);
	void calcFrustumCorners(glm::mat4 projection, glm::mat4 view);
	void genRandomOffsetData(int windowSize, int filterSize);
	void genRandomOffsetTexture(int windowSize, int filterSize);

	void calcLightSpaceMatrix(const Camera& camera, glm::vec3 lightDir, int windowWidth, int windowHeight,
		const float& nearPlane, const float& farPlane);

	void calcLightSpaceMatrices(const Camera& camera, glm::vec3 lightDir, int windowWidth, int windowHeight);

public:
	CascadedShadows(float lambda = 0.5f, int windowSize = 4, int filterSize = 4);

	void _init();

	inline GLuint getNumCascades() const { return this->numCascades; }
	inline GLuint csmDepthBuffer() const { return this->shadowMaps; }
	inline std::vector<float> cascadePlanes() const { return this->cascadeSplits; }

	void calculateShadows(const Camera& camera, int windowWidth, int windowHeight, std::vector<Mesh*>& meshes, 
		glm::vec3 lightPosition, GLuint currFramebuffer = 0);

	~CascadedShadows();
};

