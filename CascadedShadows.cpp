#include "CascadedShadows.h"

CascadedShadows::CascadedShadows(float lambda, int windowSize, int filterSize) {
	this->calcSplitDepths(lambda);
	this->_init();
	this->genRandomOffsetData(windowSize, filterSize);
}

bool CascadedShadows::checkFramebufferStatus() {
	uint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void CascadedShadows::genRandomOffsetData(int windowSize, int filterSize) {
	int bufferSize = windowSize * windowSize * filterSize * filterSize * 2;

	this->offsetData.clear();
	this->offsetData.resize(bufferSize);

	int index = 0;

	for (int i = 0; i < windowSize; i++) {
		for (int j = 0; j < windowSize; j++) {
			for (int x = filterSize - 1; x >= 0; x--) {
				for (int y = 0; y < filterSize; y++) {
					float u = ((float)y + 0.5f + this->jitter(-0.5f, 0.5f)) / (float)filterSize;
					float v = ((float)x + 0.5f + this->jitter(-0.5f, 0.5f)) / (float)filterSize;

					assert(index + 1 < this->offsetData.size());
					this->offsetData[index] = (sqrtf(v) * cosf(2 * PI * u));
					this->offsetData[index + 1] = (sqrtf(v) * sinf(2 * PI * u));

					index += 2;
				}
			}
		}
	}

	this->genRandomOffsetTexture(windowSize, filterSize);
}

void CascadedShadows::genRandomOffsetTexture(int windowSize, int filterSize) {
	int numFilterSamples = std::pow(filterSize, 2);

	glGenTextures(1, &this->randomOffset);
	glBindTexture(GL_TEXTURE_3D, this->randomOffset);

	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, numFilterSamples / 2, windowSize, windowSize);
	glTexSubImage3D(
		GL_TEXTURE_3D, 0, 0, 0, 0, numFilterSamples / 2, windowSize, windowSize, GL_RGBA, GL_FLOAT, &this->offsetData[0]
	);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void CascadedShadows::_init() {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->shadowMaps);
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->shadowMaps);
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, this->mapResolution, this->mapResolution,
		this->numCascades, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
	);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	constexpr float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadowMaps, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (!this->checkFramebufferStatus()) {
		std::cerr << "Error initializing CSM framebuffer" << std::endl;
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &this->UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, this->UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CascadedShadows::calcSplitDepths(float lambda) {
	this->cascadeSplits.clear();

	float range = ::far_plane - ::near_plane;
	float logFactor = std::log(::far_plane / ::near_plane);

	for (int i = 0; i < this->numCascades; i++) {
		float p = (float)(i + 1) / this->numCascades;

		float linearSplit = ::near_plane + range * p;

		float logSplit = ::near_plane * std::pow(logFactor, p);

		this->cascadeSplits.push_back(lambda * linearSplit + (1 - lambda) * logSplit);
	}
}

void CascadedShadows::calcFrustumCorners(glm::mat4 projection, glm::mat4 view) {
	glm::mat4 inverse = glm::inverse(projection * view);

	this->frustumCorners.clear();

	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				glm::vec4 point{ 2.f * x - 1.f, 2.f * y - 1.f, 2.f * z - 1.f, 1.f };

				point = inverse * point;
				point /= point.w;

				this->frustumCorners.push_back(point);
			}
		}
	}
}

void CascadedShadows::calcLightSpaceMatrix(const Camera& camera, glm::vec3 lightDir,
	int windowWidth, int windowHeight, const float& nearPlane, const float& farPlane)
{
	float aspect = (float)windowWidth / windowHeight;
	glm::mat4 projection = glm::perspective(glm::radians(45.f), aspect, nearPlane, farPlane);
	glm::mat4 view = camera.generateViewMatrix();

	glm::vec3 center = glm::vec3(0.f, 0.f, 0.f);

	this->calcFrustumCorners(projection, view);

	for (const auto& corner : this->frustumCorners)
		center += glm::vec3(corner);

	center /= this->frustumCorners.size();

	glm::mat4 lightView = glm::lookAt(
		center + lightDir,
		center,
		glm::vec3(0.f, 1.f, 0.f)
	);

	glm::vec3 min{ std::numeric_limits<float>::max() };
	glm::vec3 max{ std::numeric_limits<float>::lowest() };

	for (const glm::vec4& corner : this->frustumCorners) {
		glm::vec3 trf = glm::vec3(lightView * corner);
		min = glm::min(min, trf);
		max = glm::max(max, trf);
	}

	float zMult = 10.f;

	min.z = (min.z < 0.f) ? min.z * zMult : min.z / zMult;
	max.z = (max.z < 0.f) ? max.z / zMult : max.z * zMult;

	glm::mat4 lightProjection = glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);

	this->lightSpaceMatrices.push_back(lightProjection * lightView);
}

void CascadedShadows::calcLightSpaceMatrices(const Camera& camera, glm::vec3 lightDir, int windowWidth, int windowHeight) 
{
	this->lightSpaceMatrices.clear();

	for (int i = 0; i < this->numCascades + 1; i++) {
		if (i == 0) {
			this->calcLightSpaceMatrix(
				camera, lightDir, windowWidth, windowHeight, ::near_plane, this->cascadeSplits[i]
			);
		}
		else if (i < this->numCascades) {
			this->calcLightSpaceMatrix(
				camera, lightDir, windowWidth, windowHeight, this->cascadeSplits[i - 1], this->cascadeSplits[i]
			);
		}
		else {
			this->calcLightSpaceMatrix(
				camera, lightDir, windowWidth, windowHeight, this->cascadeSplits[i - 1], ::far_plane
			);
		}
	}
}

void CascadedShadows::calculateShadows(const Camera& camera, int windowWidth, int windowHeight, std::vector<Mesh*>& meshes, 
	glm::vec3 lightPosition, GLuint currFramebuffer)
{
	this->calcLightSpaceMatrices(camera, lightPosition, windowWidth, windowHeight);

	glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);

	for (size_t i = 0; i < this->lightSpaceMatrices.size(); i++) {
		glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &this->lightSpaceMatrices.at(i));
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, this->mapResolution, this->mapResolution);

	this->shader.useShader();

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(3.f, 3.f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	for (size_t i = 0; i < meshes.size(); i++) {
		this->shader.setMat4("model", meshes[i]->getModelMatrix());
		meshes[i]->renderMesh(GL_TRIANGLES);
	}

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	glDisable(GL_POLYGON_OFFSET_FILL);

	this->shader.endShader();

	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, currFramebuffer);
}

CascadedShadows::~CascadedShadows() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->shadowMaps != 0)
		glDeleteTextures(1, &this->shadowMaps);
}