#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in DATA {
	vec4 vColor;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
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
uniform bool useNormalMap = false;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

uniform vec3 eyePosition;
uniform float height_scale;

const float minLayers = 8.f;
const float maxLayers = 32.f;

vec2 parallaxMapping(vec2 texel, vec3 viewDir) {
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.f, 0.f, 1.f), viewDir), 0.f));
	float layerDepth = 1.f / numLayers;

	float currentLayerDepth = 0.f;

	vec2 P = viewDir.xy * height_scale;
	vec2 deltaTexel = P / numLayers;

	vec2 currTexel = data_in.texel;
	float currDepth = texture(depthMap, currTexel).r;

	while(currentLayerDepth < currDepth) {
		currTexel -= deltaTexel;
		currDepth = texture(depthMap, currTexel).r;
		currentLayerDepth += layerDepth;
	}

	return currTexel;
}

vec4 calcLightByDirection(Light light, vec3 direction, vec3 normal) {
	vec4 ambientColor = vec4(light.color, 1.f) * light.ambientIntensity;

	float diffuseFactor = max(dot(normalize(normal), normalize(direction)), 0.f);
	vec4 diffuseColor = vec4(light.color, 1.f) * light.diffuseIntensity * diffuseFactor;

	vec4 specularColor = vec4(0.f, 0.f, 0.f, 1.f);

	// Calculate specular color (blinn-phong)
	if(diffuseFactor > 0.f) {
		vec3 fragToEye = vec3(0.f);
		vec3 halfwayDir = vec3(0.f);

		fragToEye = normalize(eyePosition - vec3(data_in.fragPos));
		halfwayDir = normalize(normalize(direction) + fragToEye);
		float specularFactor = dot(normalize(normal), halfwayDir);

		//vec3 reflection = reflect(-normalize(direction), normalize(normal));
		//float specularFactor = dot(normalize(fragToEye), normalize(reflection));
		
		if(specularFactor > 0.f) {
			specularFactor = pow(specularFactor, material.specularPower);
			specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.f);
		}
	}

	return (ambientColor + diffuseColor + specularColor);
}

vec4 calcDirectionalLight(vec3 normal) {
	vec3 direction = normalize(directionalLight.direction);
	return calcLightByDirection(directionalLight.base, direction, normal);
}

vec4 calcPointLight(PointLight pointLight, vec3 normal) {
	vec3 direction = pointLight.position - vec3(data_in.fragPos);
	float dist = length(direction);
	direction = normalize(direction);

	vec4 currColor = calcLightByDirection(pointLight.base, direction, normal);
	float attenuation = (pointLight.exponent * pow(dist, 2)) + 
						(pointLight.linear * dist) + pointLight.constant;

	return currColor / attenuation;
}

vec4 calcPointLights(vec3 normal) {
	vec4 totalPointLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	for(int i = 0; i < pointLightCount; i++)
		totalPointLightColor += calcPointLight(pointLights[i], normal);

	return totalPointLightColor;
}

vec4 calcSpotLight(SpotLight spotLight, vec3 normal) {
	vec3 direction = normalize(spotLight.base.position - vec3(data_in.fragPos));
	float theta = dot(direction, normalize(-spotLight.direction));
	
	vec4 spotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	if(theta > spotLight.edge) {
		spotLightColor = calcPointLight(spotLight.base, normal);
		float clampVal = 1.f - ((1.f - theta) * (1.f/(1.f - spotLight.edge)));
		spotLightColor *= clampVal;
	}

	return spotLightColor;
}

vec4 calcSpotLights(vec3 normal) {
	vec4 finalSpotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	for(int i = 0; i < spotLightCount; i++) {
		finalSpotLightColor += calcSpotLight(spotLights[i], normal);
	}

	return finalSpotLightColor;
}

void main() {
	vec3 normal;

	if(useNormalMap) {
		normal = texture(normalMap, data_in.texel).rgb;
		normal = normalize(normal * 2.f - 1.f);
		
		vec3 T = normalize(data_in.tangent);
		vec3 N = normalize(data_in.normal);

		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		normal = normalize(mat3(T, B, N) * normal);
	}
	else
		normal = data_in.normal;

	vec4 finalColor = calcDirectionalLight(normal) + calcPointLights(normal) + calcSpotLights(normal);

	if (!useTexture) {
		finalColor *= data_in.vColor;
	}
	else {
		vec4 texColor = texture(diffuseMap, data_in.texel);

		if(texColor.a < 0.1)
			discard;

		finalColor *= texture(diffuseMap, data_in.texel);
	}

	fragColor = finalColor;

	float brightness = dot(finalColor.rgb, vec3(0.2125f, 0.7152f, 0.0722f));

	if(brightness > 1.f)
		brightColor = vec4(finalColor.rgb, 1.f);
	else
		brightColor = vec4(0.f, 0.f, 0.f, 1.f);
}