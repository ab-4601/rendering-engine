#include "Mesh.h"

Mesh::Mesh()
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 }, color{ 1.f, 1.f, 1.f }, model{ glm::mat4(1.f) }, objectID{ 0 }, enableSSAO{ false },
	useTexture{ false }, drawIndexed{ false }, useNormalMap{ false }, calcShadows{ false }, useMaterialMap{ false }
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

void Mesh::createFlatMesh() {
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createMesh() {
	this->useTexture = true;
	this->drawIndexed = true;

	if(this->normals.size() != 0 && this->texCoords.size() != 0) {
		std::vector<GLfloat> tmpVertices = this->vertices;
		this->vertices.clear();

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
	}

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::createModel() {
	this->useTexture = true;
	this->drawIndexed = true;
	this->useNormalMap = true;
	this->useMaterialMap = true;

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(sizeof(float) * 5));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(sizeof(float) * 8));
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(uint), this->indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::setShader(DirectionalLight& directionalLight, std::vector<PointLight>& pointLights, int pointLightCount,
	std::vector<SpotLight>& spotLights, int spotLightCount, int numCascades, std::vector<float> cascadePlanes,
	const Camera& camera)
{
	glUseProgram(this->shader.getProgramID());

	this->shader.setDirectionalLight(&directionalLight);
	this->shader.setPointLights(pointLights.data(), pointLightCount);
	this->shader.setSpotLights(spotLights.data(), spotLightCount);

	glUniformMatrix4fv(this->shader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));
	glUniformMatrix4fv(this->shader.getUniformView(), 1, GL_FALSE, glm::value_ptr(camera.generateViewMatrix()));
	glUniform3fv(this->shader.getEyePosition(), 1, glm::value_ptr(camera.getCameraPosition()));
	glUniform1f(this->shader.getUniformFarPlane(), ::far_plane);
	glUniform1f(this->shader.getUniformNearPlane(), ::near_plane);
	glUniform1i(this->shader.getUniformCascadeCount(), numCascades);
	glUniform1i(this->shader.getUniformCalcShadows(), this->calcShadows);
	glUniform1i(this->shader.getUniformSSAObool(), this->enableSSAO);

	for (size_t i = 0; i < cascadePlanes.size(); i++)
		glUniform1f(this->shader.getUniformCascadePlaneDistance((int)i), cascadePlanes[i]);

	glUniform1i(this->shader.getUniformDiffuseSampler(), 0);
	glUniform1i(this->shader.getUniformNormalSampler(), 1);
	glUniform1i(this->shader.getUniformDepthSampler(), 2);
	glUniform1i(this->shader.getUniformMetallicSampler(), 3);
	glUniform1i(this->shader.getUniformRoughnessSampler(), 4);
	glUniform1i(this->shader.getUniformBRDFSampler(), 5);
	glUniform1i(this->shader.getUniformPrefilterSampler(), 6);
	glUniform1i(this->shader.getUniformIrradianceSampler(), 7);
	glUniform1i(this->shader.getUniformPointShadowSampler(), 8);
	glUniform1i(this->shader.getUniformCSMSampler(), 9);
}

void Mesh::setMeshMaterial(float roughness, float metallic, float ao) {
	this->roughness = roughness;
	this->metallic = metallic;
	this->ao = ao;
}

void Mesh::renderMesh(GLenum renderMode, GLuint pointShadowMap, GLuint cascadedShadowMap,
	GLuint irradianceMap, GLuint brdfSampler, GLuint prefilterSampler)
{
	glUniform3fv(this->shader.getUniformAlbedo(), 1, glm::value_ptr(this->color));
	glUniform1f(this->shader.getUniformAo(), this->ao);
	glUniform1f(this->shader.getUniformRoughness(), this->roughness);
	glUniform1f(this->shader.getUniformMetallic(), this->metallic);

	glUniform1i(this->shader.getUniformTextureBool(), this->useTexture);
	glUniform1i(this->shader.getUniformNormalMapBool(), this->useNormalMap);
	glUniform1i(this->shader.getUniformUseMaterialMap(), this->useMaterialMap);

	glUniformMatrix4fv(this->shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(this->model));
	glUniform3fv(this->shader.getUniformColor(), 1, glm::value_ptr(this->color));

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, brdfSampler);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterSampler);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowMap);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D_ARRAY, cascadedShadowMap);

	glBindVertexArray(this->VAO);
	if (this->drawIndexed) {
		glDrawElements(renderMode, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, this->indices.data());
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

		glDrawArrays(renderMode, 0, (GLsizei)this->vertices.size() / 6);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::renderMeshWithOutline(GLenum renderMode, const Camera& camera,
	DirectionalLight& dirLight, std::vector<PointLight>& pointLights, int pointLightCount,
	std::vector<SpotLight>& spotLights, int spotLightCount, int numCascades, 
	std::vector<float> cascadePlanes, bool calcShadows,
	GLuint pointShadowMap, GLuint cascadedShadowMap, GLuint irradianceMap, GLuint brdfSampler, GLuint prefilterSampler)
{
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	this->renderMesh(renderMode, pointShadowMap, cascadedShadowMap, irradianceMap, brdfSampler, prefilterSampler);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	//glDisable(GL_DEPTH_TEST);

	this->outlineShader.useShader();

	this->outlineShader.setMat4("projection", camera.getProjectionMatrix());
	this->outlineShader.setMat4("view", camera.generateViewMatrix());

	this->outlineModel = glm::scale(this->model, glm::vec3(1.05f, 1.05f, 1.05f));
	this->outlineShader.setMat4("model", this->outlineModel);

	this->renderMesh(GL_TRIANGLES);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glDisable(GL_STENCIL_TEST);
	//glEnable(GL_DEPTH_TEST);

	this->setShadowBoolUniform(calcShadows);
	this->outlineShader.endShader();

	this->setShader(
		dirLight, pointLights, pointLightCount, spotLights, spotLightCount, numCascades, cascadePlanes, camera
	);
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