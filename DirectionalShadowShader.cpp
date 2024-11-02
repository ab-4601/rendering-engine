#include "DirectionalShadowShader.h"

DirectionalShadowShader::DirectionalShadowShader(std::string vertFileName, std::string fragFileName)
	: IShaderLoader(vertFileName, fragFileName, "")
{
	this->readFile(this->vertFileName, this->vertexShader);
	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);

	this->readFile(this->fragFileName, this->fragmentShader);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID });
}

void DirectionalShadowShader::getUniformLocations() {
	this->uniformModel = glGetUniformLocation(this->programID, "model");
	this->uniformView = glGetUniformLocation(this->programID, "view");
	this->uniformProjection = glGetUniformLocation(this->programID, "projection");
}