#version 450 core

const float PI = 3.14159265359;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in GEOM_DATA {
	vec4 color;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
	vec4 lightSpaceFragPos;
} data_in;

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
	vec3 albedo;
	float metallic;
	float roughness;
	float ao;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D directionalShadowMap;
uniform samplerCube pointShadowMap;

uniform bool useTexture;
uniform bool useNormalMap;
uniform bool calcShadows;

uniform float farPlane;

uniform vec3 cameraPos;

//const float minLayers = 8.f;
//const float maxLayers = 32.f;

//vec2 parallaxMapping(vec2 texel, vec3 viewDir) {
	//float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.f, 0.f, 1.f), viewDir), 0.f));
	//float layerDepth = 1.f / numLayers;

	//float currentLayerDepth = 0.f;

	//vec2 P = viewDir.xy * height_scale;
	//vec2 deltaTexel = P / numLayers;

	//vec2 currTexel = data_in.texel;
	//float currDepth = texture(depthMap, currTexel).r;

	//while(currentLayerDepth < currDepth) {
		//currTexel -= deltaTexel;
		//currDepth = texture(depthMap, currTexel).r;
		//currentLayerDepth += layerDepth;
	//}

	//return currTexel;
//}

float DistributionGGX(float NdotR, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float denom = (NdotR * NdotR) * (a2 - 1.f) + 1.f;

	return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.f);
	float k = pow(r, 2) / 8.f;

	float numerator = NdotV;
	float denominator = NdotV * (1.f - k) + k;

	return numerator / denominator;
}

float fresnelSchlick(float theta, vec3 F0) {
	return length(F0 + (1.f - F0) * pow(clamp(1.f - theta, 0.f, 1.f), 5.f));
}

float rand(vec2 v){
    return fract(sin(dot(v, vec2(12.9898, 78.233))) * 43758.5453);
}

const vec2 poissonSamples[] = vec2[](
    vec2( 0.334,  0.334), vec2( 0.557,  0.334), vec2( 0.778,  0.334), vec2( 1.0,  0.334),
    vec2( 0.334,  0.557), vec2( 0.557,  0.557), vec2( 0.778,  0.557), vec2( 1.0,  0.557),
    vec2( 0.334,  0.778), vec2( 0.557,  0.778), vec2( 0.778,  0.778), vec2( 1.0,  0.778),
    vec2( 0.334,  1.0),   vec2( 0.557,  1.0),   vec2( 0.778,  1.0),   vec2( 1.0,  1.0),
    vec2( 0.25,   0.25),  vec2( 0.5,    0.25),  vec2( 0.75,   0.25),  vec2( 1.0,  0.25),
    vec2( 0.25,   0.5),   vec2( 0.5,    0.5),   vec2( 0.75,   0.5),   vec2( 1.0,  0.5),
    vec2( 0.25,   0.75),  vec2( 0.5,    0.75),  vec2( 0.75,   0.75),  vec2( 1.0,  0.75)
);

float calculateDirectionalShadow() {
	vec3 projectionCoords = data_in.lightSpaceFragPos.xyz / data_in.lightSpaceFragPos.w;
	projectionCoords = projectionCoords * 0.5f + 0.5f;

	float closestDepth = texture2D(directionalShadowMap, projectionCoords.xy).r;
	float currentDepth = projectionCoords.z;

	float shadow = 0.f;
	float shadowSamples = 28.f;
	float shadowThreshold = 0.6f;

	float shadowMapVal = texture(directionalShadowMap, projectionCoords.xy).r;
	float lightSpaceFragDepth = data_in.lightSpaceFragPos.z;

	// PCF with random sampling
	if(abs(lightSpaceFragDepth - shadowMapVal) < shadowThreshold) {
		for(float i = 0.f; i < shadowSamples; i++) {
			vec2 randomOffset = poissonSamples[int(i)] * 2.f - 1.f;
			randomOffset *= 0.001;

			float sampleDepth = texture(directionalShadowMap, projectionCoords.xy + randomOffset).r;

			shadow += currentDepth > sampleDepth ? 1.f : 0.f;
		}
	}

	shadow /= shadowSamples;

	return shadow;
}

vec4 calcPBRLighting(Light light, vec3 direction, vec3 normal) 
{
	normal = normalize(normal);
	direction = normalize(direction);
	vec3 viewDir = normalize(cameraPos - vec3(data_in.fragPos));
	vec3 F0 = vec3(0.f);
    F0 = mix(F0, material.albedo, material.metallic);

	vec3 R = normalize(viewDir + direction);

	// Diffuse BRDF (Lambert's cosine law)
	float NdotL = max(dot(normal, direction), 0.f);
	vec3 diffuse = (1.f - material.metallic) * (1.f / PI) * light.color * NdotL;

	// Specular BRDF (Cook-Torrance model)
	float NdotR = max(dot(normal, R), 0.f);
	float VdotR = max(dot(viewDir, R), 0.f);
	float F = fresnelSchlick(VdotR, F0);
	float D = DistributionGGX(NdotR, material.roughness);
	float G = GeometrySchlickGGX(NdotL, material.roughness) * GeometrySchlickGGX(VdotR, material.roughness);

	vec3 specular = vec3(F * D * G / (4.f * max(NdotL, 0.001f) * max(VdotR, 0.001f))) * light.color;

	vec4 lighting = vec4(diffuse + specular, 1.f);

	if(calcShadows == true) {
		float shadow = calculateDirectionalShadow();
		lighting = vec4((1.f - shadow) * (diffuse + specular), 1.f);
	}
	else
		lighting = vec4((diffuse + specular), 1.f);

	return lighting;
}

vec4 calcDirectionalLights(vec3 normal) {
	vec3 direction = directionalLight.direction - vec3(data_in.fragPos);
	return calcPBRLighting(directionalLight.base, direction, normal); 
}

vec4 calcPointLight(PointLight pointLight, vec3 normal) {
	vec3 direction = pointLight.position - vec3(data_in.fragPos);
	float dist = length(direction);

	vec4 currColor = calcPBRLighting(pointLight.base, direction, normal);
	float attenuation = (pointLight.exponent * pow(dist, 2)) + 
						(pointLight.linear * dist) + pointLight.constant;;

	return currColor / attenuation;
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

vec4 calcPointLights(vec3 normal) {
	vec4 totalPointLightColor = vec4(0.f);

	for(int i = 0; i < pointLightCount; i++) {
		totalPointLightColor += calcPointLight(pointLights[i], normal);
	}

	return totalPointLightColor;
}

vec4 calcSpotLights(vec3 normal) {
	vec4 totalSpotLightColor = vec4(0.f);

	for(int i = 0; i < spotLightCount; i++) {
		totalSpotLightColor += calcSpotLight(spotLights[i], normal);
	}

	return totalSpotLightColor;
}

void main() {
    vec3 normal = data_in.normal;
	//vec2 texel = data_in.texel;

	if(useNormalMap) {
		//texel = parallaxMapping(data_in.texel, eyePosition - vec3(data_in.fragPos));
		normal = texture(normalMap, data_in.texel).rgb;
		normal = normalize(normal * 2.f - 1.f);
		
		vec3 T = normalize(data_in.tangent);
		vec3 N = normalize(data_in.normal);

		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		normal = normalize(mat3(T, B, N) * normal);
	}

	vec4 finalColor = calcDirectionalLights(normal) + calcPointLights(normal) + calcSpotLights(normal);

	if (!useTexture) {
		finalColor *= data_in.color;
	}
	else {
		vec4 texColor = texture(diffuseMap, data_in.texel);

		if(texColor.a < 0.1)
			discard;

		finalColor *= texture(diffuseMap, data_in.texel);
	}

	finalColor = pow(finalColor, vec4(1 / 2.2f));
	fragColor = finalColor;

	float brightness = dot(finalColor.rgb, vec3(0.2125f, 0.7152f, 0.0722f));

	if(brightness > 1.f)
		brightColor = vec4(finalColor.rgb, 1.f);
	else
		brightColor = vec4(0.f, 0.f, 0.f, 1.f);
}