#pragma once

#include "Core.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "PBRShader.h"

class Mesh {
protected:
	static uint meshCount;
	uint objectID;

	glm::vec3 color;
	glm::mat4 model;
	glm::mat4 outlineModel;
	std::vector<GLfloat> vertices;
	std::vector<uint> indices;
	std::vector<GLfloat> texCoords;
	std::vector<GLfloat> normals;

	// Material values for specular lighting
	GLfloat metallic{ 0.f }, roughness{ 0.f }, ao{ 0.f };

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	Texture* diffuseMap = nullptr;
	Texture* normalMap = nullptr;
	Texture* depthMap = nullptr;
	Texture* metallicMap = nullptr;
	Texture* roughnessMap = nullptr;

	bool useTexture;
	bool useNormalMap;
	bool useMaterialMap;
	bool drawIndexed;
	bool calcShadows;
	bool enableSSAO;

	Shader outlineShader{ "highlight.vert", "highlight.frag" };
	PBRShader shader;

public:
	static std::vector<Mesh*> meshList;
	Mesh();

	virtual inline void setVertices(const std::vector<GLfloat>& vertices) { this->vertices = vertices; }
	virtual inline void setIndices(const std::vector<uint>& indices) { this->indices = indices; }
	virtual inline void setTexCoords(const std::vector<GLfloat>& texCoords) { this->texCoords = texCoords; }
	virtual inline void setNormals(const std::vector<GLfloat>& normals) { this->normals = normals; }

	virtual inline glm::mat4& getModelMatrix() { return this->model; }
	virtual inline glm::vec3 getColor() const { return this->color; }
	virtual inline uint getObjectID() const { return this->objectID; }
	virtual inline float getMetallic() const { return this->metallic; }
	virtual inline float getRoughness() const { return this->roughness; }
	virtual inline float getAO() const { return this->ao; }
	virtual inline bool isDrawIndexed() const { return this->drawIndexed; }

	virtual inline bool getDiffuseMapBool() const { return this->useTexture; }
	virtual inline bool getNormalMapBool() const { return this->useNormalMap; }
	virtual inline bool getMaterialMapBool() const { return this->useMaterialMap; }

	virtual inline void setDiffuseMap(Texture* materialMap) { this->diffuseMap = materialMap; }
	virtual inline void setNormalMap(Texture* materialMap) { this->normalMap = materialMap; }
	virtual inline void setMetallicMap(Texture* materialMap) { this->metallicMap = materialMap; }
	virtual inline void setRoughnessMap(Texture* materialMap) { this->roughnessMap = materialMap; }

	virtual inline void bindDiffuseMap() const { this->diffuseMap->useTexture(); }
	virtual inline void bindNormalMap() const { this->normalMap->useNormalMap(); }
	virtual inline void bindMetallicMap() const { this->metallicMap->useMetalnessMap(); }
	virtual inline void bindRoughnessMap() const { this->roughnessMap->useRoughnessMap(); }

	virtual inline void setModelMatrix(const glm::mat4& matrix) { this->model = matrix; }
	virtual inline void setColor(glm::vec3 color) { this->color = color; }
	virtual inline void setObjectID(int objectID) { this->objectID = objectID; }
	virtual void inline setShadowBoolUniform(bool calcShadows) { this->calcShadows = calcShadows; }
	virtual void inline setSSAOboolUniform(bool enableSSAO) { this->enableSSAO = enableSSAO; }

	virtual void createUnindexedMesh();
	virtual void createUnindexedTexturedMesh();
	virtual void createFlatMesh();
	virtual void createTexturedMesh();
	virtual void createMeshWithNormals();
	virtual void createMesh();
	virtual void createModel();

	virtual void setShader(DirectionalLight& directionalLight, std::vector<PointLight>& pointLights, int pointLightCount,
		std::vector<SpotLight>& spotLights, int spotLightCount, int numCascades, std::vector<float> cascadePlanes,
		const Camera& camera);

	virtual void renderMesh(
		GLenum renderMode = GL_TRIANGLES, 
		GLuint pointShadowMap = 0, GLuint cascadedShadowMap = 0,
		GLuint irradianceMap = 0, GLuint brdfSampler = 0, GLuint prefilterSampler = 0
	);

	virtual void renderMeshWithOutline(
		GLenum renderMode, const Camera& camera, DirectionalLight& dirLight, 
		std::vector<PointLight>& pointLights, int pointLightCount, std::vector<SpotLight>& spotLights,
		int spotLightCount, int numCascades, std::vector<float> cascadePlanes,
		bool calcShadows, GLuint pointShadowMap = 0, GLuint cascadedShadowMap = 0, GLuint irradianceMap = 0,
		GLuint brdfSampler = 0, GLuint prefilterSampler = 0
	);

	virtual void setMeshMaterial(float roughness, float metallic, float ao);

	void clearMesh();

	~Mesh() {
		this->clearMesh();
	}
};