#include "st_pch.h"
#include "Resources.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Renderer/Renderer.h"

namespace Stulu {
	/*
	Default Asset list:
	7   -> Skybox Shader
	8   -> Quad Shader
	9   -> PBRShader
	10  -> TerrainShader
	11  -> Default Skybox
	12  -> Default Material 
	13  -> Reflective Material
	14  -> Terrain Material

	301 -> Black Texture
	302 -> White Texture
	303 -> Loading Texture
	304 -> Logo Texture

	401 -> Cube Mesh 
	402 -> Plane Mesh
	404 -> Sphere Mesh
	405 -> Capsule Mesh
	406 -> IcoSphere Mesh
	407 -> High Res Sphere Mesh
	*/

	void Resources::load() {
		std::string assetPath = Application::getEngineAssetDir();
		AssetsManager::update(UUID(9), { AssetType::Shader,Renderer::getShaderSystem()->GetShader("Default/PBR"),"",UUID(9) });
		AssetsManager::update(UUID(10), { AssetType::Shader,Renderer::getShaderSystem()->GetShader("Default/Terrain"),"",UUID(10) });

		getWhiteTexture();
		getBlackTexture();
		getLoadingTexture();

		getSkyBoxShader();
		AssetsManager::update(UUID(11), { AssetType::SkyBox,static_cast<Ref<Texture>>(SkyBox::create(assetPath + "/SkyBox/Default.hdr", 1024)),"",UUID(11)});
		getDefaultSkyBox();

		getDefaultMaterial();

		getCubeMesh();
		getPlaneMesh();

		AssetsManager::add(assetPath + "/Meshes/capsule.fbx");
		AssetsManager::add(assetPath + "/Meshes/ico_sphere.fbx");
		AssetsManager::add(assetPath + "/Meshes/high_res_sphere.obj");
		AssetsManager::add(assetPath + "/Meshes/sphere.fbx");
		
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
			AssetsManager::update(UUID(302), Asset{ AssetType::Texture2D,static_cast<Ref<Texture>>(tex),"",UUID(302), });
		}
		return std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(302)).data);
	}
	Ref<Texture> Resources::getLoadingTexture() {
		static Ref<Texture2D> tex;
		if (!tex) {
			tex = Texture2D::create(Application::getEngineAssetDir()+"/Textures/Loading.png");
			AssetsManager::update(UUID(303), Asset{ AssetType::Texture2D,static_cast<Ref<Texture>>(tex),"",UUID(303), });
		}
		return std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(303)).data);

	}
	Ref<Texture> Resources::getLogoTexture() {
		static Ref<Texture2D> tex;
		if (!tex) {
			tex = Texture2D::create(Application::getEngineAssetDir() + "/Textures/PNG - Logo - White.png");
			AssetsManager::update(UUID(304), Asset{ AssetType::Texture2D,static_cast<Ref<Texture>>(tex),"",UUID(304), });
		}
		return std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(304)).data);
	}

	Ref<SkyBox> Resources::getDefaultSkyBox() {
		return std::dynamic_pointer_cast<SkyBox>(std::any_cast<Ref<Texture>&>(AssetsManager::get(UUID(11)).data));
	}

	Asset& Resources::getDefaultSkyBoxAsset() {
		return AssetsManager::get(UUID(11));
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
	Ref<Mesh> Resources::getHighResSphereMesh() {
		return getHighResSphereMeshAsset().mesh;
	}
	Ref<Mesh> Resources::getIcoSphereMesh() {
		return getIcoSphereMeshAsset().mesh;
	}
	Ref<Mesh> Resources::getCapsuleMesh() {
		return getCapsuleMeshAsset().mesh;
	}

	MeshAsset& Resources::getCubeMeshAsset() {
		static Ref<Mesh> m;
		if(!m) {
			static std::vector<Vertex> vertices{
				//top
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
				{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
				{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
				//bottom														 		 
				{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
				//right															 		 
				{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
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
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
				{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
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
			{glm::vec3(-0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
			{glm::vec3(-0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
			{glm::vec3(0.5f,  0.0f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
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
		if (AssetsManager::existsAndType(UUID(404), AssetType::Mesh)) {
			return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(404)).data);
		}
		return getCubeMeshAsset();
	}
	MeshAsset& Resources::getIcoSphereMeshAsset() {
		if (AssetsManager::existsAndType(UUID(406), AssetType::Mesh)) {
			return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(406)).data);
		}
		return getCubeMeshAsset();
	}
	MeshAsset& Resources::getCapsuleMeshAsset() {
		if (AssetsManager::existsAndType(UUID(405), AssetType::Mesh)) {
			return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(405)).data);
		}
		return getCubeMeshAsset();
	}
	MeshAsset& Resources::getHighResSphereMeshAsset() {
		if (AssetsManager::existsAndType(UUID(407), AssetType::Mesh)) {
			return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(407)).data);
		}
		return getCubeMeshAsset();
	}

	Ref<VertexArray>& Resources::getFullscreenVA() {
		static Ref<VertexArray> m_quadVertexArray;
		if (!m_quadVertexArray) {
			Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
			Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
			m_quadVertexArray = Stulu::VertexArray::create();
			float vertices[20]{
				-1.0f,-1.0f,.0f, 0.0f,0.0f,
				1.0f,-1.0f,.0f, 1.0f,0.0f,
				1.0f,1.0f,.0f, 1.0f,1.0f,
				-1.0f,1.0f,.0f, 0.0f,1.0f,
			};
			vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(20 * sizeof(float)), vertices);
			vertexBuffer->setLayout({
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
				});
			m_quadVertexArray->addVertexBuffer(vertexBuffer);
			uint32_t indices[6]{
				0,1,2,
				2,3,0
			};
			indexBuffer = Stulu::IndexBuffer::create((uint32_t)6, indices);
			m_quadVertexArray->setIndexBuffer(indexBuffer);
		}
		return m_quadVertexArray;
	}

	Ref<Shader>& Resources::getFullscreenShader() {
		static Ref<Shader> shader;
		const UUID id = UUID(8);
		if (!AssetsManager::exists(id)) {
			shader = Renderer::getShaderSystem()->GetShader("Renderer/QuadFullScreen");
			AssetsManager::update(id, Asset{ AssetType::Shader, shader,"",id });
		}
		return std::any_cast<Ref<Shader>&>(AssetsManager::get(id).data);
	}
	Ref<Shader> Resources::getSkyBoxShader() {
		static Ref<Shader> shader;
		const UUID id = UUID(7);
		if (!AssetsManager::exists(id)) {
			shader = Renderer::getShaderSystem()->GetShader("Renderer/SkyBox");
			AssetsManager::update(id, Asset{ AssetType::Shader, shader,"",id });
		}
		return std::any_cast<Ref<Shader>&>(AssetsManager::get(id).data);
	}

	Ref<Material> Resources::getDefaultMaterial() {
		static Ref<Material> material;
		if (!material) {
			material = createMaterial("Default Material", UUID(12));
		}
		return material;
	}
	Ref<Material> Resources::getReflectiveMaterial() {
		static Ref<Material> material;
		if (!material) {
			material = createMaterial("Reflective Material", UUID(13), glm::vec4(1.0f), 1.0f, 0.0f, 1.0f);
		}
		return material;
	}

	Ref<Material> Resources::getTerrainMaterial() {
		static Ref<Material> material;
		if (!material) {
			Ref<Material> material = createRef<Material>(AssetsManager::get(UUID(10)), std::vector<MaterialDataType>{}, "Terrain Material", TransparencyMode::Opaque, 1.0f);
			AssetsManager::update(UUID(14), {AssetType::Material,material,"",UUID(14) });
			return std::any_cast<Ref<Material>&>(AssetsManager::get(UUID(14)).data);
		}
		return material;
	}
	Ref<Material> Resources::createMaterial(const std::string& name, const UUID& uuid, const glm::vec4& albedo, const float& metallic, const float& roughness, const float& ao, const glm::vec4& emission, const UUID& albedoMap, const UUID& metallicMap, const UUID& roughnessMap, const UUID& aoMap, const UUID& emissionMap, const UUID& normalMap, const glm::vec2& textureTilling, TransparencyMode transparencyMode, float alphaCutOff) {
		Ref<Material> material = createRef<Material>(AssetsManager::get(UUID(9)),
			(std::vector<MaterialDataType>{
				MaterialDataType{ ShaderDataType::Float4,albedo,"albedo",0 },
				MaterialDataType{ ShaderDataType::Float,metallic,"metallic",1 },
				MaterialDataType{ ShaderDataType::Float,roughness,"roughness",2 },
				MaterialDataType{ ShaderDataType::Float,ao,"ao",3 },
				MaterialDataType{ ShaderDataType::Float4,emission,"emission",4 },
				MaterialDataType{ ShaderDataType::Sampler,MaterialTexture{0,nullptr,Resources::getWhiteTexture(),1,albedoMap},"albedoMap",5 },
				MaterialDataType{ ShaderDataType::Sampler,MaterialTexture{1,nullptr,Resources::getBlackTexture(),1,metallicMap},"metallicMap",6 },
				MaterialDataType{ ShaderDataType::Sampler,MaterialTexture{2,nullptr,Resources::getBlackTexture(),1,roughnessMap},"roughnessMap",7 },
				MaterialDataType{ ShaderDataType::Sampler,MaterialTexture{3,nullptr,Resources::getBlackTexture(),1,normalMap},"normalMap",8 },
				MaterialDataType{ ShaderDataType::Sampler,MaterialTexture{4,nullptr,Resources::getBlackTexture(),1,aoMap},"aoMap",9 },
				MaterialDataType{ ShaderDataType::Sampler,MaterialTexture{5,nullptr,Resources::getBlackTexture(),1,emissionMap},"emissionMap",10 },
				MaterialDataType{ ShaderDataType::Float2,textureTilling,"textureTilling",11 },

		}), name, transparencyMode, alphaCutOff);
		AssetsManager::update(uuid, { AssetType::Material,material,"",uuid });

		return std::any_cast<Ref<Material>&>(AssetsManager::get(uuid).data);
	}


}
