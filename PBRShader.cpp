#include "PBRShader.h"

PBRShader::PBRShader(std::string vertFileName, std::string fragFileName, std::string geomFileName)
	: LightingShader(vertFileName, fragFileName, geomFileName)
{

}

void PBRShader::getUniformLocations() {
	this->uniformModel = glGetUniformLocation(this->programID, "model");
	this->uniformProjection = glGetUniformLocation(this->programID, "projection");
	this->uniformView = glGetUniformLocation(this->programID, "view");

	this->uniformColor = glGetUniformLocation(this->programID, "color");

	this->uniformDirectionalLight.uniformAmbientColor = glGetUniformLocation(this->programID,
		"directionalLight.base.color");
	this->uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(this->programID,
		"directionalLight.base.ambientIntensity");
	this->uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(this->programID,
		"directionalLight.base.diffuseIntensity");
	this->uniformDirectionalLight.uniformDirection = glGetUniformLocation(this->programID, "directionalLight.direction");

	this->uniformAlbedo = glGetUniformLocation(this->programID, "material.albedo");
	this->uniformMetallic = glGetUniformLocation(this->programID, "material.metallic");
	this->uniformRoughness = glGetUniformLocation(this->programID, "material.roughness");
	this->uniformAo = glGetUniformLocation(this->programID, "material.ao");

	this->uniformEyePosition = glGetUniformLocation(this->programID, "eyePosition");

	this->uniformTextureBool = glGetUniformLocation(this->programID, "useTexture");
	this->uniformUseNormalMap = glGetUniformLocation(this->programID, "useNormalMap");

	this->uniformDiffuseSampler = glGetUniformLocation(this->programID, "diffuseMap");
	this->uniformNormalSampler = glGetUniformLocation(this->programID, "normalMap");
	this->uniformDepthSampler = glGetUniformLocation(this->programID, "depthMap");

	this->uniformPointShadowSampler = glGetUniformLocation(this->programID, "pointShadowMap");
	this->uniformDirectionalShadowSampler = glGetUniformLocation(this->programID, "directionalShadowMap");
	this->uniformLightSpaceTransform = glGetUniformLocation(this->programID, "lightSpaceTransform");
	this->uniformCalcShadows = glGetUniformLocation(this->programID, "calcShadows");
	this->uniformFarPlane = glGetUniformLocation(this->programID, "farPlane");

	this->uniformHeightScale = glGetUniformLocation(this->programID, "height_scale");

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