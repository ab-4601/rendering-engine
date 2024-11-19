#pragma once

#include "ShaderLoader.h"

class PhysBloomShader : public IShaderLoader {
private:
	GLuint uniformSrcTexture{ 0 }, uniformSrcResolution{ 0 }, uniformFilterRadius{ 0 }, uniformMipLevel{ 0 };

	bool upsample = false;

public:
	PhysBloomShader(std::string vertFileName, std::string fragFileName, bool isUpsample = false);

	virtual void getUniformLocations() override;

	inline GLuint getUniformSrcTexture() const { return this->uniformSrcTexture; }
	inline GLuint getUniformSrcResolution() const { return this->uniformSrcResolution; }
	inline GLuint getUniformFilterRadius() const { return this->uniformFilterRadius; }
	inline GLuint getUniformMipLevel() const { return this->uniformMipLevel; }

	~PhysBloomShader() = default;
};

