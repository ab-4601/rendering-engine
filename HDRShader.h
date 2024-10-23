#pragma once

#include "ShaderLoader.h"

class HDRShader : public IShaderLoader {
public:
	HDRShader(std::string vertFileName = "hdr.vert", std::string fragFileName = "hdr.frag");

	virtual void getUniformLocations() override {
		return;
	}

	~HDRShader() = default;
};

