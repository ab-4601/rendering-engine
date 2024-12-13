#include "Skybox.h"

Skybox::Skybox(int windowWidth, int windowHeight, const char* fileName)
	: Texture() {
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->quad.createQuad();

	this->_initFBO();
	this->loadEquirectangularMap(fileName);
	this->_generateCubemap(windowWidth, windowHeight);
	this->_generateIrradianceMap(windowWidth, windowHeight);
	this->_capturePrefilterMipmap(windowWidth, windowHeight);
	this->_calculateBRDF(windowWidth, windowHeight);
}

void Skybox::loadEquirectangularMap(const char* file_name) {
	stbi_set_flip_vertically_on_load(true);

	float* data = stbi_loadf(file_name, &this->width, &this->height, &this->bitDepth, 0);

	if (data) {
		glGenTextures(1, &this->textureID);
		glBindTexture(GL_TEXTURE_2D, this->textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->width, this->height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cerr << "HDR skybox image load failed" << std::endl;
	}

	stbi_set_flip_vertically_on_load(false);
}

void Skybox::_initFBO() {
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glGenTextures(1, &this->environmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenTextures(1, &this->irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->irradianceMap);

	for (int i = 0; i < 6; i++)
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			this->CONVOLUTION_WIDTH, this->CONVOLUTION_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL
		);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenTextures(1, &this->prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->prefilterMap);

	for (int i = 0; i < 6; i++)
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			this->PREFILTER_WIDTH, this->PREFILTER_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL
		);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenTextures(1, &this->brdfTexture);
	glBindTexture(GL_TEXTURE_2D, this->brdfTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &this->RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->RBO);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error initializing skybox framebuffer" << std::endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Skybox::_generateCubemap(int windowWidth, int windowHeight) {
	this->hdrToCubeShader.useShader();

	this->hdrToCubeShader.setMat4("projection", this->projection);
	this->hdrToCubeShader.setInt("equirectangularMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	glViewport(0, 0, this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	for (int i = 0; i < 6; i++) {
		this->hdrToCubeShader.setMat4("view", this->viewMatrices[i]);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->environmentMap, 0
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->renderCube();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glViewport(0, 0, windowWidth, windowHeight);

	this->hdrToCubeShader.endShader();
}

void Skybox::_generateIrradianceMap(int windowWidth, int windowHeight) {
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->CONVOLUTION_WIDTH, this->CONVOLUTION_HEIGHT);

	this->irradianceShader.useShader();
	this->irradianceShader.setMat4("projection", this->projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);

	glViewport(0, 0, this->CONVOLUTION_WIDTH, this->CONVOLUTION_HEIGHT);
	for (int i = 0; i < 6; i++) {
		this->irradianceShader.setMat4("view", this->viewMatrices[i]);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradianceMap, 0
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->renderCube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);

	this->irradianceShader.endShader();
}

void Skybox::_capturePrefilterMipmap(int windowWidth, int windowHeight) {
	uint maxMipLevels = 5;

	this->prefilterShader.useShader();
	this->prefilterShader.setInt("environmentMap", 0);
	this->prefilterShader.setMat4("projection", this->projection);
	this->prefilterShader.setFloat("cubemapResolution", this->CUBEMAP_WIDTH);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	for (uint mipLevel = 0; mipLevel < maxMipLevels; mipLevel++) {
		uint mipWidth = static_cast<uint>(this->PREFILTER_WIDTH * pow(0.5f, mipLevel));
		uint mipHeight = static_cast<uint>(this->PREFILTER_HEIGHT * pow(0.5f, mipLevel));

		glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mipLevel / (float)(maxMipLevels - 1);
		this->prefilterShader.setFloat("roughness", roughness);

		for (uint j = 0; j < 6; j++) {
			this->prefilterShader.setMat4("view", this->viewMatrices[j]);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, this->prefilterMap, mipLevel
			);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->renderCube();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);

	this->prefilterShader.endShader();
}

void Skybox::_calculateBRDF(int windowWidth, int windowHeight) {
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->brdfTexture, 0);

	glViewport(0, 0, this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT);

	this->brdfShader.useShader();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->quad.renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, windowWidth, windowHeight);
}

void Skybox::renderSkybox() {
	glDepthFunc(GL_LEQUAL);

	this->skyboxShader.useShader();

	this->skyboxShader.setInt("environmentMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->environmentMap);

	this->renderCube();

	glDepthFunc(GL_LESS);

	this->skyboxShader.endShader();
}