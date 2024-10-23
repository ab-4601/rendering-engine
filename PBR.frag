#version 450 core

const float PI = 3.14159265359;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

out vec4 fragColor;

in DATA {
	vec4 vColor;
    vec2 texCoord;
    smooth vec3 normal;
    vec3 fragPos;
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

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

uniform vec3 cameraPos;

float DistributionGGX(vec3 normal, vec3 halfway, float roughness) {
	float a = pow(roughness, 2);
	float a2 = pow(a, 2);
	float NdotH = max(dot(normal, halfway), 0.f);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float denominator = (NdotH2 * (a2 - 1.f) + 1.f);
	denominator = PI * pow(denominator, 2);

	return numerator / denominator;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.f);
	float k = pow(r, 2) / 8.f;

	float numerator = NdotV;
	float denominator = NdotV * (1.f - k) + k;

	return numerator / denominator;
}

float GeometrySmith(vec3 normal, vec3 fragToEye, vec3 direction, float roughness) {
	float NdotV = max(dot(normal, fragToEye), 0.f);
	float NdotL = max(dot(normal, direction), 0.f);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float theta, vec3 F0) {
	return F0 + (1.f - F0) * pow(clamp(1.f - theta, 0.f, 1.f), 5.f);
}

vec3 calcSpotLights() {
	vec3 Lo = vec3(0.f);

	return Lo;
}

vec3 calcPointLights(vec3 fragToEye, vec3 F0) {
	vec3 Lo = vec3(0.f);

	for(int i = 0; i < MAX_POINT_LIGHTS; i++) {
        vec3 direction = normalize(pointLights[i].position - fragToEye);
		vec3 halfway = normalize(fragToEye + direction);

		float distanceFromFrag = length(direction);
		float attenuation = 1.f / (pow(distanceFromFrag, 2));

		vec3 radiance = pointLights[i].base.color * attenuation;

		// cook-torrence BRDF
		float NDF = DistributionGGX(data_in.normal, halfway, material.roughness);
		float G = GeometrySmith(data_in.normal, fragToEye, direction, material.roughness);
		vec3 F = fresnelSchlick(max(dot(halfway, fragToEye), 0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.f) - kS;

		kD *= 1.f - material.metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.f * max(dot(data_in.normal, fragToEye), 0.f) * 
			max(dot(data_in.normal, direction), 0.f) + 0.0001;

		vec3 specular = numerator / denominator;

		float NdotL = max(dot(data_in.normal, direction), 0.f);
		Lo += (kD * material.albedo / PI * specular) * radiance * NdotL;
    }

	return Lo;
}

void main() {
    vec3 normal = normalize(data_in.normal);
    vec3 fragToEye = normalize(cameraPos - data_in.fragPos);

    vec3 F0 = vec3(0.f);
    F0 = mix(F0, material.albedo, material.metallic);

    vec3 Lo = vec3(0.f);

    Lo += calcPointLights(fragToEye, F0);

	vec3 ambient = vec3(0.03) * material.albedo * material.ao;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.f));

	fragColor = vec4(color, 1.f);
}