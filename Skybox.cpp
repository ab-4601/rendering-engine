#include "Skybox.h"

Skybox::Skybox(std::vector<std::string> faces)
	: Texture(), faces{ faces }, VAO{ 0 }, VBO{ 0 } {

	this->loadCubemap();

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Skybox::loadCubemap() {
	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (size_t i = 0; i < this->faces.size(); i++) {
		unsigned char* data = stbi_load(this->faces[i].c_str(), &this->width, &this->height, &this->bitDepth, 3);

		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, this->width, this->height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
		else {
			std::cerr << "Cubemap texture failed to load at path: " << this->faces.at(i) << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::loadHDRCubemap(const char* file_name) {
	stbi_set_flip_vertically_on_load(true);

	int width{}, height{}, nrComponents{};
	float* data = stbi_loadf(file_name, &width, &height, &nrComponents, 0);

	if (data) {
		glGenTextures(1, &this->textureID);
		glBindTexture(GL_TEXTURE_2D, this->textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cerr << "HDR skybox image load failed" << std::endl;
	}
}

void Skybox::renderSkybox(const glm::mat4& projection, const Camera& camera) {
	glDepthMask(false);
	glDepthFunc(GL_LEQUAL);

	glUseProgram(this->shader.getProgramID());

	glm::mat4 view = glm::mat4(glm::mat3(camera.generateViewMatrix()));

	glUniformMatrix4fv(this->shader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->shader.getUniformView(), 1, GL_FALSE, glm::value_ptr(view));

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
	glDepthMask(true);
}