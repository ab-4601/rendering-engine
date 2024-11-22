#version 450 core

const float PI = 3.14159265359;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

layout (location = 0) out vec4 fragColor;

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
uniform sampler2D directionalShadowMap;
uniform samplerCube pointShadowMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform bool useTexture;
uniform bool useNormalMap;
uniform bool useMaterialMap;
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
	float ggx2 = GeometrySchlickGGX(NdotV, material.roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, material.roughness);

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

vec4 calcPBRLighting(Light light, vec3 direction, vec3 normal, float metallic, float roughness, float ao) {
	vec3 N = normalize(normal);
	vec3 V = normalize(cameraPos - data_in.fragPos.xyz);

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, material.albedo, metallic);

	vec3 Lo = vec3(0.f);

	vec3 L = normalize(direction);
	vec3 H = normalize(V + L);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.f), F0);

	vec3 kS = F;
	vec3 kD = 1.f - kS;
	kD *= (1.f - metallic);

	vec3 numerator = NDF * G * F;
	float denominator = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f) + 0.0001f;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(N, L), 0.f);
	Lo = (kD / PI + specular) * light.color * NdotL;

	if(calcShadows) {
		float shadow = calculateDirectionalShadow();
		Lo = (1 - shadow) * Lo;
	}

	return vec4(Lo, 1.f);
}

vec4 calcDirectionalLights(vec3 normal, float metallic, float roughness, float ao) {
	vec3 direction = directionalLight.direction;
	return calcPBRLighting(directionalLight.base, direction, normal, metallic, roughness, ao);
}

vec4 calcPointLight(PointLight pointLight, vec3 normal, float metallic, float roughness, float ao) {
	vec3 direction = pointLight.position - vec3(data_in.fragPos);
	float dist = length(direction);

	vec4 currColor = calcPBRLighting(pointLight.base, direction, normal, metallic, roughness, ao);
	float attenuation = (pointLight.exponent * pow(dist, 2)) + 
						(pointLight.linear * dist) + pointLight.constant;;

	return currColor / attenuation;
}

vec4 calcSpotLight(SpotLight spotLight, vec3 normal, float metallic, float roughness, float ao) {
	vec3 direction = normalize(spotLight.base.position - vec3(data_in.fragPos));
	float theta = dot(direction, normalize(-spotLight.direction));
	
	vec4 spotLightColor = vec4(0.f, 0.f, 0.f, 1.f);

	if(theta > spotLight.edge) {
		spotLightColor = calcPointLight(spotLight.base, normal, metallic, roughness, ao);
		float clampVal = 1.f - ((1.f - theta) * (1.f / (1.f - spotLight.edge)));
		spotLightColor *= clampVal;
	}

	return spotLightColor;
}

vec4 calcPointLights(vec3 normal, float metallic, float roughness, float ao) {
	vec4 totalPointLightColor = vec4(0.f);

	for(int i = 0; i < pointLightCount; i++) {
		totalPointLightColor += calcPointLight(pointLights[i], normal, metallic, roughness, ao);
	}

	return totalPointLightColor;
}

vec4 calcSpotLights(vec3 normal, float metallic, float roughness, float ao) {
	vec4 totalSpotLightColor = vec4(0.f);

	for(int i = 0; i < spotLightCount; i++) {
		totalSpotLightColor += calcSpotLight(spotLights[i], normal, metallic, roughness, ao);
	}

	return totalSpotLightColor;
}

void main() {
    vec3 normal = data_in.normal;
	//vec2 texel = data_in.texel;
	float metallic = material.metallic;
	float roughness = material.roughness;
	float ao = material.ao;

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

	if(useMaterialMap) {
		//ao = texture(metallicMap, data_in.texel).r;
		ao = 1.f;
		roughness = texture(metallicMap, data_in.texel).g;
		metallic = texture(metallicMap, data_in.texel).b;
	}

	vec4 finalColor = calcDirectionalLights(normal, metallic, roughness, ao) + 
					  calcPointLights(normal, metallic, roughness, ao) + 
					  calcSpotLights(normal, metallic, roughness, ao);

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, material.albedo, metallic);

	vec3 N = normalize(data_in.normal);
	vec3 V = normalize(cameraPos - data_in.fragPos.xyz);

	vec3 R = reflect(-V, N);
	const float MAX_REFLECTION_LOD = 4.f;

	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.f), F0, roughness);	

	vec3 kS = F;
	vec3 kD = 1.f - kS;
	kD *= 1.f - metallic;

	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * material.albedo;

	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.f), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	finalColor += vec4(ambient, 0.f);

	if (!useTexture) {
		finalColor *= data_in.color;
	}
	else {
		vec4 texColor = texture(diffuseMap, data_in.texel);

		if(texColor.a < 0.1f)
			discard;

		finalColor *= texColor;
	}

	fragColor = finalColor;
}