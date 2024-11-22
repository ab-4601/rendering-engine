#include "CascadedShadows.h"

CascadedShadows::CascadedShadows(float lambda) {
	for (int i = 0; i < this->numCascades; i++) {
		this->FBO[i] = 0;
		this->shadowMaps[i] = 0;
		this->lightSpaceMatrices[i] = glm::mat4(1.f);
	}

	this->calcSplitDepths(lambda);
}

bool CascadedShadows::checkFramebufferStatus() {
	uint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void CascadedShadows::calcSplitDepths(float lambda) {
	float range = ::near_plane - ::far_plane;
	float logFactor = std::log(::far_plane / ::near_plane);

	for (int i = 0; i < this->numCascades; i++) {
		float p = (float)(i + 1) / this->numCascades;

		float linearSplit = ::near_plane + range * p;

		float logSplit = ::near_plane * std::exp(logFactor * p);

		this->cascadeSplits[i] = lambda * logSplit + (1.f - lambda) * linearSplit;
	}
}

void CascadedShadows::_init() {
	glGenFramebuffers(this->numCascades, this->FBO);
	glGenTextures(this->numCascades, this->shadowMaps);

	for (int i = 0; i < this->numCascades; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[i]);
		glBindTexture(GL_TEXTURE_2D, this->shadowMaps[i]);

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->shadowWidth,
			this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowMaps[i], 0);
		glReadBuffer(GL_NONE);
		glDrawBuffer(GL_NONE);

		if (!this->checkFramebufferStatus()) {
			std::cerr << "Error initializing cascaded shadow map framebuffers" << std::endl;
			exit(-1);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::vector<glm::vec3> CascadedShadows::calcFrustumCorners(const glm::mat4& view, const glm::mat4& projection) {
	std::vector<glm::vec3> res{};
	
	glm::vec4 frustumCornersNDC[8] = {
		glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f),
		glm::vec4(1.0f,  1.0f, -1.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
		glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
		glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f),
		glm::vec4(1.0f,  1.0f,  1.0f, 1.0f),
		glm::vec4(1.0f, -1.0f,  1.0f, 1.0f),
		glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f)
	};

	glm::mat4 inverseProjectionView = glm::inverse(projection * view);

	for (int i = 0; i < 8; i++) {
		glm::vec4 cornerWorldSpace = inverseProjectionView * frustumCornersNDC[i];
		cornerWorldSpace /= cornerWorldSpace.w;

		glm::vec3 cornerWS = glm::vec3(cornerWorldSpace);

		glm::vec3 corner = glm::mix(cornerWS, cornerWS, (::near_plane + ::far_plane));
		res.push_back(corner);
	}

	return res;
}

void CascadedShadows::calcLightSpaceBoundingBox(const std::vector<glm::vec3>& frustumCorners, glm::vec3& lightDirection,
	glm::vec3& outMin, glm::vec3& outMax) 
{
	glm::mat4 lightView = glm::lookAt(
		glm::vec3(0.f),
		lightDirection,
		glm::vec3(0.f, 1.f, 0.f)
	);

	outMin = glm::vec3(FLT_MAX);
	outMax = glm::vec3(-FLT_MAX);

	for (const auto& corner : frustumCorners) {
		glm::vec4 cornerLightSpace = lightView * glm::vec4(corner, 1.f);

		outMin = glm::min(outMin, glm::vec3(cornerLightSpace));
		outMax = glm::max(outMax, glm::vec3(cornerLightSpace));
	}
}

void CascadedShadows::calcLightSpaceMatrices(const glm::vec3& lightDirection, const glm::mat4& view,
	const glm::mat4& projection) 
{
	for (int i = 0; i < this->numCascades; i++) {
		float nearSplit = (i == 0) ? 0.1f : this->cascadeSplits[i - 1];
		float farSplit = this->cascadeSplits[i];

		std::vector<glm::vec3> frustumCorners = this->calcFrustumCorners(view, projection);

		glm::vec3 min{}, max{}, lightDir{ lightDirection };
		this->calcLightSpaceBoundingBox(frustumCorners, lightDir, min, max);

		glm::mat4 lightProj = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);
		glm::mat4 lightView = glm::lookAt(
			-lightDirection * max.z,
			glm::vec3(0.f),
			glm::vec3(0.f, 1.f, 0.f)
		);
	}
}

void CascadedShadows::calculateShadows(int windowWidth, int windowHeight, std::vector<Mesh*>& meshes, 
	glm::vec3 lightPosition, GLuint currentFramebuffer) 
{
	glViewport(0, 0, this->shadowWidth, this->shadowHeight);

	this->shader.useProgram();

	for (int i = 0; i < this->numCascades; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[i]);
		glClear(GL_DEPTH_BUFFER_BIT);

		this->shader.setMat4("projection", glm::mat4(1.f));
		this->shader.setMat4("view", this->lightSpaceMatrices[i]);

		for (size_t i = 0; i < meshes.size(); i++) {
			this->shader.setMat4("model", meshes[i]->getModelMatrix());
			meshes[i]->renderMesh(GL_TRIANGLES);
		}
	}

	this->shader.endProgram();
}

CascadedShadows::~CascadedShadows() {
	if (this->FBO[0] != 0)
		glDeleteFramebuffers(this->numCascades, this->FBO);

	if (this->shadowMaps[0] != 0)
		glDeleteTextures(this->numCascades, this->shadowMaps);
}