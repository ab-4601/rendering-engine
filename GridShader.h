#pragma once

#include "ShaderLoader.h"

class GridShader : public IShaderLoader{
private:
	GLuint uniformProjection, uniformView, uniformModel;
	GLuint uniformCameraPosition, uniformGridSize, uniformGridCellSize;
	GLuint uniformMinPixelBetweenCells, uniformGridColorThick, uniformGridColorThin;

public:
	GridShader(std::string vertFileLocation = "grid.vert", std::string fragFileLocation = "grid.frag");

	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformView() const { return this->uniformView; }
	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformCameraPosition() const { return this->uniformCameraPosition; }
	inline GLuint getUniformGridSize() const { return this->uniformGridSize; }
	inline GLuint getUniformGridCellSize() const { return this->uniformGridCellSize; }
	inline GLuint getUniformMinPixelCount() const { return this->uniformMinPixelBetweenCells; }
	inline GLuint getUniformGridColorThick() const { return this->uniformGridColorThick; }
	inline GLuint getUniformGridColorThin() const { return this->uniformGridColorThin; }

	virtual void getUniformLocations() override;

	~GridShader() = default;
};