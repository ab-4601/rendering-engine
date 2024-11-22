#pragma once

#include "ShaderLoader.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class PBRShader : public IShaderLoader {
private:
	GLuint uniformModel{ 0 }, uniformProjection{ 0 }, uniformColor{ 0 }, uniformView{ 0 };
	GLuint uniformEyePosition{ 0 }, uniformAlbedo{ 0 }, uniformMetallic{ 0 }, uniformRoughness{ 0 }, uniformAo{ 0 };
	GLuint uniformTextureBool{ 0 }, uniformUseNormalMap{ 0 }, uniformUseMaterialMap{ 0 };
	GLuint uniformIrradianceSampler{ 0 }, uniformBRDFSampler{ 0 }, uniformPrefilterSampler{ 0 };
	GLuint uniformDiffuseSampler{ 0 }, uniformNormalSampler{ 0 }, uniformMetallicSampler{ 0 }, uniformRoughnessSampler{ 0 };
	GLuint uniformHeightScale{ 0 }, uniformDepthSampler{ 0 }, uniformLightSpaceTransform{ 0 }, uniformCalcShadows{ 0 };
	GLuint uniformPointShadowSampler{ 0 }, uniformDirectionalShadowSampler{ 0 }, uniformFarPlane{ 0 };

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
	PBRShader(
		std::string vertFileName = "PBR.vert",
		std::string fragFileName = "PBR.frag",
		std::string geomFileName = "PBR.geom"
	);

	virtual void getUniformLocations() override;

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
	inline GLuint getEyePosition() const { return this->uniformEyePosition; }
	inline GLuint getUniformTextureBool() const { return this->uniformTextureBool; }
	inline GLuint getUniformNormalMapBool() const { return this->uniformUseNormalMap; }
	inline GLuint getUniformUseMaterialMap() const { return this->uniformUseMaterialMap; }
	inline GLuint getUniformDiffuseSampler() const { return this->uniformDiffuseSampler; }
	inline GLuint getUniformNormalSampler() const { return this->uniformNormalSampler; }
	inline GLuint getUniformDepthSampler() const { return this->uniformDepthSampler; }
	inline GLuint getUniformMetallicSampler() const { return this->uniformMetallicSampler; }
	inline GLuint getUniformRoughnessSampler() const { return this->uniformRoughnessSampler; }
	inline GLuint getUniformIrradianceSampler() const { return this->uniformIrradianceSampler; }
	inline GLuint getUniformBRDFSampler() const { return this->uniformBRDFSampler; }
	inline GLuint getUniformPrefilterSampler() const { return this->uniformPrefilterSampler; }
	inline GLuint getUniformHeightScale() const { return this->uniformHeightScale; }
	inline GLuint getUniformPointShadowSampler() const { return this->uniformPointShadowSampler; }
	inline GLuint getUniformFarPlane() const { return this->uniformFarPlane; }
	inline GLuint getUniformDirectionalShadowSampler() const { return this->uniformDirectionalShadowSampler; }
	inline GLuint getUniformLightSpaceTransform() const { return this->uniformLightSpaceTransform; }
	inline GLuint getUniformCalcShadows() const { return this->uniformCalcShadows; }

	inline GLuint getUniformAlbedo() const { return this->uniformAlbedo; }
	inline GLuint getUniformMetallic() const { return this->uniformMetallic; }
	inline GLuint getUniformRoughness() const { return this->uniformRoughness; }
	inline GLuint getUniformAo() const { return this->uniformAo; }

	~PBRShader() = default;
};

