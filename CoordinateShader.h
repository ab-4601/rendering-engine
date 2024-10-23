#pragma once

#include "ShaderLoader.h"

class CoordinateShader : public IShaderLoader {
private:
	GLuint uniformModel, uniformView, uniformProjection;
	GLuint uniformColor;

public:
	CoordinateShader(std::string vertFileName = "coord.vert", std::string fragFileName = "coord.frag");

	virtual void getUniformLocations() override;

	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformView() const { return this->uniformView; }
	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformColor() const { return this->uniformColor; }

	~CoordinateShader() = default;
};

