#pragma once

#include "ShaderLoader.h"

class ShadowShader : public IShaderLoader {
private:
	GLuint uniformModel{ 0 }, uniformShadowMatrices[6] = { 0, 0, 0, 0, 0, 0 };
	GLuint uniformLightPos{ 0 }, uniformFarPlane{ 0 };

public:
	ShadowShader(
		std::string vertFileName = "shadow.vert", 
		std::string fragFileName = "shadow.frag", 
		std::string geomFileName = "shadow.geom"
	);

	virtual void getUniformLocations() override;

	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformShadowMatrix(int index) const { return this->uniformShadowMatrices[index]; }
	inline GLuint getUniformLightPos() const { return this->uniformLightPos; }
	inline GLuint getUniformFarPlane() const { return this->uniformFarPlane; }

	~ShadowShader() = default;
};