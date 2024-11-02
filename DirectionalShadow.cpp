#include "DirectionalShadow.h"

DirectionalShadow::DirectionalShadow(float nearPlane, float farPlane) {
	this->projection = glm::ortho(-5000.f, 5000.f, -5000.f, 5000.f, nearPlane, farPlane);

	this->_init();
}

void DirectionalShadow::_init() {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->depthMap);
	glBindTexture(GL_TEXTURE_2D, this->depthMap);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->SHADOW_WIDTH,
		this->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMap, 0);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DirectionalShadow::calculateShadows(int windowWidth, int windowHeight, 
	std::vector<Mesh*>& meshes, glm::vec3 lightPosition, GLuint currentFramebuffer) 
{
	glViewport(0, 0, this->SHADOW_WIDTH, this->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(this->shader.getProgramID());

	this->view = glm::lookAt(
		lightPosition,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);

	glUniformMatrix4fv(this->shader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(this->projection));
	glUniformMatrix4fv(this->shader.getUniformView(), 1, GL_FALSE, glm::value_ptr(this->view));

	glCullFace(GL_FRONT);

	for (size_t i = 0; i < meshes.size(); i++) {
		glUniformMatrix4fv(
			this->shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(meshes[i]->getModelMatrix())
		);

		meshes[i]->renderMesh(GL_TRIANGLES);
	}

	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);
	glViewport(0, 0, windowWidth, windowHeight);
}

DirectionalShadow::~DirectionalShadow() {
	if (this->FBO != 0)
		glDeleteFramebuffers(1, &this->FBO);

	if (this->depthMap != 0)
		glDeleteTextures(1, &this->depthMap);
}