#pragma once

#include "ShaderLoader.h"

class HDRShader : public IShaderLoader {
private:
	GLuint uniformExposure{ 0 }, uniformHDRbuffer{ 0 }, uniformBloomBuffer{ 0 };

public:
	HDRShader(std::string vertFileName = "hdr.vert", std::string fragFileName = "hdr.frag");

	virtual void getUniformLocations() override {
		this->uniformExposure = glGetUniformLocation(this->programID, "exposure");
		this->uniformHDRbuffer = glGetUniformLocation(this->programID, "hdrBuffer");
		this->uniformBloomBuffer = glGetUniformLocation(this->programID, "bloomBuffer");
	}

	inline GLuint getUniformExposure() const { return this->uniformExposure; }
	inline GLuint getUniformHDRbuffer() const { return this->uniformHDRbuffer; }
	inline GLuint getUniformBloomBuffer() const { return this->uniformBloomBuffer; }

	~HDRShader() = default;
};

