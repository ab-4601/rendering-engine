#pragma once

#include "Core.h"
#include "ShaderLoader.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class LightingShader : public IShaderLoader {
private:
	GLuint uniformModel, uniformProjection, uniformColor, uniformView;
	GLuint uniformSpecularIntensity, uniformSpecularPower, uniformEyePosition;
	GLuint uniformTextureBool;

	GLuint uniformPointLightCount;

	struct {
		GLuint uniformAmbientColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[MAX_POINT_LIGHTS];

	GLuint uniformSpotLightCount;

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformDirection;
		GLuint uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

public:
	LightingShader(std::string vertFileName = "blinn-phong.vert", 
		std::string fragFileName = "blinn-phong.frag", std::string geomFileName = "blinn-phong.geom");

	void setDirectionalLight(DirectionalLight* directionalLight);
	void setPointLights(PointLight* pointLights, unsigned int lightCount);
	void setSpotLights(SpotLight* spotLights, unsigned int lightCount);

	inline GLuint getUniformModel() const { return this->uniformModel; }
	inline GLuint getUniformProjection() const { return this->uniformProjection; }
	inline GLuint getUniformColor() const { return this->uniformColor; }
	inline GLuint getUniformView() const { return this->uniformView; }
	inline GLuint getAmbientIntensityLocation() const { return this->uniformDirectionalLight.uniformAmbientIntensity; }
	inline GLuint getAmbientColorLocation() const { return this->uniformDirectionalLight.uniformAmbientColor; }
	inline GLuint getDiffuseIntensityLocation() const { return this->uniformDirectionalLight.uniformDiffuseIntensity; }
	inline GLuint getDirectionLocation() const { return this->uniformDirectionalLight.uniformDirection; }
	inline GLuint getSpecularIntensity() const { return this->uniformSpecularIntensity; }
	inline GLuint getSpecularPower() const { return this->uniformSpecularPower; }
	inline GLuint getEyePosition() const { return this->uniformEyePosition; }
	inline GLuint getUniformTextureBool() const { return this->uniformTextureBool; }

	virtual inline void getUniformLocations() override;

	~LightingShader() = default;
};