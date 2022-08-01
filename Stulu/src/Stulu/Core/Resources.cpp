#include "st_pch.h"
#include "Resources.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Scene/AssetsManager.h"

namespace Stulu {
	void Resources::load() {
		std::string assetPath = Application::getEngineAssetDir();
		AssetsManager::update(UUID(9), { AssetType::Shader,Shader::create(assetPath + "/Shaders/PBRShader.glsl"),"",UUID(9) });

		getWhiteTexture();
		getBlackTexture();
		getLoadingTexture();

		getSkyBoxShader();
		AssetsManager::loadAllFiles(assetPath + "/SkyBox", false);
		getDefaultSkyBox();

		getDefaultMaterial();

		getCubeMesh();
		getPlaneMesh();

		AssetsManager::add(assetPath + "/Meshes/capsule.fbx");
		AssetsManager::add(assetPath + "/Meshes/sphere.obj");
		
	}

	Ref<Texture> Resources::getBlackTexture() {
		static Ref<Texture2D> tex;
		if (!tex) {
			tex = Texture2D::create(1, 1);
			static uint32_t datablack = 0x00000000;
			tex->setData(&datablack, sizeof(uint32_t));
			AssetsManager::update(UUID(301), Asset{ AssetType::Texture2D,static_cast<Ref<Texture>>(tex),"",UUID(301), });
		}
		return std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(301)).data);

	}
	Ref<Texture> Resources::getWhiteTexture() {
		static Ref<Texture2D> tex;
		if (!tex) {
			tex = Texture2D::create(1, 1);
			static uint32_t datawhite = 0xffffffff;
			tex->setData(&datawhite, sizeof(uint32_t));
			AssetsManager::update(UUID(302), Asset{ AssetType::Texture2D,static_cast<Ref<Texture>>(tex),"",UUID(301), });
		}
		return std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(302)).data);
	}

	Ref<Texture> Resources::getLoadingTexture() {
		static Ref<Texture2D> tex;
		if (!tex) {
			tex = Texture2D::create(Application::getEngineAssetDir()+"/Textures/loading.png");
			AssetsManager::update(UUID(303), Asset{ AssetType::Texture2D,static_cast<Ref<Texture>>(tex),"",UUID(303), });
		}
		return std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(303)).data);

	}
	Ref<Texture> Resources::getLogoTexture() {
		static Ref<Texture2D> tex;
		if (!tex) {
			tex = Texture2D::create(Application::getEngineAssetDir() + "/Textures/logo.png");
			AssetsManager::update(UUID(304), Asset{ AssetType::Texture2D,static_cast<Ref<Texture>>(tex),"",UUID(304), });
		}
		return std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(304)).data);
	}

	Ref<SkyBox> Resources::getDefaultSkyBox() {
		return std::dynamic_pointer_cast<SkyBox>(std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(11)).data));
	}

	Ref<Mesh> Resources::getCubeMesh() {
		return getCubeMeshAsset().mesh;
	}
	Ref<Mesh> Resources::getPlaneMesh() {
		return getPlaneMeshAsset().mesh;
	}
	Ref<Mesh> Resources::getSphereMesh() {
		return getSphereMeshAsset().mesh;
	}
	Ref<Mesh> Resources::getCapsuleMesh() {
		return getCapsuleMeshAsset().mesh;
	}

	MeshAsset& Resources::getCubeMeshAsset() {
		static Ref<Mesh> m;
		if(!m) {
			static std::vector<Vertex> vertices{
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
			static std::vector<uint32_t> indices{
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
			m = createRef<Mesh>(vertices, indices);
			AssetsManager::update(UUID(401), Asset{ AssetType::Mesh, MeshAsset{"Cube",m,UUID(401),true},"",UUID(401) });
		}

		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(401)).data);
	}
	MeshAsset& Resources::getPlaneMeshAsset() {
		static Ref<Mesh> m;
		if (!m) {
			static std::vector<Vertex> vertices{
			{glm::vec3(-0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
			{glm::vec3(-0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			};
			static std::vector<uint32_t> indices{
				2,1,0,
				0,3,2,
			};
			m = createRef<Mesh>(vertices, indices);
			AssetsManager::update(UUID(402), Asset{ AssetType::Mesh, MeshAsset{"Plane",m,UUID(402),true},"",UUID(402), });
		}
		
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(402)).data);
	}
	MeshAsset& Resources::getSphereMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(404)).data);
	}
	MeshAsset& Resources::getCapsuleMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(405)).data);
	}

	Ref<Material> Resources::getDefaultMaterial() {
		if (!AssetsManager::exists(UUID(12))) {
			AssetsManager::update(UUID(12), { AssetType::Material,createRef<Material>(AssetsManager::get(UUID(9)),
				(std::vector<MaterialDataType>{
					MaterialDataType{ShaderDataType::Float4,glm::vec4(.9f,.9f,.9f,1.0f),"albedo",0},
					MaterialDataType{ShaderDataType::Float,.0f,"metallic",1},
					MaterialDataType{ShaderDataType::Float,.5f,"roughness",2},
					MaterialDataType{ShaderDataType::Float,.1f,"ao",3},
					MaterialDataType{ShaderDataType::Float4,glm::vec4(1.0f),"emissionHDRColor",4},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{4,nullptr,Resources::getWhiteTexture(),1,UUID::null},"albedoMap",5},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{5,nullptr,Resources::getBlackTexture(),1,UUID::null},"metallicMap",6},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{6,nullptr,Resources::getBlackTexture(),1,UUID::null},"roughnessMap",7},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{7,nullptr,Resources::getBlackTexture(),1,UUID::null},"normalMap",8},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{8,nullptr,Resources::getBlackTexture(),1,UUID::null},"aoMap",9},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{9,nullptr,Resources::getBlackTexture(),1,UUID::null},"emissionMap",10},
					MaterialDataType{ShaderDataType::Float2,glm::vec2(1.0f),"textureTilling",11},

				})),"",UUID(12) });
		}
		return std::any_cast<Ref<Material>&>(AssetsManager::get(UUID(12)).data);
	}
	Ref<Material> Resources::getReflectiveMaterial() {
		if (!AssetsManager::exists(UUID(13))) {
			AssetsManager::update(UUID(13), { AssetType::Material,createRef<Material>(AssetsManager::get(UUID(9)),
				(std::vector<MaterialDataType>{
					MaterialDataType{ShaderDataType::Float4,glm::vec4(1.0f),"albedo",0},
					MaterialDataType{ShaderDataType::Float,1.0f,"metallic",1},
					MaterialDataType{ShaderDataType::Float,.0f,"roughness",2},
					MaterialDataType{ShaderDataType::Float,1.0f,"ao",3},
					MaterialDataType{ShaderDataType::Float4,glm::vec4(1.0f),"emissionHDRColor",4},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{4,nullptr,Resources::getWhiteTexture(),1,UUID::null},"albedoMap",5},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{5,nullptr,Resources::getBlackTexture(),1,UUID::null},"metallicMap",6},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{6,nullptr,Resources::getBlackTexture(),1,UUID::null},"roughnessMap",7},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{7,nullptr,Resources::getBlackTexture(),1,UUID::null},"normalMap",8},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{8,nullptr,Resources::getBlackTexture(),1,UUID::null},"aoMap",9},
					MaterialDataType{ShaderDataType::Sampler,MaterialTexture{9,nullptr,Resources::getBlackTexture(),1,UUID::null},"emissionMap",10},
					MaterialDataType{ShaderDataType::Float2,glm::vec2(1.0f),"textureTilling",11},

				})),"",UUID(13) });
		}

		return std::any_cast<Ref<Material>&>(AssetsManager::get(UUID(13)).data);
	}

	Ref<Shader> Resources::getSkyBoxShader() {
		if (!AssetsManager::exists(UUID(07))) {
			static std::string src = R"(
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

		##add ST_functions
		##add ST_bindings

		struct vertInput
		{
			vec3 texCoords;
		};
		layout (location = 0) in vertInput vertex;

		void main() {
			vec4 color = vec4(.0f);
			if(useSkybox){
				vec3 mapColor = clearColor.xyz;
				if(skyboxMapType == 0){
					//acces mip maps if needed
					if(env_lod == 0)
						mapColor = texture(environmentMap, vertex.texCoords).rgb;
					else {
						mapColor = textureLod(environmentMap, vertex.texCoords, env_lod).rgb;
					}
				}else if(skyboxMapType == 1){
					mapColor = texture(irradianceMap, vertex.texCoords).rgb;
				}else if(skyboxMapType == 2){
					mapColor = texture(prefilterMap, vertex.texCoords).rgb;
				}
				color = vec4(mapColor, 1.0f);
			}
			
			FragColor = enableGammaCorrection ? gammaCorrect(color, gamma, toneMappingExposure) : color;
		})";
			AssetsManager::update(UUID(7), Asset{ AssetType::Shader, Shader::create("SkyBox",src),"",UUID(7) });
		}

		return std::any_cast<Ref<Shader>&>(AssetsManager::get(UUID(7)).data);
	}
}
