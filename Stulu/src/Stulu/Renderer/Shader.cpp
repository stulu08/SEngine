#include "st_pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Stulu{
	Ref<Shader> Shader::create(const std::string& path) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(path);
		case RenderAPI::API::Vulkan:
			return std::make_shared<VulkanShader>(path);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Shader creation")
			return nullptr;
	}
	Ref<Shader> Shader::create(const std::string& name, const std::string& src) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, src);
		case RenderAPI::API::Vulkan:
			return std::make_shared<VulkanShader>(name,src);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Shader creation")
			return nullptr;
	}
	Ref<Shader> Shader::create(const std::string& name, const std::string& vertex, const std::string& fragment) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertex, fragment);
		case RenderAPI::API::Vulkan:
			return std::make_shared<VulkanShader>(name, vertex, fragment);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in Shader creation")
			return nullptr;
	}
	void ShaderLibary::add(const std::string& name, const Ref<Shader>& shader) {
		CORE_ASSERT(!exists(name), "{0} Shader already exists: {1}", name);
		m_shaders[name] = shader;
	}
	bool ShaderLibary::exists(const std::string& name)
	{
		return m_shaders.find(name) != m_shaders.end();
	}
	void ShaderLibary::add(const Ref<Shader>& shader){
		const std::string& name = shader->getName();
		add(name, shader);
	}
	Ref<Shader> ShaderLibary::load(const std::string& path){
		auto shader = Shader::create(path);
		add(shader);
		return shader;
	}
	Ref<Shader> ShaderLibary::load(const std::string& name, const std::string& path){

		auto shader = Shader::create(path);
		add(name, shader);
		return shader;
	}
	Ref<Shader> ShaderLibary::get(const std::string& name){
		CORE_ASSERT(exists(name), std::string("Shader not found: ") + name);
		return m_shaders[name];
	}
	const std::vector<std::pair<std::string, std::string>> Shader::s_preProcessorAdds{
			{"ST_default",R"(
##add ST_vertex
##type fragment
#version 460 core
out vec4 FragColor;
##add ST_functions
##add ST_pbr_calculation
##add ST_bindings
)"},//ST_default
			{"ST_vertex",R"(
##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;

layout(std140, binding = 0) uniform matrices
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	mat4 transform;
};
struct vertOutput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
};
layout (location = 0) out vertOutput _output;

void main()
{
    _output.worldPos = vec3(transform * vec4(a_pos, 1.0));
    _output.normal = transpose(inverse(mat3(transform))) * a_normal;
    _output.texCoords = a_texCoords;
    gl_Position =  viewProjection * transform * vec4(a_pos, 1.0);
}
)"},//ST_vertex
			{"ST_functions", R"(
const float PI = 3.14159265359;
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
} 
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
vec3 getNormalFromMap(vec3 world, vec2 tex, vec3 normal, sampler2D map)
{
    vec3 tangentNormal = texture(map, tex).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(world);
    vec3 Q2  = dFdy(world);
    vec2 st1 = dFdx(tex);
    vec2 st2 = dFdy(tex);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
)"},//ST_functions
			{"ST_bindings",R"(
layout(std140, binding = 0) uniform matrices
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	mat4 transform;
};
layout(std140, binding = 2) uniform postProcessing
{
	float toneMappingExposure;
	float gamma;
	float env_lod;
	uint useSkybox;
};
struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
};
layout (location = 0) in vertInput vertex;
)"},//ST_bindings
			{"ST_pbr_calculation",R"(
const int st_maxLights = 25;
struct Light{
	vec4 colorAndStrength;
	vec4 positionAndType;   
	vec4 rotation;
	vec4 spotLightData;
};
struct PBRData {
	vec3  albedo;
	float metallic ;
	float roughness;
	float ao;
	float alpha;

	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;

	vec3 cameraPosition;
	vec3 cameraRotation;

	float toneMappingExposure;
	float gamma;
	float env_lod;

	bool toneMappingAndGammaCorrection;
	bool useSkybox;

	Light lights[st_maxLights];
	uint lightCount;
	bool useLights;
};
layout (binding = 0) uniform samplerCube environmentMap;
layout (binding = 1) uniform samplerCube irradianceMap;
layout (binding = 2) uniform samplerCube prefilterMap;
layout (binding = 3) uniform sampler2D BRDFLUTMap;
vec4 ST_pbr_calculation(PBRData data)
{
	if(data.alpha <= 0.001){
		return vec4(.0f);
	}
	vec3 cameraPos = data.cameraPosition;
	vec3 cameraRot = data.cameraRotation;
	vec3 N = data.normal;
	vec3 V = normalize(cameraPos - data.worldPos);
	vec3 R = reflect(-V, N);
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, data.albedo, data.metallic);
	
	vec3 Lo = vec3(0.0);
	if(data.useLights){
		for(int i = 0; i < data.lightCount; i++){
			//directional
			if(data.lights[i].positionAndType.w == 0){
			
				 // calculate per-light radiance
				vec3 L = normalize(-data.lights[i].rotation.xyz);
				vec3 H = normalize(V + L);
				float attenuation = 1.0f;

				vec3 radiance     = data.lights[i].colorAndStrength.xyz * attenuation;        
	
				// cook-torrance brdf
				float NDF = DistributionGGX(N, H, data.roughness);        
				float G   = GeometrySmith(N, V, L, data.roughness);      
				vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
	
				vec3 kS = F;
				vec3 kD = vec3(1.0) - kS;
				kD *= 1.0 - data.metallic;	  
	
				vec3 numerator    = NDF * G * F;
				float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
				vec3 specular     = numerator / denominator;  

				// add to outgoing radiance Lo
				float NdotL = max(dot(N, L), 0.0);                
				Lo += (kD * data.albedo / PI + specular) * radiance * NdotL * data.lights[i].colorAndStrength.w;
			
			}
			//point
			else if(data.lights[i].positionAndType.w == 1){
				// calculate per-light radiance
				vec3 L = normalize(data.lights[i].positionAndType.xyz - data.worldPos);
				vec3 H = normalize(V + L);
				float distance = length(data.lights[i].positionAndType.xyz - data.worldPos);
				float radius = data.lights[i].spotLightData.w;
				//float attenuation = 1.0 / (distance * distance);
				float attenuation = 1.0f-(min(distance/radius,1.0f));
			
				vec3 radiance     = data.lights[i].colorAndStrength.xyz * attenuation;        
	
				// cook-torrance brdf
				float NDF = DistributionGGX(N, H, data.roughness);        
				float G   = GeometrySmith(N, V, L, data.roughness);      
				vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
	
				vec3 kS = F;
				vec3 kD = vec3(1.0) - kS;
				kD *= 1.0 - data.metallic;	  
	
				vec3 numerator    = NDF * G * F;
				float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
				vec3 specular     = numerator / max(denominator, 0.001);  
		
				// add to outgoing radiance Lo
				float NdotL = max(dot(N, L), 0.0);                
				Lo += (kD * data.albedo / PI + specular) * radiance * NdotL * data.lights[i].colorAndStrength.w;
			}
			//spot
			else if(data.lights[i].positionAndType.w == 2) {
				vec3 L = normalize(data.lights[i].positionAndType.xyz - data.worldPos);
				float theta = dot(L, normalize(-data.lights[i].rotation.xyz));
				float epsilon   = data.lights[i].spotLightData.x - data.lights[i].spotLightData.y;
				float intensity = clamp((theta - data.lights[i].spotLightData.y) / epsilon, 0.0, 1.0);    

				float attenuation = 1.0f;

				vec3 radiance     = data.lights[i].colorAndStrength.xyz * attenuation;        

				vec3 H = normalize(V + L);
	  
				// cook-torrance brdf
				float NDF = DistributionGGX(N, H, data.roughness);        
				float G   = GeometrySmith(N, V, L, data.roughness);      
				vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
	
				vec3 kS = F;
				vec3 kD = vec3(1.0) - kS;
				kD *= 1.0 - data.metallic;	  
	
				vec3 numerator    = NDF * G * F;
				float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
				vec3 specular     = numerator / max(denominator, 0.001);  
		
				// add to outgoing radiance Lo
				float NdotL = max(dot(N, vec3(intensity)), 0.0);                
				Lo += (kD * data.albedo / PI + specular) * radiance * NdotL * data.lights[i].colorAndStrength.w;
			}
		}
	}
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, data.roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - data.metallic;	  
	const float MAX_REFLECTION_LOD = data.env_lod;
   
	vec3 irradiance = vec3(.5f);
	if(data.useSkybox)
		irradiance = texture(irradianceMap, N).rgb;
 
	vec3 diffuse      = irradiance * data.albedo;
   
	vec3 prefilteredColor = vec3(.5f);
	if(data.useSkybox)
		prefilteredColor = textureLod(prefilterMap, R, data.roughness * MAX_REFLECTION_LOD).rgb;
	
	vec2 brdf = vec2(.75*data.roughness);
	if(data.useSkybox)
		brdf  = texture(BRDFLUTMap, vec2(max(dot(N, V), 0.0), data.roughness)).rg;
   
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient = ((kD * diffuse * data.alpha) + specular) * data.ao;
   
	vec3 color = ambient + Lo;

	if(data.toneMappingAndGammaCorrection){
		color = vec3(1.0f) - exp(-color * data.toneMappingExposure);//HDR tonemapping
		color = pow(color, vec3(1.0/data.gamma)); //gamma correct
	}
	return vec4(color, data.alpha);
}
layout(std140, binding = 1) uniform lightData
{
    Light lights[st_maxLights];
    uint lightCount;
};
)"},//ST_pbr_calculation
			{ "ST_CubemapVertex",R"(
		##type vertex
		#version 460 core
		layout (location = 0) in vec3 a_pos;
		layout (location = 1) in vec3 a_normal;
		layout (location = 2) in vec2 a_texcoord;
		out vec3 WorldPos;

		uniform mat4 projection;
		uniform mat4 view;

		void main()
		{
			WorldPos = a_pos;  
			gl_Position =  projection * view * vec4(WorldPos, 1.0);
		}
)" },//ST_CubemapVertex
	};


	ShaderProperity::Type ShaderProperity::typeFromString(const std::string& str) {
		if (str == "Color" || str == "Color4" || str == "color" || str == "color4") {
			return Type::Color4;
		}
		if (str == "Range" || str == "range") {
			return Type::Range;
		}
		if (str == "Enum" || str == "enum" || str == "Combo" || str == "combo") {
			return Type::Enum;
		}
		if (str == "Marker" || str == "marker" || str == "Help" || str == "help") {
			return Type::Marker;
		}
		CORE_WARN("Uknown Shader Properity type: {0}", str);
		return Type::None;
	}

	ShaderProperityRange::ShaderProperityRange(const std::string& values) {
		std::stringstream stream(values);
		std::string segment;
		std::vector<std::string> segments;
		while (std::getline(stream, segment, ','))
		{
			segments.push_back(segment);
		}
		if (segments.size() != 2) {
			CORE_ERROR("Wrong arguments for ShaderProperity Range");
			return;
		}
		m_min = std::stof(segments[0]);
		m_max = std::stof(segments[1]);
	}


	ShaderProperityEnum::ShaderProperityEnum(const std::string& values) {
		std::stringstream stream(values);
		std::string segment;
		while (std::getline(stream, segment, ',')) {
			m_names.push_back(segment);
		}
	}
	ShaderProperityMarker::ShaderProperityMarker(const std::string& text){
		m_text = text;
		size_t index = 0;
		while (true) {
			index = m_text.find("\\n", index);
			if (index == std::string::npos) break;

			m_text.replace(index, 2, "\n");

			index += 2;
		}
	}

}