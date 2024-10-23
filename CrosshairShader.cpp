#include "CrosshairShader.h"

CrosshairShader::CrosshairShader(std::string vertFileName, std::string fragFileName)
	: IShaderLoader(vertFileName, fragFileName, ""), uniformTransformation{ 0 }
{
	this->readFile(this->vertFileName, this->vertexShader);
	this->readFile(this->fragFileName, this->fragmentShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ vertexShaderID, fragmentShaderID });

	glDeleteShader(this->vertexShaderID);
	glDeleteShader(this->fragmentShaderID);
}

void CrosshairShader::getUniformLocations() {
	this->uniformTransformation = glGetUniformLocation(this->programID, "transformation");
}