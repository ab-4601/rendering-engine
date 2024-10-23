#include "LightingShader.h"

LightingShader::LightingShader(std::string vertFileName, std::string fragFileName, std::string geomFileName)
	:IShaderLoader(vertFileName, fragFileName, geomFileName), 
	uniformModel{ 0 }, uniformProjection{ 0 }, uniformColor{ 0 }, uniformView{ 0 },
	uniformEyePosition{ 0 }, uniformTextureBool{ 0 },
	uniformPointLightCount{ 0 }, uniformPointLight{ 0 }, uniformSpotLight{ 0 }, uniformSpotLightCount{ 0 },
	uniformDirectionalLight{ 0 }, uniformSpecularIntensity{ 0 }, uniformSpecularPower{ 0 } {

	uniformDirectionalLight.uniformAmbientColor = 0;
	uniformDirectionalLight.uniformAmbientIntensity = 0;
	uniformDirectionalLight.uniformDiffuseIntensity = 0;
	uniformDirectionalLight.uniformDirection = 0;

	this->readFile(this->vertFileName, this->vertexShader);
	this->readFile(this->fragFileName, this->fragmentShader);

	this->compileShader(this->vertexShaderID, this->vertexShader, GL_VERTEX_SHADER);
	this->compileShader(this->fragmentShaderID, this->fragmentShader, GL_FRAGMENT_SHADER);

	if (this->geomFileName != "") {
		this->readFile(this->geomFileName, this->geometryShader);
		this->compileShader(this->geometryShaderID, this->geometryShader, GL_GEOMETRY_SHADER);
		this->attachShader({ vertexShaderID, fragmentShaderID, geometryShaderID });
		glDeleteShader(this->geometryShaderID);
	}
	else {
		this->attachShader({ vertexShaderID, fragmentShaderID });
	}

	glDeleteShader(this->vertexShaderID);
	glDeleteShader(this->fragmentShaderID);

}

void LightingShader::getUniformLocations() {
	this->uniformModel = glGetUniformLocation(this->programID, "model");
	this->uniformProjection = glGetUniformLocation(this->programID, "projection");
	this->uniformColor = glGetUniformLocation(this->programID, "color");
	this->uniformView = glGetUniformLocation(this->programID, "view");
	this->uniformDirectionalLight.uniformAmbientColor = glGetUniformLocation(this->programID,
		"directionalLight.base.color");
	this->uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(this->programID,
		"directionalLight.base.ambientIntensity");
	this->uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(this->programID,
		"directionalLight.base.diffuseIntensity");
	this->uniformDirectionalLight.uniformDirection = glGetUniformLocation(this->programID, "directionalLight.direction");
	this->uniformSpecularIntensity = glGetUniformLocation(this->programID, "material.specularIntensity");
	this->uniformSpecularPower = glGetUniformLocation(this->programID, "material.specularPower");
	this->uniformEyePosition = glGetUniformLocation(this->programID, "eyePosition");
	this->uniformTextureBool = glGetUniformLocation(this->programID, "useTexture");

	this->uniformPointLightCount = glGetUniformLocation(this->programID, "pointLightCount");
	this->uniformSpotLightCount = glGetUniformLocation(this->programID, "spotLightCount");

	std::string buffer{};

	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		buffer = "pointLights[" + std::to_string(i) + "].base.color";
		uniformPointLight[i].uniformColor = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].base.ambientIntensity";
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].base.diffuseIntensity";
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].position";
		uniformPointLight[i].uniformPosition = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].constant";
		uniformPointLight[i].uniformConstant = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].linear";
		uniformPointLight[i].uniformLinear = glGetUniformLocation(this->programID, buffer.data());

		buffer = "pointLights[" + std::to_string(i) + "].exponent";
		uniformPointLight[i].uniformExponent = glGetUniformLocation(this->programID, buffer.data());
	}

	buffer.clear();

	for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
		buffer = "spotLights[" + std::to_string(i) + "].base.base.color";
		uniformSpotLight[i].uniformColor = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.base.ambientIntensity";
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.base.diffuseIntensity";
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.position";
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.constant";
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.linear";
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].base.exponent";
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].direction";
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(this->programID, buffer.data());

		buffer = "spotLights[" + std::to_string(i) + "].edge";
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(this->programID, buffer.data());
	}
}

void LightingShader::setDirectionalLight(DirectionalLight* directionalLight) {
	directionalLight->useLight(uniformDirectionalLight.uniformAmbientIntensity,
		uniformDirectionalLight.uniformAmbientColor, uniformDirectionalLight.uniformDiffuseIntensity,
		uniformDirectionalLight.uniformDirection);
}

void LightingShader::setPointLights(PointLight* pointLights, unsigned int lightCount) {
	if (lightCount > MAX_POINT_LIGHTS)
		lightCount = MAX_POINT_LIGHTS;

	glUniform1i(this->uniformPointLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++) {
		pointLights[i].useLight(uniformPointLight[i].uniformAmbientIntensity,
			uniformPointLight[i].uniformColor, uniformPointLight[i].uniformDiffuseIntensity,
			uniformPointLight[i].uniformPosition, uniformPointLight[i].uniformConstant,
			uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);
	}
}

void LightingShader::setSpotLights(SpotLight* spotLights, unsigned int lightCount) {
	if (lightCount > MAX_SPOT_LIGHTS)
		lightCount = MAX_POINT_LIGHTS;

	glUniform1i(this->uniformSpotLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++) {
		spotLights[i].useLight(uniformSpotLight[i].uniformAmbientIntensity,
			uniformSpotLight[i].uniformColor, uniformSpotLight[i].uniformDiffuseIntensity,
			uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformConstant,
			uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent,
			uniformSpotLight[i].uniformDirection, uniformSpotLight[i].uniformEdge);
	}
}