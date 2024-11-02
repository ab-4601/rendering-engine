#include "ShadowShader.h"

ShadowShader::ShadowShader(std::string vertFileName, std::string fragFileName, std::string geomFileName)
	: IShaderLoader(vertFileName, fragFileName, geomFileName)
{
	this->readFile(this->vertFileName, this->vertexShader);
	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);

	this->readFile(this->fragFileName, this->fragmentShader);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->readFile(this->geomFileName, this->geometryShader);
	this->compileShader(this->geometryShaderID, this->geometryShader, GL_GEOMETRY_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID, this->geometryShaderID });
}

void ShadowShader::getUniformLocations() {
	this->uniformModel = glGetUniformLocation(this->programID, "model");
	this->uniformLightPos = glGetUniformLocation(this->programID, "lightPosition");
	this->uniformFarPlane = glGetUniformLocation(this->programID, "farPlane");

	std::string buffer{};

	for (int i = 0; i < 6; i++) {
		buffer = "shadowMatrices[" + std::to_string(i) + "]";

		this->uniformShadowMatrices[i] = glGetUniformLocation(this->programID, buffer.data());
	}
}