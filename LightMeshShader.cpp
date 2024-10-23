#include "LightMeshShader.h"

LightMeshShader::LightMeshShader(std::string vertFileName, std::string fragFileName)
	: IShaderLoader(vertFileName, fragFileName, ""), uniformModel{ 0 }, uniformView{ 0 }, uniformProjection{ 0 },
	uniformColor{ 0 }
{
	this->readFile(this->vertFileName, this->vertexShader);
	this->readFile(this->fragFileName, this->fragmentShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID });
}

void LightMeshShader::getUniformLocations() {
	this->uniformModel = glGetUniformLocation(this->programID, "model");
	this->uniformProjection = glGetUniformLocation(this->programID, "projection");
	this->uniformView = glGetUniformLocation(this->programID, "view");
	this->uniformColor = glGetUniformLocation(this->programID, "lightColor");
}