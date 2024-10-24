#pragma once

#include "ShaderLoader.h"

class HDRShader : public IShaderLoader {
private:
	GLuint uniformExposure = 0;

public:
	HDRShader(std::string vertFileName = "hdr.vert", std::string fragFileName = "hdr.frag");

	virtual void getUniformLocations() override {
		this->uniformExposure = glGetUniformLocation(this->programID, "exposure");
	}

	inline GLuint getUniformExposure() const { return this->uniformExposure; }

	~HDRShader() = default;
};

