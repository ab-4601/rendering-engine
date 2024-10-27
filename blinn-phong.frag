#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in DATA {
	vec4 vColor;
    vec2 texCoord;
    vec3 normal;
    vec4 fragPos;
} data_in;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light {
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight  {
	Light base;
	vec3 direction;
};

struct PointLight {
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight {
	PointLight base;
	vec3 direction;
	float edge;
};

struct Material {
	float specularIntensity;
	float specularPower;
};

uniform bool useTexture = false;
uniform sampler2D textureUnit;

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

uniform vec3 eyePosition;

vec4 calcLightByDirection(Light light, vec3 direction) {
	vec4 ambientColor = vec4(light.color, 1.f) * light.ambientIntensity;

	float diffuseFactor = max(dot(normalize(data_in.normal), normalize(direction)), 0.f);
	vec4 diffuseColor = vec4(light.color, 1.f) * light.diffuseIntensity * diffuseFactor;

	vec4 specularColor = vec4(0.f, 0.f, 0.f, 1.f);

	// Calculate specular color (blinn-phong)
	if(diffuseFactor > 0.f) {
		vec3 fragToEye = normalize(eyePosition - vec3(data_in.fragPos));
		vec3 halfwayDir = normalize(normalize(direction) + fragToEye);
		float specularFactor = dot(normalize(data_in.normal), halfwayDir);

		//vec3 reflection = reflect(-normalize(direction), normalize(data_in.normal));
		//float specularFactor = dot(normalize(fragToEye), normalize(reflection));
		
		if(specularFactor > 0.f) {
			specularFactor = pow(specularFactor, material.specularPower);
			specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.f);
		}
	}

	return (ambientColor + diffuseColor + specularColor);
}

vec4 calcDirectionalLight() {
	vec3 direction = normalize(directionalLight.direction);
	return calcLightByDirection(directionalLight.base, direction);
}

vec4 calcPointLight(PointLight pointLight) {
	vec3 direction = pointLight.position - vec3(data_in.fragPos);
	float dist = length(direction);
	direction = normalize(direction);

	vec4 currColor = calcLightByDirection(pointLight.base, direction);
	float attenuation = (pointLight.exponent * pow(dist, 2)) + 
						(pointLight.linear * dist) + pointLight.constant;

	return currColor / attenuation;
}

vec4 calcPointLights() {
	vec4 totalPointLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	for(int i = 0; i < pointLightCount; i++)
		totalPointLightColor += calcPointLight(pointLights[i]);

	return totalPointLightColor;
}

vec4 calcSpotLight(SpotLight spotLight) {
	vec3 direction = normalize(spotLight.base.position - vec3(data_in.fragPos));
	float theta = dot(direction, normalize(-spotLight.direction));
	
	vec4 spotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	if(theta > spotLight.edge) {
		spotLightColor = calcPointLight(spotLight.base);
		float clampVal = 1.f - ((1.f - theta) * (1.f/(1.f - spotLight.edge)));
		spotLightColor *= clampVal;
	}

	return spotLightColor;
}

vec4 calcSpotLights() {
	vec4 finalSpotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	for(int i = 0; i < spotLightCount; i++) {
		finalSpotLightColor += calcSpotLight(spotLights[i]);
	}

	return finalSpotLightColor;
}

void main() {	
	vec4 finalColor = calcDirectionalLight() + calcPointLights() + calcSpotLights();

	if (!useTexture) {
		finalColor *= data_in.vColor;
	}
	else {
		finalColor *= texture(textureUnit, data_in.texCoord);
	}

	fragColor = finalColor;

	float brightness = dot(finalColor.rgb, vec3(0.2125f, 0.7152f, 0.0722f));

	if(brightness > 1.f)
		brightColor = vec4(finalColor.rgb, 1.f);
	else
		brightColor = vec4(0.f, 0.f, 0.f, 1.f);
}