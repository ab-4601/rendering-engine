#pragma once

#include "Core.h"
#include "LightingShader.h"
#include "HighlightShader.h"

class Mesh {
protected:
	static unsigned int meshCount;
	unsigned int objectID;

	glm::vec3 color;
	glm::mat4 model;
	glm::mat4 outlineModel;
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	std::vector<GLfloat> texCoords;
	std::vector<GLfloat> normals;

	// Material values for specular lighting
	GLfloat specularIntensity;
	GLfloat specularPower;

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	bool useTexture;
	bool drawIndexed;

	HighlightShader outlineShader;
	LightingShader meshShader;

public:
	static std::vector<Mesh*> meshList;
	Mesh(GLfloat specularIntensity = 0.f, GLfloat specularPower = 0.f);

	virtual void setVertices(const std::vector<GLfloat>& vertices) { this->vertices = vertices; }
	virtual void setIndices(const std::vector<unsigned int>& indices) { this->indices = indices; }
	virtual void setTexCoords(const std::vector<GLfloat>& texCoords) { this->texCoords = texCoords; }
	virtual void setNormals(const std::vector<GLfloat>& normals) { this->normals = normals; }

	virtual inline void setModelMatrix(const glm::mat4& matrix) { this->model = matrix; }
	virtual inline glm::mat4& getModelMatrix() { return this->model; }
	virtual inline bool isDrawIndexed() const { return this->drawIndexed; }
	virtual inline void setColor(glm::vec3 color) { this->color = color; }
	virtual inline glm::vec3 getColor() const { return this->color; }
	virtual inline unsigned int getObjectID() const { return this->objectID; }

	virtual void createUnindexedMesh();
	virtual void createUnindexedTexturedMesh();
	virtual void createMesh();
	virtual void createTexturedMesh();
	virtual void createMeshWithNormals();
	virtual void createMeshFinal();

	virtual void setShader(DirectionalLight& directionalLight,
		std::vector<PointLight>& pointLights, int pointLightCount, std::vector<SpotLight>& spotLights, int spotLightCount,
		const glm::mat4& projection, const glm::mat4& view);

	virtual void renderMesh(GLenum renderMode);

	virtual void renderMeshWithOutline(GLenum renderMode, const glm::mat4& projection, const glm::mat4& view,
		DirectionalLight& dirLight, std::vector<PointLight>& pointLights, int pointLightCount,
		std::vector<SpotLight>& spotLights, int spotLightCount);

	virtual void setMeshMaterial(GLuint specularIntensityLocation, GLuint specularPowerLocation,
		GLfloat sIntensity = 1.f, GLfloat sPower = 32.f);

	void clearMesh();

	~Mesh() {
		this->clearMesh();
	}
};