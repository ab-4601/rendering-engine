#pragma once

#include "Core.h"
#include "ShaderLoader.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class LightingShader : public IShaderLoader {
private:
	GLuint uniformModel{ 0 }, uniformProjection{ 0 }, uniformColor{ 0 }, uniformView{ 0 };
	GLuint uniformSpecularIntensity{ 0 }, uniformSpecularPower{ 0 }, uniformEyePosition{ 0 };
	GLuint uniformTextureBool{ 0 }, uniformNormalMap{ 0 }, uniformTextureSampler{ 0 }, uniformNormalSampler{ 0 };

	GLuint uniformPointLightCount = 0;

	struct {
		GLuint uniformAmbientColor = 0;
		GLuint uniformAmbientIntensity = 0;
		GLuint uniformDiffuseIntensity = 0;

		GLuint uniformDirection = 0;
	} uniformDirectionalLight;

	struct {
		GLuint uniformColor = 0;
		GLuint uniformAmbientIntensity = 0;
		GLuint uniformDiffuseIntensity = 0;

		GLuint uniformPosition = 0;
		GLuint uniformConstant = 0;
		GLuint uniformLinear = 0;
		GLuint uniformExponent = 0;
	} uniformPointLight[MAX_POINT_LIGHTS];

	GLuint uniformSpotLightCount = 0;

	struct {
		GLuint uniformColor = 0;
		GLuint uniformAmbientIntensity = 0;
		GLuint uniformDiffuseIntensity = 0;

		GLuint uniformPosition = 0;
		GLuint uniformConstant = 0;
		GLuint uniformLinear = 0;
		GLuint uniformExponent = 0;

		GLuint uniformDirection = 0;
		GLuint uniformEdge = 0;
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
	inline GLuint getUniformNormalMap() const { return this->uniformNormalMap; }
	inline GLuint getUniformTextureSampler() const { return this->uniformTextureSampler; }
	inline GLuint getUniformNormalSampler() const { return this->uniformNormalSampler; }

	virtual inline void getUniformLocations() override;

	~LightingShader() = default;
};