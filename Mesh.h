#pragma once

#include "Core.h"
#include "Shader.h"
#include "LightingShader.h"
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

	bool useTexture;
	bool useNormalMap;
	bool useMaterialMap;
	bool drawIndexed;
	bool calcShadows;

	Shader outlineShader{ "highlight.vert", "highlight.frag" };
	Shader lightingShader{ "PBR.vert", "PBR.frag", "PBR.geom" };

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


	virtual inline void setModelMatrix(const glm::mat4& matrix) { this->model = matrix; }
	virtual inline void setColor(glm::vec3 color) { this->color = color; }
	virtual inline void setObjectID(int objectID) { this->objectID = objectID; }
	virtual void inline setShadowBoolUniform(bool calcShadows) {
		this->calcShadows = calcShadows;
	}

	virtual void createUnindexedMesh();
	virtual void createUnindexedTexturedMesh();
	virtual void createFlatMesh();
	virtual void createTexturedMesh();
	virtual void createMeshWithNormals();
	virtual void createMesh();
	virtual void createModel();

	virtual void setShader(DirectionalLight& directionalLight,
		std::vector<PointLight>& pointLights, int pointLightCount, std::vector<SpotLight>& spotLights, int spotLightCount,
		const glm::mat4& projection, const glm::mat4& view, glm::vec3 cameraPos);

	virtual void renderMesh(
		GLenum renderMode = GL_TRIANGLES, glm::mat4 lightSpaceTransform = glm::mat4(1.f),
		GLuint directionalShadowMap = 0, GLuint pointShadowMap = 0, GLuint irradianceMap = 0, GLuint brdfSampler = 0,
		GLuint prefilterSampler = 0
	);

	virtual void renderMeshWithOutline(
		GLenum renderMode, const glm::mat4& projection, const glm::mat4& view,
		DirectionalLight& dirLight, std::vector<PointLight>& pointLights, int pointLightCount,
		std::vector<SpotLight>& spotLights, int spotLightCount, glm::vec3 cameraPosition,
		glm::mat4 lightSpaceTransform, bool calcShadows, GLuint directionalShadowMap = 0, GLuint pointShadowMap = 0,
		GLuint irradianceMap = 0, GLuint brdfSampler = 0, GLuint prefilterSampler = 0
	);

	virtual void setMeshMaterial(float roughness, float metallic, float ao);

	void clearMesh();

	~Mesh() {
		this->clearMesh();
	}
};