#include "Mesh.h"

Mesh::Mesh(GLfloat specularIntensity, GLfloat specularPower)
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 }, color{ 1.f, 1.f, 1.f }, model{ glm::mat4(1.f) }, objectID{ 0 },
	specularIntensity{ specularIntensity }, specularPower{ specularPower }, useTexture{ false }, drawIndexed{ false }
{
	meshList.push_back(this);
	this->objectID = this->meshCount++;

	this->vertices.clear();
	this->indices.clear();
	this->texCoords.clear();
	this->normals.clear();

	this->outlineModel = glm::scale(this->model, glm::vec3(1.1f, 1.1f, 1.1f));
}

void Mesh::createUnindexedMesh() {
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createUnindexedTexturedMesh() {
	this->useTexture = true;

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createMesh() {
	this->drawIndexed = true;

	// Generate vertex array object and bind it
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// Generate vertex buffer object and bind it
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Generate index buffer object and bind it
	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createTexturedMesh() {
	std::vector<GLfloat> tmpVertices = this->vertices;
	this->vertices.clear();

	this->useTexture = true;
	this->drawIndexed = true;

	for (size_t i = 0, j = 0; i < tmpVertices.size(); i++) {
		if (i % 3 == 0 && i != 0) {
			this->vertices.push_back(this->texCoords.at(j));
			this->vertices.push_back(this->texCoords.at(j + 1));

			j += 2;
		}

		this->vertices.push_back(tmpVertices.at(i));
	}

	tmpVertices.clear();

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::createMeshWithNormals() {
	std::vector<GLfloat> tmpVertices = this->vertices;
	this->vertices.clear();

	this->drawIndexed = true;

	for (size_t i = 0; i < tmpVertices.size(); i += 3) {
		this->vertices.push_back(tmpVertices.at(i));
		this->vertices.push_back(tmpVertices.at(i + 1));
		this->vertices.push_back(tmpVertices.at(i + 2));

		this->vertices.push_back(this->normals.at(i));
		this->vertices.push_back(this->normals.at(i + 1));
		this->vertices.push_back(this->normals.at(i + 2));
	}

	tmpVertices.clear();

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createMeshFinal() {
	std::vector<GLfloat> tmpVertices = this->vertices;
	this->vertices.clear();

	this->useTexture = true;
	this->drawIndexed = true;

	for (size_t i = 0, j = 0; i < tmpVertices.size(); i += 3) {
		this->vertices.push_back(tmpVertices.at(i));
		this->vertices.push_back(tmpVertices.at(i + 1));
		this->vertices.push_back(tmpVertices.at(i + 2));

		this->vertices.push_back(this->texCoords.at(j++));
		this->vertices.push_back(this->texCoords.at(j++));

		this->vertices.push_back(this->normals.at(i));
		this->vertices.push_back(this->normals.at(i + 1));
		this->vertices.push_back(this->normals.at(i + 2));
	}

	tmpVertices.clear();

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 5));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::setMeshMaterial(GLuint specularIntensityLocation, GLuint specularPowerLocation,
	GLfloat sIntensity, GLfloat sPower) {

	this->specularIntensity = sIntensity;
	this->specularPower = sPower;

	glUniform1f(specularIntensityLocation, this->specularIntensity);
	glUniform1f(specularPowerLocation, this->specularPower);
}

void Mesh::setShader(DirectionalLight& directionalLight, std::vector<PointLight>& pointLights, int pointLightCount, 
	std::vector<SpotLight>& spotLights, int spotLightCount, const glm::mat4& projection, const glm::mat4& view) 
{
	glUseProgram(this->meshShader.getProgramID());

	this->meshShader.setDirectionalLight(&directionalLight);
	this->meshShader.setPointLights(pointLights.data(), pointLightCount);
	this->meshShader.setSpotLights(spotLights.data(), spotLightCount);

	glUniformMatrix4fv(this->meshShader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->meshShader.getUniformView(), 1, GL_FALSE, glm::value_ptr(view));
}

void Mesh::renderMesh(GLenum renderMode)  {
	glUniform1i(this->meshShader.getUniformTextureBool(), this->useTexture);
	glUniform3fv(this->meshShader.getUniformColor(), 1, glm::value_ptr(this->color));
	glUniformMatrix4fv(this->meshShader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(this->model));
	glBindVertexArray(this->VAO);

	if (this->drawIndexed) {
		glDrawElements(renderMode, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, this->indices.data());
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		glDrawArrays(renderMode, 0, (GLsizei)this->vertices.size());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
}

void Mesh::renderMeshWithOutline(GLenum renderMode, const glm::mat4& projection, const glm::mat4& view,
	DirectionalLight& dirLight, std::vector<PointLight>& pointLights, int pointLightCount,
	std::vector<SpotLight>& spotLights, int spotLightCount)
{
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glUniformMatrix4fv(this->meshShader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(this->model));

	this->renderMesh(renderMode);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(this->outlineShader.getProgramID());

	glUniformMatrix4fv(this->outlineShader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->outlineShader.getUniformView(), 1, GL_FALSE, glm::value_ptr(view));

	this->outlineModel = glm::scale(this->model, glm::vec3(1.03f, 1.03f, 1.03f));
	glUniformMatrix4fv(this->outlineShader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(this->outlineModel));

	this->renderMesh(GL_TRIANGLES);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);

	this->setShader(dirLight, pointLights, pointLightCount, spotLights, spotLightCount, projection, view);
}

void Mesh::clearMesh() {
	if (this->IBO != 0) {
		glDeleteBuffers(1, &this->IBO);
		this->IBO = 0;
	}

	if (this->VBO != 0) {
		glDeleteBuffers(1, &this->VBO);
		this->VBO = 0;
	}

	if (this->VAO != 0) {
		glDeleteVertexArrays(1, &this->VAO);
		this->VAO = 0;
	}
}