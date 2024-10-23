#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 worldUpDir, GLfloat pitch, GLfloat yaw, GLfloat moveSpeed, GLfloat turnSpeed) :
	position{ position }, worldUpDir{ worldUpDir }, pitch{ pitch }, yaw{ yaw },
	movementSpeed{ moveSpeed }, turnSpeed{ turnSpeed }, movementSpeedMultiplier{ 1.f } {

	glm::vec3 origin(0.f, 0.f, 0.f);
	this->front = glm::normalize(this->position - origin);

	update();
}

void Camera::keyFunctionality(const Window* currWindow, GLfloat deltaTime) {
	GLfloat velocity = this->movementSpeed * deltaTime * this->movementSpeedMultiplier;

	if (currWindow->getKeyPress(GLFW_KEY_W) || currWindow->getKeyPress(GLFW_KEY_UP))
		this->position += this->front * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_S) || currWindow->getKeyPress(GLFW_KEY_DOWN))
		this->position -= this->front * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_D) || currWindow->getKeyPress(GLFW_KEY_RIGHT))
		this->position += this->right * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_A) || currWindow->getKeyPress(GLFW_KEY_LEFT))
		this->position -= this->right * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_SPACE))
		this->position += this->up * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_CONTROL))
		this->position -= this->up * velocity;

	if (currWindow->getKeyPress(GLFW_KEY_LEFT_SHIFT))
		this->movementSpeedMultiplier = 3.f;
	else
		this->movementSpeedMultiplier = 1.f;
}

void Camera::mouseFunctionality(GLfloat xChange, GLfloat yChange, GLfloat scrollChange) {
	if (this->movementSpeed > 0)
		this->movementSpeed += scrollChange * 20;

	if (this->movementSpeed <= 0)
		this->movementSpeed = 1.f;

	xChange *= this->turnSpeed;
	yChange *= this->turnSpeed;

	this->yaw += xChange;
	this->pitch += yChange;

	if (this->pitch > 89.f) {
		this->pitch = 89.f;
	}


	if (this->pitch < -89.f) {
		this->pitch = -89.f;
	}

	this->update();
}

void Camera::update() {
	this->front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
	this->front.y = sinf(glm::radians(pitch));
	this->front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

	this->front = glm::normalize(this->front);

	this->right = glm::normalize(glm::cross(this->front, this->worldUpDir));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}