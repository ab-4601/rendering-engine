#pragma once

#include "ShaderLoader.h"

class BloomShader : public IShaderLoader {
private:
	GLuint uniformHorizontalFlag = 0;
	GLuint uniformWeights[5] = { 0, 0, 0, 0, 0 };

public:
	BloomShader(std::string vertFileName = "bloom.vert", std::string fragFileName = "bloom.frag");

	virtual void getUniformLocations() override;

	inline GLuint getUniformHorizontalFlag() const { return this->uniformHorizontalFlag; }
	inline GLuint getUniformWeight(int index) const { return this->uniformWeights[index]; };

	~BloomShader() = default;
};

