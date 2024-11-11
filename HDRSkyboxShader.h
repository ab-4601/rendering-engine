#pragma once

#include "ShaderLoader.h"

class HDRSkyboxShader : public IShaderLoader {
private:
	GLuint uniformView{ 0 }, uniformProjection{ 0 };
	GLuint uniformSampler{ 0 };

public:
	HDRSkyboxShader(std::string vertFileName = "HDRskybox.vert", std::string fragFileName = "HDRskybox.frag");

	virtual void getUniformLocations() override;

	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformView() const { return this->uniformView; }
	inline GLuint getUniformSampler() const { return this->uniformSampler; }

	~HDRSkyboxShader() = default;
};

