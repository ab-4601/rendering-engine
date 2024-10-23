#include "Grid.h"

Grid::Grid()
	: VAO{ 0 }, VBO{ 0 }, IBO{ 0 } {

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Grid::renderGrid(glm::mat4& model, const glm::mat4& projection, const glm::mat4& view, glm::vec3 camPos) {
	glUseProgram(this->shader.getProgramID());

	model = glm::mat4(1.f);

	glUniformMatrix4fv(this->shader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->shader.getUniformView(), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(this->shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(this->shader.getUniformCameraPosition(), 1, glm::value_ptr(camPos));
	glUniform1f(this->shader.getUniformGridSize(), (float)INT_MAX);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(this->VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, this->indices);

	glBindVertexArray(0);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

Grid::~Grid() {
	if (this->VAO != 0)
		glDeleteVertexArrays(1, &this->VAO);

	if (this->VBO != 0)
		glDeleteBuffers(1, &this->VBO);

	if (this->IBO != 0)
		glDeleteBuffers(1, &this->IBO);
}
