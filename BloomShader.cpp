#include "BloomShader.h"

BloomShader::BloomShader(std::string vertFileName, std::string fragFileName)
	: IShaderLoader(vertFileName, fragFileName, "") {
	this->readFile(this->vertFileName, this->vertexShader);
	this->readFile(this->fragFileName, this->fragmentShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID });
}

void BloomShader::getUniformLocations() {
	this->uniformHorizontalFlag = glGetUniformLocation(this->programID, "horizontal");

	std::string buffer{};

	for (int i = 0; i < 5; i++) {
		buffer = "weight[" + std::to_string(i) + "]";
		this->uniformWeights[i] = glGetUniformLocation(this->programID, buffer.data());
	}
}