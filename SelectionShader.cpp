#include "SelectionShader.h"

SelectionShader::SelectionShader(std::string vertFileName, std::string fragFileName)
	: IShaderLoader(vertFileName, fragFileName, ""), uniformModel{ 0 }, uniformProjection{ 0 }, uniformView{ 0 },
	uniformObjectIndex{ 0 }, uniformDrawIndex{ 0 }
{
	this->readFile(this->vertFileName, this->vertexShader);
	this->readFile(this->fragFileName, this->fragmentShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	this->attachShader({ this->vertexShaderID, this->fragmentShaderID });
}

void SelectionShader::getUniformLocations() {
	this->uniformModel = glGetUniformLocation(this->programID, "model");
	this->uniformProjection = glGetUniformLocation(this->programID, "projection");
	this->uniformView = glGetUniformLocation(this->programID, "view");
	this->uniformObjectIndex = glGetUniformLocation(this->programID, "objectIndex");
	this->uniformDrawIndex = glGetUniformLocation(this->programID, "drawIndex");
}
