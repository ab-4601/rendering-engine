#pragma once

#include "ShaderLoader.h"

class HighlightShader : public IShaderLoader {
private:
	GLuint uniformModel, uniformProjection, uniformView;

public:
	HighlightShader(std::string vertFileName = "highlight.vert", std::string fragFileName = "highlight.frag");

	virtual void getUniformLocations() override;

	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformView() const { return this->uniformView; }
	inline GLuint getUniformProjection() const { return this->uniformProjection; }

	~HighlightShader() = default;
};

