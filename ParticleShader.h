#pragma once

#include "Core.h"
#include "ShaderLoader.h"

class ParticleShader : public IShaderLoader{
private:
	GLuint uniformColorID, uniformNumberOfRows, uniformProjectionMatrix;

public:
	ParticleShader(std::string vertFileName, std::string fragFileName, std::string geomFileName);

	inline GLuint getUniformColor() const { return this->uniformColorID; }
	inline GLuint getUniformNumberOfRows() const { return this->uniformNumberOfRows; }
	inline GLuint getUniformProjection() const { return this->uniformProjectionMatrix; };

	virtual void getUniformLocations() override;

	~ParticleShader() = default;
};

