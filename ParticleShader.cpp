#include "ParticleShader.h"

ParticleShader::ParticleShader(std::string vertFileName, std::string fragFileName, std::string geomFileName) 
	: IShaderLoader(vertFileName, fragFileName, geomFileName), uniformProjectionMatrix { 0 }, uniformColorID{ 0 },
	uniformNumberOfRows{ 0 } {

	this->readFile(this->vertFileName, this->vertexShader);
	this->readFile(this->fragFileName, this->fragmentShader);
	this->readFile(this->geomFileName, this->geometryShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);
	this->compileShader(this->geometryShaderID, this->geometryShader, GL_GEOMETRY_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID, this->geometryShaderID });
	glDeleteShader(geometryShaderID);
}

void ParticleShader::getUniformLocations() {
	this->uniformProjectionMatrix = glGetUniformLocation(this->programID, "projectionMatrix");
	this->uniformNumberOfRows = glGetUniformLocation(this->programID, "numberOfRows");
	this->uniformColorID = glGetUniformLocation(this->programID, "vColor");
}