#pragma once
//_*
// C for cast
// P for pointer
// D custome datatype
#define SERIALIZE(obj, field) out << YAML::Key << #field << YAML::Value << obj.field;
#define SERIALIZE_PROPERTY(obj, field) out << YAML::Key << #field << YAML::Value << obj.field;
#define SERIALIZE_PROPERTY_C(obj, field, cast) out << YAML::Key << #field << YAML::Value << (cast)obj.field;
#define SERIALIZE_PROPERTY_P(obj, field) out << YAML::Key << #field << YAML::Value << obj->field;
#define SERIALIZE_PROPERTY_PC(obj, field, cast) out << YAML::Key << #field << YAML::Value << (cast)obj->field;


#define SERIALIZE_PROPERTY_IFEXIST(obj, field) \
if (obj.field) { \
	out << YAML::Key << #field << YAML::Value << obj.field; \
}
#define SERIALIZE_PROPERTY_IFEXIST_C(obj, field, cast) \
if (obj.field) { \
	out << YAML::Key << #field << YAML::Value << (cast)obj.field; \
}

#define SERIALIZE_BEGINMAP(name) out << YAML::Key << name << YAML::Value << YAML::BeginMap;
#define SERIALIZE_ENDMAP() out << YAML::EndMap;

#define SERIALIZE_ENUM(obj, field) SERIALIZE_PROPERTY_C(obj, field, int)
#define SERIALIZE_ENUMPROP(obj, field) SERIALIZE_PROPERTY_C(obj, field, int)
#define SERIALIZE_UUIDPROP(obj, field) SERIALIZE_PROPERTY_C(obj, field, uint64_t)
#define SERIALIZE_TEXTURE(obj, field)\
if (obj.field) { \
	out << YAML::Key << #field << YAML::Value << (uint64_t)obj.field->uuid; \
}
#define SERIALIZE_MATERIAL(obj, field)\
if (obj.field) { \
	out << YAML::Key << #field << YAML::Value << (uint64_t)obj.field->getUUID(); \
}
#define SERIALIZE_MESH(obj, field) out << YAML::Key << #field << YAML::Value << (uint64_t)obj.field.uuid;

#define BEGIN_SERIALIZE_COMPONENT(comp) \
if (gameObject.hasComponent<comp>()) { \
const comp& Serialized##comp = gameObject.getComponent<comp>();\
out << YAML::Key << #comp;\
out << YAML::BeginMap; \

#define END_SERIALIZE_COMPONENT() \
out << YAML::EndMap; \
}

#define DESERIALIZE_PROPERTY_D(obj, field, datatype)  \
if (nodeStack.back().second[#field]) \
	obj.field = nodeStack.back().second[#field].as<datatype>();
#define DESERIALIZE_PROPERTY_DC(obj, field, cast, datatype)  \
if (nodeStack.back().second[#field]) \
	obj.field = (cast)nodeStack.back().second[#field].as<datatype>();

#define DESERIALIZE_PROPERTY_DP(obj, field, datatype)  \
if (nodeStack.back().second[#field]) \
	obj->field = nodeStack.back().second[#field].as<datatype>();
#define DESERIALIZE_PROPERTY_DPC(obj, field, cast)  \
if (nodeStack.back().second[#field]) \
	obj->field = (cast)nodeStack.back().second[#field].as<datatype>();

#define DESERIALIZE_D(obj, field, from, datatype)  \
if (from[#field]) \
	obj.field = from[#field].as<datatype>();
#define DESERIALIZE_DC(obj, field, from, cast, datatype)  \
if (from[#field]) \
	obj.field = (cast)from[#field].as<datatype>();
#define DESERIALIZE(obj, field, from) DESERIALIZE_D(obj, field, from, decltype(obj.field))
#define DESERIALIZE_C(obj, field, from, cast) DESERIALIZE_DC(obj, field, from, decltype(obj.field), cast)


#define DESERIALIZE_PROPERTY(obj, field) DESERIALIZE_PROPERTY_D(obj, field, decltype(obj.field))
#define DESERIALIZE_PROPERTY_C(obj, field, cast) DESERIALIZE_PROPERTY_DC(obj, field, cast, decltype(obj.field))
#define DESERIALIZE_PROPERTY_P(obj, field) DESERIALIZE_PROPERTY_DP(obj, field, decltype(obj.field))
#define DESERIALIZE_PROPERTY_PC(obj, field, cast) DESERIALIZE_PROPERTY_DPC(obj, field, cast, decltype(obj.field))

#define DESERIALIZE_ENUMPROP(obj, field) DESERIALIZE_PROPERTY_DC(obj, field, decltype(obj.field), int)
#define DESERIALIZE_UUIDPROP(obj, field) DESERIALIZE_PROPERTY_DC(obj, field, UUID, uint64_t);
#define DESERIALIZE_ASSETSMANAGER(obj, field, type, dataType) \
if (nodeStack.back().second[#field]) { \
	UUID id = nodeStack.back().second[#field].as<uint64_t>(); \
	if (AssetsManager::existsAndType(id, type)) \
		obj.field = std::any_cast<dataType>(AssetsManager::get(id).data); \
}
#define DESERIALIZE_ASSETSMANAGER_C(obj, field, type, dataType, cast) \
if (nodeStack.back().second[#field]) { \
	UUID id = nodeStack.back().second[#field].as<uint64_t>(); \
	if (AssetsManager::existsAndType(id, type)) \
		obj.field = std::dynamic_pointer_cast<cast>(std::any_cast<dataType>(AssetsManager::get(id).data)); \
}
#define DESERIALIZE_TEXTURE(obj, field) DESERIALIZE_ASSETSMANAGER(obj, field, AssetType::Texture2D, Ref<Texture>)
#define DESERIALIZE_MATERIAL(obj, field) DESERIALIZE_ASSETSMANAGER(obj, field, AssetType::Material, Ref<Material>)
#define DESERIALIZE_MESH(obj, field) DESERIALIZE_ASSETSMANAGER(obj, field, AssetType::Mesh, MeshAsset)
#define DESERIALIZE_SKYBOX(obj, field) DESERIALIZE_ASSETSMANAGER_C(obj, field, AssetType::SkyBox, Ref<Texture>, SkyBox)

#define DESERIALIZE_ENUMPROP_P(obj, field) DESERIALIZE_PROPERTY_DPC(obj, field, decltype(obj.field), int)
#define DESERIALIZE_UUIDPROP_P(obj, field) DESERIALIZE_PROPERTY_DPC(obj, field, UUID, uint64_t);

#define BEGIN_DESERIALIZE_COMPONENT(comp) { \
auto componentNode = gameObject[#comp]; \
if (componentNode) { \
	std::vector<std::pair<std::string, YAML::Node>> nodeStack; \
	nodeStack.push_back( { #comp, componentNode } ); \
	try {\
	comp& Added##comp = deserialized.saveAddComponent<comp>();

#define END_DESERIALIZE_COMPONENT() }\
catch (YAML::Exception ex) { \
	CORE_ERROR("{1}: YAML exception while deserialzing \"{3}\" at {2}: {0}", ex.what(), path, ex.mark.line, nodeStack.back().first) \
}\
}\
}

#define DESERIALIZE_BEGINMAP(name) { \
nodeStack.push_back( { name, nodeStack.back().second[name] } ); \
if(nodeStack.back().second) {

#define DESERIALIZE_ENDMAP() \
}else {CORE_ERROR("Cant open map: {0}", nodeStack.back().first)}\
nodeStack.pop_back(); \
}