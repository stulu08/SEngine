#include "st_pch.h"
#include "Resources.h"
#include "Stulu/Renderer/Model.h"
#include "Stulu/Scene/AssetsManager.h"

namespace Stulu {
	void Resources::load() {
		loadCubeMesh();
		loadPlaneMesh();
		loadSphereMesh();
		loadSkyBoxMesh();
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
	Ref<Mesh>& Resources::getSkyBoxMesh() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(403)).data).mesh;
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
	MeshAsset& Resources::getSkyBoxMeshAsset() {
		return std::any_cast<MeshAsset&>(AssetsManager::get(UUID(403)).data);
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
	void Resources::loadSkyBoxMesh() {
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
			0,1,2,
			2,3,0,
			//bottom
			6,5,4,
			4,7,6,
			//right
			8,9,10,
			10,11,8,
			//left
			14,13,12,
			12,15,14,
			//front
			18,17,16,
			16,19,18,
			//back
			20,21,22,
			22,23,20
		};
		Ref<Mesh> m = createRef<Mesh>(vertices, indices);
		AssetsManager::update(UUID(403), Asset{ AssetType::Mesh, MeshAsset{"SkyBox Mesh",m,UUID(403),true},"",UUID(403),});

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
		layout (location = 0) in vertInput vertex;
		layout (binding = 1) uniform samplerCube skybox;
		void main() {
			FragColor = texture(skybox, vertex.texCoords);
		})";
		AssetsManager::update(UUID(7), Asset{ AssetType::Shader, Shader::create("SkyBox",src),"",UUID(7)});
	}
	void Resources::loadReflectiveMaterial() {
		std::string src = R"(
		##add ST_default
		void main()
		{		
		    vec3 view = normalize(vertex.worldPos.xyz - cameraPosition.xyz);
		    vec3 refelcted = reflect(view, normalize(vertex.normal));
		    vec4 relectedColor = texture(skybox, refelcted);
		    FragColor = relectedColor;
		})";
		AssetsManager::update(UUID(6), Asset{ AssetType::Shader, Shader::create("Reflective",src),"",UUID(6) });
		AssetsManager::update(UUID(13), { AssetType::Material,Material(AssetsManager::get(UUID(6)),(std::vector<MaterialDataType>{})),"",UUID(13) });
	}
	void Resources::loadDefaultMaterial() {
		AssetsManager::update(UUID(12), { AssetType::Material,Material(AssetsManager::get(UUID(9)),
		(std::vector<MaterialDataType>{
		MaterialDataType{ShaderDataType::Float4,glm::vec4(.9f),"albedo",0},
		MaterialDataType{ShaderDataType::Float,.0f,"metallic",1},
		MaterialDataType{ShaderDataType::Float,.0f,"roughness",2},
		MaterialDataType{ShaderDataType::Float,.3f,"ao",3},})),"",UUID(12) });
	}
}
