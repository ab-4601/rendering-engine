#pragma once

#include "LightingShader.h"

class PBRShader : public LightingShader {
private:
	GLuint uniformAlbedo{ 0 }, uniformMetallic{ 0 }, uniformRoughness{ 0 }, uniformAo{ 0 };

public:
	PBRShader(
		std::string vertFileName = "PBR.vert",
		std::string fragFileName = "PBR.frag",
		std::string geomFileName = "PBR.geom"
	);

	virtual void getUniformLocations() override;

	inline GLuint getUniformAlbedo() const { return this->uniformAlbedo; }
	inline GLuint getUniformMetallic() const { return this->uniformMetallic; }
	inline GLuint getUniformRoughness() const { return this->uniformRoughness; }
	inline GLuint getUniformAo() const { return this->uniformAo; }

	~PBRShader() = default;
};

