#include "st_pch.h"
#include "MeshAsset.h"

#include "Stulu/Renderer/Material/MaterialProperty.h"
#include "Stulu/Renderer/Material/TestMaterial.h"
#include "Stulu/Renderer/Renderer.h"

#include <magic_enum/magic_enum.hpp>

namespace Stulu {
	bool SharedMeshAssetData::LoadFromPath(const std::string& path) {
		std::ifstream stream(path, std::ios::binary);
		if (!stream.is_open())
			return false;

		// Check magic header
		char magicBuffer[sizeof(MagicValue)];
		stream.read(magicBuffer, sizeof(MagicValue));
		if (std::memcmp(magicBuffer, MagicValue, sizeof(MagicValue)) != 0) {
			return false;
		}

		// Read YAML header
		uint32_t headerSize = 0;
		stream.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));

		std::string header(headerSize, '\0');
		stream.read(header.data(), headerSize);

		YAML::Node yaml = YAML::Load(header);

		const uint32_t vertexCount = yaml["VertexCount"].as<uint32_t>();
		const uint32_t indexCount = yaml["IndexCount"].as<uint32_t>();
		std::string name = yaml["Name"].as<std::string>("");

		// Determine layout
		BufferLayout layout = Mesh::DefaultVertexLayout();

		//if (yaml["VertexLayout"]) {
		//	layout = BufferLayout();
		//	YAML::Node layoutNode = yaml["VertexLayout"];
		//	for (auto it : layoutNode) {
		//		auto& data = it.second;
		//
		//		BufferElementIDType name = magic_enum::enum_cast<BufferElementIDType>(it.first.as<std::string>()).value_or(BufferElementIDType::Other);
		//		ShaderDataType type = magic_enum::enum_cast<ShaderDataType>(data["Type"].as<std::string>()).value_or(ShaderDataType::none);
		//		bool normalized = data["Normalized"] ? data["Normalized"].as<bool>() : false;
		//		layout.addElement({ type, name, normalized });
		//	}
		//}

		uint32_t stride = layout.getStride();
		std::vector<Mesh::ByteType> vertexData(vertexCount * stride);
		stream.read(reinterpret_cast<char*>(vertexData.data()), vertexData.size());

		std::vector<uint32_t> indexData(indexCount);
		stream.read(reinterpret_cast<char*>(indexData.data()), indexData.size() * sizeof(uint32_t));

		// Create mesh
		Ref<Mesh> mesh = createRef<Mesh>();
		mesh->ConstructMesh(vertexData.data(), (size_t)vertexCount, layout, (const uint32_t*)indexData.data(), (size_t)indexCount);

		// Submeshes
		if (yaml["SubMeshes"]) {
			YAML::Node subMeshNode = yaml["SubMeshes"];
			for (auto it : subMeshNode) {
				std::string subMeshName = it.first.as<std::string>();
				auto data = it.second;

				MeshSubmesh sm;
				sm.name = subMeshName;
				sm.indexCount = data["IndexCount"].as<uint32_t>();
				sm.indexOffset = data["IndexOffset"].as<uint32_t>();
				sm.vertexOffset = data["VertexOffset"].as<uint32_t>();
				mesh->AddSubmesh(sm);
				if (name.empty()) {
					name = subMeshName;
				}
			}
		}

		m_mesh = mesh;
		m_mesh->SetName(name);

		return true;
	}
	bool SharedMeshAssetData::SaveToFile(const std::string& path) const {
		if (!m_mesh->GetVertexArray() || m_mesh->GetVertexArray()->getVertexBuffers().size() < 1)
			return false;

		std::ofstream stream(path, std::ios::binary);
		if (!stream.is_open()) {
			return false;
		}

		// SMesh
		stream.write(MagicValue, sizeof(MagicValue));

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << m_mesh->GetName();
		out << YAML::Key << "VertexCount" << YAML::Value << m_mesh->GetVerticesCount();
		out << YAML::Key << "IndexCount" << YAML::Value << m_mesh->GetIndices().size();

		// saved meshes only support default layout
		//out << YAML::Key << "VertexLayout" << YAML::Value << YAML::BeginMap;
		//
		//for (const auto& ele : m_mesh->GetVertexArray()->getVertexBuffers().front()->getLayout()) {
		//	out << YAML::Key << std::string(magic_enum::enum_name(ele.idType)) << YAML::BeginMap;
		//	out << YAML::Key << "Type" << YAML::Value << std::string(magic_enum::enum_name(ele.type));
		//	if (ele.normalized)
		//		out << YAML::Key << "Normalized" << YAML::Value << true;
		//	out << YAML::EndMap;
		//}

		out << YAML::EndMap;
	
		const auto& subMeshes = m_mesh->GetSubmeshes();
		if (subMeshes.size() > 0) {
			out << YAML::Key << "SubMeshes" << YAML::BeginMap;
			for (const MeshSubmesh& subMesh : subMeshes) {
				out << YAML::Key << subMesh.name << YAML::BeginMap;
				out << YAML::Key << "IndexCount" << YAML::Value << subMesh.indexCount;
				out << YAML::Key << "IndexOffset" << YAML::Value << subMesh.indexOffset;
				out << YAML::Key << "VertexOffset" << YAML::Value << subMesh.vertexOffset;
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

		out << YAML::EndMap;

		std::string header = out.c_str();
		uint32_t headerSize = (uint32_t)header.size();
		stream.write(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
		stream.write(header.data(), headerSize);
		stream.write(reinterpret_cast<const char*>(m_mesh->GetVertices()), m_mesh->GetVerticesCount() * m_mesh->GetStride());
		stream.write(reinterpret_cast<const char*>(m_mesh->GetIndices().data()), m_mesh->GetIndices().size() * sizeof(uint32_t));
		stream.close();

		return true;
	}
}