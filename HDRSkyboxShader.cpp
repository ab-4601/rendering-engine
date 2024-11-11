#include "HDRSkyboxShader.h"

HDRSkyboxShader::HDRSkyboxShader(std::string vertFileName, std::string fragFileName)
	: IShaderLoader(vertFileName, fragFileName, "")
{
	this->readFile(this->vertFileName, this->vertexShader);
	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);

	this->readFile(this->fragFileName, this->fragmentShader);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID });
}

void HDRSkyboxShader::getUniformLocations() {
	this->uniformProjection = glGetUniformLocation(this->programID, "projection");
	this->uniformView = glGetUniformLocation(this->programID, "view");
	this->uniformSampler = glGetUniformLocation(this->programID, "equirectangularMap");
}