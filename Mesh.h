#pragma once

#include "Core.h"
#include "LightingShader.h"
#include "HighlightShader.h"

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
	GLfloat specularIntensity;
	GLfloat specularPower;

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	bool useTexture;
	bool useNormalMap;
	bool drawIndexed;

	HighlightShader outlineShader;
	LightingShader meshShader;

public:
	static std::vector<Mesh*> meshList;
	Mesh(GLfloat specularIntensity = 0.f, GLfloat specularPower = 0.f);

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

	virtual void renderMesh(GLenum renderMode);

	virtual void renderMeshWithOutline(GLenum renderMode, const glm::mat4& projection, const glm::mat4& view,
		DirectionalLight& dirLight, std::vector<PointLight>& pointLights, int pointLightCount,
		std::vector<SpotLight>& spotLights, int spotLightCount, glm::vec3 cameraPosition);

	virtual void setMeshMaterial(GLfloat sIntensity = 1.f, GLfloat sPower = 32.f);

	void clearMesh();

	~Mesh() {
		this->clearMesh();
	}
};