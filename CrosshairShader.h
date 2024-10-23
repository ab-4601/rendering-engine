#pragma once

#include "Core.h"
#include "ShaderLoader.h"

class CrosshairShader : public IShaderLoader {
private:
	GLuint uniformTransformation;

public:
	CrosshairShader(std::string vertFileName = "crosshair.vert", std::string fragFileName = "crosshair.frag");

	inline GLuint getUniformTransformation() const { return this->uniformTransformation; }

	virtual void getUniformLocations() override;

	~CrosshairShader() = default;
};

