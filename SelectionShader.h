#pragma once

#include "ShaderLoader.h"

class SelectionShader : public IShaderLoader {
private:
	GLuint uniformModel, uniformView, uniformProjection;
	GLuint uniformObjectIndex, uniformDrawIndex;

public:
	SelectionShader(std::string vertFileName = "selection.vert", std::string fragFileName = "selection.frag");

	virtual void getUniformLocations() override;

	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformView() const { return this->uniformView; }
	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformObjectIndex() const { return this->uniformObjectIndex; }
	inline GLuint getUniformDrawIndex() const { return this->uniformDrawIndex; }

	~SelectionShader() = default;
};

