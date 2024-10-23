#include "GridShader.h"

GridShader::GridShader(std::string vertFileLocation, std::string fragFileLocation)
	: IShaderLoader(vertFileLocation, fragFileLocation, ""), uniformProjection{ 0 }, uniformView{ 0 }, uniformModel{ 0 },
	uniformCameraPosition{ 0 }, uniformGridCellSize{ 0 }, uniformGridSize{ 0 }, uniformGridColorThick{ 0 },
	uniformGridColorThin{ 0 }, uniformMinPixelBetweenCells{ 0 } {

	this->readFile(this->vertFileName, this->vertexShader);
	this->readFile(this->fragFileName, this->fragmentShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID });
}

void GridShader::getUniformLocations() {
	this->uniformProjection = glGetUniformLocation(this->programID, "projection");
	this->uniformView = glGetUniformLocation(this->programID, "view");
	this->uniformModel = glGetUniformLocation(this->programID, "model");
	this->uniformCameraPosition = glGetUniformLocation(this->programID, "cameraPosition");
}