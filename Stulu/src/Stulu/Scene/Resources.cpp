#include "st_pch.h"
#include "Resources.h"
#include "Stulu/Scene/AssetsManager.h"

namespace Stulu {
	void Resources::load() {
		loadCubeMesh();
		loadPlaneMesh();
		loadSphereMesh();
		loadBlackTexture();
		loadSkyBoxShader();
		loadReflectiveMaterial();
		loadDefaultMaterial();
	}

	Ref<Mesh>& Resources::getCubeMesh() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(401)).data).mesh;
	}
	Ref<Mesh>& Resources::getPlaneMesh() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(402)).data).mesh;
	}
	Ref<Mesh>& Resources::getSphereMesh() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(404)).data).mesh;
	}
	Ref<Texture2D>& Resources::getBlackTexture() {
		return std::any_cast<Ref<Texture2D>&>(AssetsManager::get(UUID(301)).data);
	}
	Ref<Texture2D>& Resources::getWhiteTexture() {
		return std::any_cast<Ref<Texture2D>&>(AssetsManager::get(UUID(302)).data);
	}
	Ref<Shader>& Resources::getSkyBoxShader() {
		return std::any_cast<Ref<Shader>&>(AssetsManager::get(UUID(7)).data);
	}
	MeshAsset& Resources::getCubeMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(401)).data);
	}
	MeshAsset& Resources::getPlaneMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(402)).data);
	}
	MeshAsset& Resources::getSphereMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(404)).data);
	}
	Material* Resources::getReflectiveMaterial() {
		return AssetsManager::get(UUID(13)).data._Cast<Material>();
	}
	Material* Resources::getDefaultMaterial() {
		return AssetsManager::get(UUID(12)).data._Cast<Material>();
	}
	void Resources::loadCubeMesh() {
		std::vector<Vertex> vertices{
			//top
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//bottom														 		 
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//right															 		 
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//left															 		 
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			//front															 		 
			{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
			//back															 		 
			{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
		};
		std::vector<uint32_t> indices{
			//top
			2,1,0,
			0,3,2,
			//bottom
			4,5,6,
			6,7,4,
			//right
			10,9,8,
			8,11,10,
			//left
			12,13,14,
			14,15,12,
			//front
			16,17,18,
			18,19,16,
			//back
			22,21,20,
			20,23,22
		};
		Ref<Mesh> m = createRef<Mesh>(vertices, indices);
		AssetsManager::update(UUID(401), Asset{ AssetType::Mesh, MeshAsset{"Cube",m,UUID(401),true},"",UUID(401) });
	}
	void Resources::loadPlaneMesh() {
		std::vector<Vertex> vertices{
			{glm::vec3(-0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
		};
		std::vector<uint32_t> indices{
			2,1,0,
			0,3,2,
		};
		Ref<Mesh> m = createRef<Mesh>(vertices, indices);
		AssetsManager::update(UUID(402), Asset{ AssetType::Mesh, MeshAsset{"Plane",m,UUID(402),true},"",UUID(402), });
	}
	void Resources::loadSphereMesh() {
	}
	void Resources::loadBlackTexture() {
		Ref<Texture2D> tex = Texture2D::create(1, 1);
		uint32_t datablack = 0x00000000;
		tex->setData(&datablack, sizeof(uint32_t));
		AssetsManager::update(UUID(301), Asset{ AssetType::Texture2D,tex,"",UUID(301), });
	}
	void Resources::loadWhiteTexture() {
		Ref<Texture2D> tex = Texture2D::create(1, 1);
		uint32_t datawhite = 0xffffffff;
		tex->setData(&datawhite, sizeof(uint32_t));
		AssetsManager::update(UUID(302), Asset{ AssetType::Texture2D,tex,"",UUID(301), });
	}
	void Resources::loadSkyBoxShader() {
		std::string src = R"(
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
			vec3 texCoords;
		};
		layout (location = 0) out vertOutput _output;

		void main()
		{
			_output.texCoords = a_pos;
			mat4 view = mat4(mat3(viewMatrix));
			vec4 pos = projMatrix * view* transform * vec4(a_pos, 1.0);
			gl_Position =  pos.xyww;
		}
		##type fragment
		#version 460 core
		out vec4 FragColor;
		struct vertInput
		{
			vec3 texCoords;
		};
		layout(std140, binding = 2) uniform postProcessing
		{
			float toneMappingExposure;
			float gamma;
			float env_load;
			uint useSkybox;
		};
		layout (location = 0) in vertInput vertex;
		layout (binding = 0) uniform samplerCube environmentMap;
		layout (binding = 1) uniform samplerCube irradianceMap;
		layout (binding = 2) uniform samplerCube prefilterMap;
		layout (binding = 3) uniform sampler2D BRDFLUTMap;
		uniform int mapType = 0;
		void main() {
			vec4 color = vec4(.0f);
			if(useSkybox == 1){
				vec3 mapColor;
				if(mapType == 0){
					mapColor = texture(environmentMap, vertex.texCoords).rgb;
				}else if(mapType == 1){
					mapColor = texture(irradianceMap, vertex.texCoords).rgb;
				}else if(mapType == 2){
					mapColor = texture(prefilterMap, vertex.texCoords).rgb;
				}else {
					mapColor = texture(environmentMap, vertex.texCoords).rgb;
				}

				mapColor = vec3(1.0f) - exp(-mapColor * toneMappingExposure);//HDR tonemapping
				mapColor = pow(mapColor, vec3(1.0/gamma)), 1.0; //gamma correct
				color = vec4(mapColor, 1.0f);
			}
			
			FragColor = color;
		})";
		AssetsManager::update(UUID(7), Asset{ AssetType::Shader, Shader::create("SkyBox",src),"",UUID(7)});
	}
	void Resources::loadReflectiveMaterial() {
		std::string src = R"(
		##add ST_vertex
		##type fragment
		#version 460 core
		out vec4 FragColor;
		##add ST_bindings
		layout (binding = 0) uniform samplerCube environmentMap;
		layout (binding = 1) uniform samplerCube irradianceMap;
		layout (binding = 2) uniform samplerCube prefilterMap;
		void main()
		{		
			vec3 cameraPos = vec3(cameraPosition);
			vec3 cameraRot = vec3(cameraRotation);
			vec3 N = normalize(vertex.normal);
			vec3 V = normalize(cameraPos - vertex.worldPos);
			vec3 R = reflect(-V, N);
			
		    vec3 relectedColor = vec3(.0f);
			if(useSkybox == 1)
				relectedColor = texture(environmentMap, R).rgb;
		    FragColor = vec4(relectedColor, 1.0f);
		})";
		AssetsManager::update(UUID(6), Asset{ AssetType::Shader, Shader::create("Reflective",src),"",UUID(6) });
		AssetsManager::update(UUID(13), { AssetType::Material,Material(AssetsManager::get(UUID(6)),(std::vector<MaterialDataType>{})),"",UUID(13) });
	}
	void Resources::loadDefaultMaterial() {
		AssetsManager::update(UUID(12), { AssetType::Material,Material(AssetsManager::get(UUID(9)),
		(std::vector<MaterialDataType>{
			MaterialDataType{ShaderDataType::Float4,glm::vec4(.9f,.9f,.9f,1.0f),"albedo",0},
			MaterialDataType{ShaderDataType::Float,.0f,"metallic",1},
			MaterialDataType{ShaderDataType::Float,.5f,"roughness",2},
			MaterialDataType{ShaderDataType::Float,.1f,"ao",3},
			MaterialDataType{ShaderDataType::Sampler,MaterialTexture{4,nullptr,1,UUID::null},"albedoMap",4},
			MaterialDataType{ShaderDataType::Sampler,MaterialTexture{5,nullptr,1,UUID::null},"metallicMap",5},
			MaterialDataType{ShaderDataType::Sampler,MaterialTexture{6,nullptr,1,UUID::null},"roughnessMap",6},
			MaterialDataType{ShaderDataType::Sampler,MaterialTexture{7,nullptr,1,UUID::null},"normalMap",7},
			MaterialDataType{ShaderDataType::Sampler,MaterialTexture{8,nullptr,1,UUID::null},"aoMap",8},
			MaterialDataType{ShaderDataType::Float2,glm::vec2(1.0f),"textureTilling",9},
			MaterialDataType{ShaderDataType::Int,0,"transparencyMode",10},
			MaterialDataType{ShaderDataType::Float,.0f,"alphaCutOff",11},
			MaterialDataType{ShaderDataType::Int,0,"useGLTFMetallicRoughnessMap",12},

		})),"",UUID(12) });
	}
}
