#include "st_pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLComputeShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Stulu{
	Ref<ComputeShader> ComputeShader::create(const std::string& name, const std::string& src) {
		switch (Renderer::getRendererAPI())
		{
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLComputeShader>(name, src);
		case RenderAPI::API::none:
			CORE_ASSERT(false, "No renderAPI specified")
				return nullptr;
		default:
			CORE_ASSERT(false, "RenderAPI not suported")
				return nullptr;
		}

		CORE_ASSERT(false, "Unknown error in ComputeShader creation")
			return nullptr;
	}
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

	ShaderProperity::Type ShaderProperity::typeFromString(const std::string& str) {
		if (str == "Color" || str == "color") {
			return Type::Color;
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
	ShaderProperityColor::ShaderProperityColor(const std::string& values) 
		:m_hdr(false) {
		std::stringstream stream(values);
		std::string arg;
		while (std::getline(stream, arg, ',')) {
			if (arg._Starts_with("hdr") || arg._Starts_with("HDR")) {
				if (arg.find("true"))
					m_hdr = true;
			}
		}
	}

	void Shader::procesInlucdes(std::string& source) {
		//I realy hate this
		static std::vector<std::pair<std::string, std::string>> s_preProcessorAdds{
			{"Stulu/Default",R"(
##include "Stulu/Vertex"
##type fragment
#version 460 core
out vec4 FragColor;
##include "Stulu/Functions"
##include "Stulu/Bindings"
##include "Stulu/PBR"
struct vertInput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) in vertInput vertex;
)"},// Stulu/Default
			{"Stulu/Vertex",R"(
##type vertex
#version 460 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;
layout (location = 3) in vec4 a_color;

##include "Stulu/Bindings"

layout(std140, binding = 1) uniform modelData
{
	mat4 normalMatrix;
	mat4 transform;
};
struct vertOutput
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
	vec4 color;
};
layout (location = 0) out vertOutput _output;

void main()
{
	vec4 world = transform * vec4(a_pos, 1.0);

    _output.worldPos = world.xyz;
    _output.normal = (normalMatrix * vec4(a_normal, 0.0)).xyz;
    _output.texCoords = a_texCoords;
	_output.color = a_color;

    gl_Position = viewProjection * world;
}
)"},// Stulu/Vertex
			{"Stulu/Functions", R"(
#define st_impl_for_all_types(func) \
func(vec4);		\
func(vec3);		\
func(vec2);		\
func(float);	\
func(int);		\
func(uint);		\
func(double);

const uint ShaderViewFlag_DisplayLighting	= 0x00000001u;
const uint ShaderViewFlag_DisplayDiffuse	= 0x00000002u;
const uint ShaderViewFlag_DisplaySpecular	= 0x00000004u;
const uint ShaderViewFlag_DisplayNormal		= 0x00000008u;
const uint ShaderViewFlag_DisplayRoughness	= 0x00000010u;
const uint ShaderViewFlag_DisplayMetallic	= 0x00000020u;
const uint ShaderViewFlag_DisplayAmbient	= 0x00000040u;
const uint ShaderViewFlag_DisplayTexCoords	= 0x00000080u;
const uint ShaderViewFlag_DisplayVertices	= 0x00000100u;
const uint ShaderViewFlag_DisplayEmission	= 0x00000200u;
const uint ShaderViewFlag_DisplayDepth		= 0x00000400u;
const float PI = 3.14159265359;

bool isFlagEnabled(uint flags, uint flag) {
	return (flags & flag) == flag;
}
bool isFlagDisabled(uint flags, uint flag) {
	return (flags & flag) != flag;
}

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
	vec3 textureValue = texture(map, tex).xyz;
	if(textureValue == vec3(0))
		return normal;
    vec3 tangentNormal = textureValue * 2.0 - 1.0;
	
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
vec4 gammaCorrect(vec4 color, float _gamma, float exposure) {
	color = vec4(vec3(1.0f) - exp(-color.rgb * exposure),color.a);//HDR tonemapping
	color = vec4(pow(color.rgb, vec3(1.0/_gamma)),color.a); //gamma correct
	return color;
}
vec3 gammaCorrect(vec3 color, float _gamma, float exposure) {
	color = vec3(1.0f) - exp(-color.rgb * exposure);//HDR tonemapping
	color = pow(color.rgb, vec3(1.0/_gamma)); //gamma correct
	return color;
}
float filterAlpha(float alpha, uint mode, float cutOut = 1.0f) {
	//CutOut
	if(mode == 1) {
		if(alpha <= cutOut){
			return 0.0;//full transparent
		}else{
			return 1.0;//full opaque
		}
	}
	//Transparent 
	else if(mode == 2) {
		return alpha;//keep alpha
	}
	//Opaque
	else{
		return 1.0;//ignore alpha
	}

}
vec3 getSkyBoxCoords(vec3 view, mat4 _skyBoxRotation) {
	return normalize(_skyBoxRotation * vec4(view, 0.0)).xyz;
}
float linearizeDepth(float depth, float near, float far) {
	float z = depth * 2.0 - 1.0; // back to NDC 
    return ((2.0 * near * far) / (far + near - z * (far - near)));
}
vec3 srgbToLin(vec3 color){
	return pow(color, vec3(2.2));
}
//i know it should be srgb_alpha_ToLin but I do nothing with the alpha
vec4 srgbToLin(vec4 color){
	return vec4(srgbToLin(color.xyz), color.a);
}
//functions for branchless stuff

#define st_when_eq_impl(type) type when_eq(type x, type y) { return type(1.0) - abs(sign(x-y));}
#define st_when_neq_impl(type) type when_neq(type x, type y) { return abs(sign(x-y));}
#define st_when_gt_impl(type) type when_gt(type x, type y) { return max(sign(x-y), type(0.0));}
#define st_when_lt_impl(type) type when_lt(type x, type y) { return max(sign(y-x), type(0.0));}
#define st_when_ge_impl(type) type when_ge(type x, type y) { return type(1.0)-when_gt(x,y);}
#define st_when_le_impl(type) type when_le(type x, type y) { return type(1.0)-when_lt(x,y);}
#define st_and_impl(type) type and(type a, type b) { return a*b;}
#define st_or_impl(type) type or(type a, type b) { return min(a+b, type(1.0));}
#define st_not_impl(type) type not(type a, type b) { return type(1.0)-a;}

st_impl_for_all_types(st_when_eq_impl);
st_impl_for_all_types(st_when_neq_impl)
st_impl_for_all_types(st_when_gt_impl)
st_impl_for_all_types(st_when_lt_impl)
st_impl_for_all_types(st_when_ge_impl)
st_impl_for_all_types(st_when_le_impl)
st_impl_for_all_types(st_and_impl)
st_impl_for_all_types(st_or_impl)
st_impl_for_all_types(st_not_impl)

float FisFlagEnabled(uint flags, uint flag) {
	return when_eq(flags & flag, flag);
}
float FisFlagDisabled(uint flags, uint flag) {
	return when_neq(flags & flag, flag);
}

)"},// Stulu/Functions
			{"Stulu/Bindings",

std::string("const int st_maxLights = ") + std::to_string(ST_MAXLIGHTS) + ";\n" +
R"(
struct Light{
	vec4 colorAndStrength;
	vec4 positionAndType;   
	vec4 rotation;
	vec4 spotLightData;
};

layout(std140, binding = 0) uniform cameraData
{
	mat4 viewProjection;
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPosition;
	vec4 cameraRotation;
	vec4 cameraNearFar;
};
//			depends on renderer
// --------------------------------------------
// following is used by the default renderer:
//layout(std140, binding = 1) uniform modelData
//{
//		mat4 normalMatrix;
//		mat4 transform;
//};
// --------------------------------------------
// following is used by post processing:
//layout(std140, binding = 1) uniform modelData
//{
//		//float z;
//};
// --------------------------------------------
// following is used by post gizmo instancing
//struct InstanceData{
//	mat4 transforms;
//	vec4 instanceColors;
//};
//layout(std140, binding = 1) uniform modelDatayout(std140, binding = 1) uniform modelData
//{
//		InstanceData instanceData[180];
//};
// --------------------------------------------
layout(std140, binding = 2) uniform lightData
{
    Light lights[st_maxLights];
    uint lightCount;
};
layout(std140, binding = 3) uniform postProcessing
{
	float time;
	float delta;
	float toneMappingExposure;
	float gamma;
};
layout(std140, binding = 4) uniform shaderSceneData
{
	mat4 skyBoxRotation;
	vec4 clearColor;
	bool useSkybox;
	uint skyboxMapType;
	uint viewFlags;
	float env_lod;
};
layout (binding = 0) uniform samplerCube environmentMap;
layout (binding = 1) uniform samplerCube irradianceMap;
layout (binding = 2) uniform samplerCube prefilterMap;
layout (binding = 3) uniform sampler2D BRDFLUTMap;
)"},// Stulu/Bindings
			{"Stulu/PBR",R"(
struct PBRData {
	vec3  albedo;
	vec3  emission;
	float metallic;
	float roughness;
	float ao;
	float alpha;

	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
};
vec4 ST_PBR(inout PBRData data)
{
	if(data.alpha <= 0.001){
		return vec4(.0f);
	}
	
	vec3 N = normalize(data.normal);
	vec3 V = normalize(cameraPosition.xyz - data.worldPos);
	vec3 R = reflect(-V, N);
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, data.albedo, data.metallic);
	
	vec3 Lo = vec3(0.0);
	//lighting
	{
		for(int i = 0; i < lightCount; i++){
			float distance = length(lights[i].positionAndType.xyz - data.worldPos);

			vec3 L;
			vec3 H;
			float attenuation = 1.0f;
			vec3 radiance;
			//calculate by light type
			
			if(lights[i].positionAndType.w == 0){
				// calculate per-light radiance
				L = normalize(-lights[i].rotation.xyz);
				H = normalize(V + L);
				radiance = lights[i].colorAndStrength.xyz * attenuation;        
			}
			//point
			else if(lights[i].positionAndType.w == 1){
				// calculate per-light radiance
				L = normalize(lights[i].positionAndType.xyz - data.worldPos);
				H = normalize(V + L);
				float radius = lights[i].spotLightData.w;
				attenuation = 1.0f-(min(distance/radius,1.0f));
				radiance     = lights[i].colorAndStrength.xyz * attenuation;        
			}
			//spot
			else if(lights[i].positionAndType.w == 2) {
				// calculate per-light radiance
				L = normalize(lights[i].positionAndType.xyz - data.worldPos);
				H = normalize(V + L);
				float theta = dot(L, normalize(-lights[i].rotation.xyz));
				float epsilon   = lights[i].spotLightData.x - lights[i].spotLightData.y;
				float intensity = clamp((theta - lights[i].spotLightData.y) / epsilon, 0.0, 1.0);    
				attenuation = intensity;
				radiance     = lights[i].colorAndStrength.xyz * attenuation;   
			}

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
			Lo += (kD * data.albedo / PI + specular) * radiance * NdotL * lights[i].colorAndStrength.w;
		}
		//emission
		Lo += data.emission;
	}
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, data.roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - data.metallic;
	const float MAX_REFLECTION_LOD = 4.0f;
  
	vec3 prefilteredColor = vec3(clearColor);
	vec3 irradiance = vec3(clearColor);
	vec2 brdf = vec2(.75*data.roughness);

	if(useSkybox) {
		vec3 SB_N = getSkyBoxCoords(N, skyBoxRotation);
		vec3 SB_R = getSkyBoxCoords(R, skyBoxRotation);
		vec3 SB_V = getSkyBoxCoords(V, skyBoxRotation);
		
		irradiance = texture(irradianceMap, SB_N).rgb;
		//irradiance = texture(irradianceMap, N).rgb;

		prefilteredColor = textureLod(prefilterMap, SB_R, data.roughness * MAX_REFLECTION_LOD).rgb;
		//prefilteredColor = textureLod(prefilterMap, R, data.roughness * MAX_REFLECTION_LOD).rgb;

		brdf  = texture(BRDFLUTMap, vec2(max(dot(SB_N, SB_V), 0.0), data.roughness)).rg;
		//brdf  = texture(BRDFLUTMap, vec2(max(dot(N, V), 0.0), data.roughness)).rg;
	}

   
	vec3 diffuse  = irradiance * data.albedo;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	vec3 ambient = ((kD * diffuse * data.alpha) + specular)*data.ao;
		
	vec3 color = ambient + Lo;
	
	//filter flags
	if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayLighting))
		color = vec3(Lo);
	if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayDepth))
		color = vec3(linearizeDepth(gl_FragCoord.z, cameraNearFar.x, cameraNearFar.y)/cameraNearFar.y);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayDiffuse))
		color = vec3(diffuse);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplaySpecular))
		color = vec3(specular);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayNormal))
		color = vec3(data.normal) * 0.5 + 0.5;
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayRoughness))
		color = vec3(data.roughness);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayMetallic))
		color = vec3(data.metallic);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayAmbient))
		color = vec3(data.ao);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayTexCoords))
		color = vec3(data.texCoords, 0);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayVertices))
		color = vec3(data.albedo);
	else if(isFlagEnabled(viewFlags, ShaderViewFlag_DisplayEmission))
		color = vec3(data.emission);

	return vec4(color, data.alpha);
}
)"},// Stulu/PBR
		};
		back:
		for (auto& p : s_preProcessorAdds) {
			std::string token = "##include \"" + p.first + "\"";
			size_t pos = source.find(token);
			if (pos != std::string::npos) {
				source.replace(pos, token.length(), p.second);
				goto back;
			}
		}
	}

}