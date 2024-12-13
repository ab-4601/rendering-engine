#version 450 core

#define PI 3.1415926535897932384626433832795

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;
const int MAX_CASCADES = 16;

out vec4 fragColor;

in GEOM_DATA {
	vec4 color;
    vec2 texel;
    vec3 normal;
	vec3 tangent;
    vec4 fragPos;
} data_in;

struct Light {
	vec3 color;
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
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform samplerCube pointShadowMap;
uniform sampler2DArray cascadedShadowMap;
uniform sampler3D randomOffsets;

uniform bool useTexture;
uniform bool useNormalMap;
uniform bool strippedNormalMap;
uniform bool useMaterialMap;
uniform bool calcShadows;
uniform bool enableSSAO;

uniform float farPlane;
uniform float nearPlane;
uniform int cascadeCount;
uniform float cascadePlanes[MAX_CASCADES];
uniform float radius;
uniform vec3 offsetTexSize;

uniform vec3 cameraPos;

layout (std140, binding = 0) uniform cameraSpaceMatrix {
	mat4 projection;
	mat4 view;
};

layout (std140, binding = 1) uniform LightSpaceMatrices {
	mat4 lightSpaceMatrices[16];
};

//const float minLayers = 8.f;
//const float maxLayers = 32.f;

//vec2 parallaxMapping(vec2 texel, vec3 viewDir) {
	//float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.f, 0.f, 1.f), viewDir), 0.f));
	//float layerDepth = 1.f / numLayers;

	//float currentLayerDepth = 0.f;

	//vec2 P = viewDir.xy * height_scale;
	//vec2 deltaTexel = P / numLayers;

	//vec2 currTexel = data_in.texel;
	//float currDepth = texture2D(depthMap, currTexel).r;

	//while(currentLayerDepth < currDepth) {
		//currTexel -= deltaTexel;
		//currDepth = texture2D(depthMap, currTexel).r;
		//currentLayerDepth += layerDepth;
	//}

	//return currTexel;
//}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.f);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float denominator = (NdotH2 * (a2 - 1.f) + 1.f);
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.f);
	float k = (r * r) / 8.f;

	float numerator = NdotV;
	float denominator = NdotV * (1.f - k) + k;

	return numerator / denominator;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.f);
	float NdotL = max(dot(N, L), 0.f);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float theta, vec3 F0) {
	return F0 + (1.f - F0) * pow(clamp(1.f - theta, 0.f, 1.f), 5.f);
}

vec3 fresnelSchlickRoughness(float theta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.f - roughness), F0) - F0) * pow(clamp(1.f - theta, 0.f, 1.f), 5.f);
}

float rand(vec2 v){
    return fract(sin(dot(v, vec2(12.9898, 78.233))) * 43758.5453);
}

float calculateShadows() {
	vec4 fragPosViewSpace = view * data_in.fragPos;
	float depthVal = abs(fragPosViewSpace.z);

	int layer = -1;
	for(int i = 0; i < cascadeCount; i++) {
		if(depthVal < cascadePlanes[i]) {
			layer = i;
			break;
		}
	}

	if(layer == -1)
		layer = cascadeCount;

	vec4 lightSpaceFragPos = lightSpaceMatrices[layer] * data_in.fragPos;
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
	projCoords = projCoords * 0.5f + 0.5f;

	float currentDepth = projCoords.z;

	if(currentDepth > 1.f)
		return 1.f;

	ivec3 offsetCoord;
	offsetCoord.xy = ivec2(mod(gl_FragCoord.xy, offsetTexSize.xy));

	float sum = 0.f;
	int samplesDiv2 = int(offsetTexSize.z);
	vec4 sc = lightSpaceFragPos;

	for(int i = 0; i < 4; i++) {
		offsetCoord.z = i;
		vec4 offsets = texelFetch(randomOffsets, offsetCoord, 0) * radius * lightSpaceFragPos.w;

		sc.xy = lightSpaceFragPos.xy + offsets.xy;
		sum += texture(cascadedShadowMap, vec3(sc.xy / sc.w, layer)).r;

		sc.xy = lightSpaceFragPos.xy + offsets.zw;
		sum += texture(cascadedShadowMap, vec3(sc.xy / sc.w, layer)).r;
	}

	float shadowFactor = sum / 8.f;

	if(shadowFactor != 1.f && shadowFactor != 0.f) {
		for(int i = 4; i < samplesDiv2; i++) {
			offsetCoord.z = i;
			vec4 offsets = texelFetch(randomOffsets, offsetCoord, 0) * radius * lightSpaceFragPos.w;

			sc.xy = lightSpaceFragPos.xy + offsets.xy;
			sum += texture(cascadedShadowMap, vec3(sc.xy / sc.w, layer)).r;

			sc.xy = lightSpaceFragPos.xy + offsets.zw;
			sum += texture(cascadedShadowMap, vec3(sc.xy / sc.w, layer)).r;
		}

		shadowFactor = sum / float(samplesDiv2 * 2.f);
	}

	return shadowFactor;
}

float calcCSMShadows() {
	vec4 fragPosViewSpace = view * data_in.fragPos;
	float depthVal = abs(fragPosViewSpace.z);

	int layer = -1;
	for(int i = 0; i < cascadeCount; i++) {
		if(depthVal < cascadePlanes[i]) {
			layer = i;
			break;
		}
	}

	if(layer == -1)
		layer = cascadeCount;

	vec4 lightSpaceFragPos = lightSpaceMatrices[layer] * data_in.fragPos;
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
	projCoords = projCoords * 0.5f + 0.5f;

	float currentDepth = projCoords.z;

	if(currentDepth > 1.f)
		return 1.f;

	float shadowFactor = 0.f;
	float shadowSamples = 16.f;
	
	vec2 texelSize = 3.f / vec2(textureSize(cascadedShadowMap, 0));

	for(float i = 0.f; i < shadowSamples; i++) {
		vec2 randomOffset = vec2(
			rand(projCoords.xy + i) - 0.5f,
			rand(projCoords.xy + i * 0.5f) - 0.5f
		);

		randomOffset *= texelSize;

		float sampleDepth = texture(cascadedShadowMap, vec3(randomOffset + projCoords.xy, layer)).r;

		shadowFactor += currentDepth > sampleDepth ? 1.f : 0.f;
	}

	shadowFactor /= shadowSamples;

	return shadowFactor;
}

vec4 calculateLighting(Light light, vec3 direction, vec3 albedo, vec3 normal, float metallic, float roughness) {
	vec3 N = normal;
	vec3 V = normalize(cameraPos - vec3(data_in.fragPos));
	vec3 L = normalize(direction);
	vec3 H = normalize(V + L);

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.f);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlickRoughness(max(dot(H, V), 0.f), F0, roughness);

	vec3 kS = F;
	vec3 kD = 1.f - kS;
	kD *= (1.f - metallic);

	vec3 numerator = NDF * G * F;
	float denominator = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f) + 0.0001f;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(N, L), 0.f);
	Lo = (kD * albedo / PI + specular) * light.color * NdotL;

	if(calcShadows) {
		float shadow = calcCSMShadows();
		Lo = (1 - shadow) * Lo;
	}

	return vec4(Lo, 1.f);
}

float linearizeDepth(float depth) {
	return (2.f * nearPlane * farPlane) / (farPlane + nearPlane - (depth * 2.f - 1.f) * (farPlane - nearPlane));
}

vec4 calcDirectionalLights(vec3 albedo, vec3 normal, float metallic, float roughness) {
	return calculateLighting(directionalLight.base, directionalLight.direction, albedo, normal, metallic, roughness);
}

vec4 calcPointLight(PointLight pointLight, vec3 albedo, vec3 normal, float metallic, float roughness) {
	vec3 direction = pointLight.position - vec3(data_in.fragPos);
	float dist = length(direction);

	vec4 pointLightColor = calculateLighting(pointLight.base, direction, albedo, normal, metallic, roughness);
	float attenuation = (pointLight.exponent * pow(dist, 2)) + 
						(pointLight.linear * dist) + pointLight.constant;

	return pointLightColor / attenuation;
}

vec4 calcSpotLight(SpotLight spotLight, vec3 albedo, vec3 normal, float metallic, float roughness) {
	vec3 direction = normalize(spotLight.base.position - vec3(data_in.fragPos));
	float theta = dot(direction, normalize(-spotLight.direction));
	
	vec4 spotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	if(theta > spotLight.edge) {
		spotLightColor = calcPointLight(spotLight.base, albedo, normal, metallic, roughness);
		float clampVal = 1.f - ((1.f - theta) * (1.f / (1.f - spotLight.edge)));
		spotLightColor *= clampVal;
	}

	return spotLightColor;
}

vec4 calcPointLights(vec3 albedo, vec3 normal, float metallic, float roughness) {
	vec4 totalPointLightColor = vec4(0.f);

	for(int i = 0; i < pointLightCount; i++) {
		totalPointLightColor += calcPointLight(pointLights[i], albedo, normal, metallic, roughness);
	}

	return totalPointLightColor;
}

vec4 calcSpotLights(vec3 albedo, vec3 normal, float metallic, float roughness) {
	vec4 totalSpotLightColor = vec4(0.f);

	for(int i = 0; i < spotLightCount; i++) {
		totalSpotLightColor += calcSpotLight(spotLights[i], albedo, normal, metallic, roughness);
	}

	return totalSpotLightColor;
}

void main() {
    vec3 normal = normalize(data_in.normal);
	//vec2 texel = data_in.texel;
	vec3 albedo = material.albedo;
	float metallic = material.metallic;
	float roughness = material.roughness;
	float ao = material.ao;

	if(useTexture) {
		vec4 texColor = texture2D(diffuseMap, data_in.texel);
		if(texColor.a < 0.1f)
			discard;

		albedo = texColor.rgb;
	}

	if(useNormalMap) {
		//texel = parallaxMapping(data_in.texel, eyePosition - vec3(data_in.fragPos));
		normal = texture2D(normalMap, data_in.texel).rgb;

		if(!strippedNormalMap)
			normal = normalize(normal * 2.f - 1.f);
		else
			normal = normalize(1.f - normal * 2.f);
		
		vec3 T = normalize(data_in.tangent);
		vec3 N = normalize(data_in.normal);

		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		normal = normalize(mat3(T, B, N) * normal);
	}

	if(useMaterialMap) {
		//ao = texture2D(metallicMap, data_in.texel).r;
		ao = 0.3f;
		roughness = texture2D(metallicMap, data_in.texel).g;
		metallic = texture2D(metallicMap, data_in.texel).b;
	}

	vec4 finalColor = calcDirectionalLights(albedo, normal, metallic, roughness) +
					  calcPointLights(albedo, normal, metallic, roughness) + 
					  calcSpotLights(albedo, normal, metallic, roughness);

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);

	vec3 N = normal;
	vec3 V = normalize(cameraPos - vec3(data_in.fragPos));

	vec3 R = reflect(-V, N);
	const float MAX_REFLECTION_LOD = 9.f;

	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.f), F0, roughness);

	vec3 kS = F;
	vec3 kD = 1.f - kS;
	kD *= 1.f - metallic;

	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;

	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture2D(brdfLUT, vec2(max(dot(N, V), 0.f), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	finalColor += vec4(ambient, 0.f);

	fragColor = finalColor;
}