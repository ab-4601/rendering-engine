#pragma once

#include "ShaderLoader.h"

class LightMeshShader : public IShaderLoader {
private:
	GLuint uniformModel, uniformProjection, uniformView;
	GLuint uniformColor;

public:
	LightMeshShader(std::string vertFileName = "light.vert", std::string fragFileName = "light.frag");

	virtual void getUniformLocations() override;

	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformView() const { return this->uniformView; }
	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformColor() const { return this->uniformColor; }

	~LightMeshShader() = default;
};

