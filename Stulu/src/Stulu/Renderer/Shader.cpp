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
    _output.normal = mat3(transpose(inverse(transform))) * a_normal;
    _output.texCoords = a_texCoords;
    gl_Position =  viewProjection * transform * vec4(a_pos, 1.0);
}
)"},//ST_vertex
			{"ST_functions", R"(
const float PI = 3.14159265359;
const int st_maxLights = 25;
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
struct Light{
    vec4 colorAndStrength;
    vec4 positionAndType;   
    vec4 rotation;
    vec4 spotLightData;
};
layout(std140, binding = 0) uniform matrices
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	mat4 transform;
};
layout(std140, binding = 1) uniform lightData
{
    Light lights[st_maxLights];
    uint lightCount;
};
layout(std140, binding = 2) uniform postProcessing
{
	float toneMappingExposure;
	float gamma;
};
struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
};
layout (location = 0) in vertInput vertex;
layout (binding = 1) uniform samplerCube skybox;
)"},//ST_bindings
	};
}