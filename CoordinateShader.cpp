#include "CoordinateShader.h"

CoordinateShader::CoordinateShader(std::string vertFileName, std::string fragFileName)
	: IShaderLoader(vertFileName, fragFileName, ""), uniformModel{ 0 }, uniformView{ 0 }, uniformProjection{ 0 },
	uniformColor{ 0 } {

	this->readFile(vertFileName, this->vertexShader);
	this->readFile(fragFileName, this->fragmentShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID });
}

void CoordinateShader::getUniformLocations() {
	this->uniformModel = glGetUniformLocation(this->programID, "model");
	this->uniformView = glGetUniformLocation(this->programID, "view");
	this->uniformProjection = glGetUniformLocation(this->programID, "projection");
	this->uniformColor = glGetUniformLocation(this->programID, "vColor");
}