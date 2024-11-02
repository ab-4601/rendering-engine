#pragma once

#include "ShaderLoader.h"

class DirectionalShadowShader : public IShaderLoader {
private:
	GLuint uniformProjection{ 0 }, uniformView{ 0 }, uniformModel{ 0 };

public:
	DirectionalShadowShader(
		std::string vertFileName = "directionalShadow.vert",
		std::string fragFileName = "directionalShadow.frag"
	);

	virtual void getUniformLocations() override;

	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformView() const { return this->uniformView; }
	inline GLuint getUniformModel() const { return this->uniformModel; }

	~DirectionalShadowShader() = default;
};