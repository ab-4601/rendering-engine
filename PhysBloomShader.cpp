#include "PhysBloomShader.h"

PhysBloomShader::PhysBloomShader(std::string vertFileName, std::string fragFileName, bool isUpsample)
	: IShaderLoader(vertFileName, fragFileName, ""), upsample{ isUpsample }
{
	this->readFile(this->vertFileName, this->vertexShader);
	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);

	this->readFile(this->fragFileName, this->fragmentShader);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID });
}

void PhysBloomShader::getUniformLocations() {
	if(this->upsample)
		this->uniformFilterRadius = glGetUniformLocation(this->programID, "filterRadius");

	this->uniformSrcTexture = glGetUniformLocation(this->programID, "srcTexture");

	if(!this->upsample) {
		this->uniformSrcResolution = glGetUniformLocation(this->programID, "srcResolution");
		this->uniformMipLevel = glGetUniformLocation(this->programID, "mipLevel");
	}
}