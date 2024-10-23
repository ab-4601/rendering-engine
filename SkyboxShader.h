#pragma once

#include "ShaderLoader.h"

class SkyboxShader : public IShaderLoader {
private:
	GLuint uniformProjection, uniformView;

public:
	SkyboxShader(std::string vertFileName = "skybox.vert", std::string fragFileName = "skybox.frag");

	virtual void getUniformLocations() override;

	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformView() const { return this->uniformView; }

	~SkyboxShader() = default;
};

