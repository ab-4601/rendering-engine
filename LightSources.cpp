#include "LightSources.h"

LightSources::LightSources() {
	this->sourceMesh.createMesh();
}

void LightSources::renderLightSources(const glm::mat4& projection, const glm::mat4& view, 
	DirectionalLight& directionalLight, std::vector<PointLight>& pointLights, std::vector<SpotLight>& spotLights,
	int pointLightCount, int spotLightCount) 
{
	glUseProgram(this->shader.getProgramID());

	glm::mat4 model = glm::mat4(1.F);

	glUniformMatrix4fv(this->shader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->shader.getUniformView(), 1, GL_FALSE, glm::value_ptr(view));
	
	glUniform3fv(this->shader.getUniformColor(), 1, glm::value_ptr(directionalLight.getLightColor()));

	glm::vec3 dirLightDirection = directionalLight.getLightDirection();

	model = glm::translate(model, dirLightDirection);
	model = glm::scale(model, glm::vec3(abs(dirLightDirection.x) / 20.f, 
		abs(dirLightDirection.y) / 20.f, abs(dirLightDirection.z) / 20.f));
	glUniformMatrix4fv(this->shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));

	this->sourceMesh.renderMesh();

	for (int i = 0; i < pointLightCount; i++) {
		model = glm::mat4(1.f);
		glUniform3fv(this->shader.getUniformColor(), 1, glm::value_ptr(pointLights[i].getLightColor()));
		
		model = glm::translate(model, pointLights[i].getPosition());
		glUniformMatrix4fv(this->shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));
		this->sourceMesh.renderMesh();
	}

	for (int i = 0; i < spotLightCount; i++) {
		model = glm::mat4(1.f);
		glUniform3fv(this->shader.getUniformColor(), 1, glm::value_ptr(spotLights[i].getLightColor()));

		model = glm::translate(model, spotLights[i].getPosition());
		glUniformMatrix4fv(this->shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));
		this->sourceMesh.renderMesh();
	}
}