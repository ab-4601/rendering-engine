#pragma once

#include "Core.h"
#include "HighlightShader.h"
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
	GLfloat specularIntensity{ 0.f }, specularPower{ 0.f };

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	bool useTexture;
	bool useNormalMap;
	bool drawIndexed;
	bool calcShadows;

	HighlightShader outlineShader;
	LightingShader shader;

public:
	static std::vector<Mesh*> meshList;
	Mesh();

	virtual inline void setVertices(const std::vector<GLfloat>& vertices) { this->vertices = vertices; }
	virtual inline void setIndices(const std::vector<uint>& indices) { this->indices = indices; }
	virtual inline void setTexCoords(const std::vector<GLfloat>& texCoords) { this->texCoords = texCoords; }
	virtual inline void setNormals(const std::vector<GLfloat>& normals) { this->normals = normals; }
	virtual inline void scale(float scaleFactor) {
		for (auto& elem : this->vertices)
			elem *= scaleFactor;
	}

	virtual inline void setModelMatrix(const glm::mat4& matrix) { this->model = matrix; }
	virtual inline glm::mat4& getModelMatrix() { return this->model; }
	virtual inline bool isDrawIndexed() const { return this->drawIndexed; }
	virtual inline void setColor(glm::vec3 color) { this->color = color; }
	virtual inline glm::vec3 getColor() const { return this->color; }
	virtual inline uint getObjectID() const { return this->objectID; }
	virtual inline void setObjectID(int objectID) { this->objectID = objectID; }
	virtual void inline setShadowBoolUniform(const bool& calcShadows) {
		this->calcShadows = calcShadows;
	}

	virtual void createUnindexedMesh();
	virtual void createUnindexedTexturedMesh();
	virtual void createFlatMesh();
	virtual void createTexturedMesh();
	virtual void createMeshWithNormals();
	virtual void createMesh();
	virtual void createNormalMappedMesh();

	virtual void setShader(DirectionalLight& directionalLight,
		std::vector<PointLight>& pointLights, int pointLightCount, std::vector<SpotLight>& spotLights, int spotLightCount,
		const glm::mat4& projection, const glm::mat4& view, glm::vec3 cameraPos);

	virtual void renderMesh(
		GLenum renderMode = GL_TRIANGLES, glm::mat4 lightSpaceTransform = glm::mat4(1.f),
		int directionalShadowMap = -1, int pointShadowMap = -1
	);

	virtual void renderMeshWithOutline(
		GLenum renderMode, const glm::mat4& projection, const glm::mat4& view,
		DirectionalLight& dirLight, std::vector<PointLight>& pointLights, int pointLightCount,
		std::vector<SpotLight>& spotLights, int spotLightCount, glm::vec3 cameraPosition,
		glm::mat4 lightSpaceTransform, GLuint directionalShadowMap = 0, GLuint pointShadowMap = 0
	);

	virtual void setMeshMaterial(float specularIntensity = 1.f, float specularPower = 32.f);

	void clearMesh();

	~Mesh() {
		this->clearMesh();
	}
};