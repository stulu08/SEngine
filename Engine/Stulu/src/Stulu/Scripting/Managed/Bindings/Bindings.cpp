#include "st_pch.h"
#include "Bindings.h"

#include <Stulu.h>

#include "Components/GameObject.h"
#include "Components/SpriteRenderer.h"
#include "Components/Transform.h"
#include "Components/ScriptingComponent.h"
#include "Components/Camera.h"
#include "Components/MeshRendererComponent.h"

#include "Core/Application.h"
#include "Core/Folders.h"
#include "Core/Time.h"
#include "Core/Gizmo.h"
#include "Core/Graphics2D.h"
#include "Core/Input.h"
#include "Core/Log.h"
#include "Core/Texture2D.h"
#include "Core/Asset.h"
#include "Core/Material.h"
#include "Core/Mesh.h"

namespace StuluBindings {
	STULU_API Stulu::Ref<Stulu::AssemblyManager> getManager() {
		return Stulu::Application::get().getAssemblyManager();
	}
	STULU_API MonoDomain* getCoreDomain() {
		return getManager()->getCoreDomain();
	}
	STULU_API MonoImage* getCoreImage() {
		return Stulu::Application::get().getScriptCoreAssembly()->getImage();
	}
	
	static Stulu::Registry* s_activeRegistry;
	STULU_API Stulu::Registry* GetCurrentRegistry() {
		return s_activeRegistry;
	}
	STULU_API void SetCurrentRegistry(Stulu::Registry* scene) {
		s_activeRegistry = scene;
	}

	#define add_call(Name, _Binding) manager->RegisterFunction(std::string("Stulu.InternalCalls::") + Name, StuluBindings::_Binding)
	#define _add_call(Name, _Binding) manager->RegisterFunction(std::string("Stulu.InternalCalls::") + Name, _Binding)


	void LoadCoreBindings() {
		Stulu::Ref<Stulu::AssemblyManager> manager = getManager();

		add_call("application_exit(int)", Application::exit);
		add_call("application_getWidth()", Application::getWidth);
		add_call("application_getHeight()", Application::getHeight);

		add_call("log_engine_log(int,string)", Log::engine_log);
		add_call("log_client_log(int,string)", Log::client_log);

		add_call("time_getFrameTime()", Time::time_frame);
		add_call("time_getDeltaTime()", Time::time_delta);
		add_call("time_getApplicationRuntime()", Time::time_runtime);
		add_call("time_getScale()", Time::time_scale);
		add_call("time_getTime()", Time::time_time);
		add_call("time_setScale(single)", Time::time_setScale);

		add_call("input_getKeyDown(uint)", Input::getKeyDown);
		add_call("input_getKeyUp(uint)", Input::getKeyUp);
		add_call("input_getKey(uint)", Input::getKey);
		add_call("input_getMouseDown(uint)", Input::getMouseDown);
		add_call("input_getMouseUp(uint)", Input::getMouseUp);
		add_call("input_getMouse(uint)", Input::getMouse);
		add_call("input_setCursorMode(uint)", Input::setCursorMode);
		add_call("input_getCursorMode()", Input::getCursorMode);
		add_call("input_getMouseDelta(Stulu.Vector2&)", Input::getMouseDelta);
		add_call("input_getMousePos(Stulu.Vector2&)", Input::getMousePos);

		add_call("renderer2D_drawLine(Stulu.Vector3&,Stulu.Vector3&,Stulu.Vector4&)", Graphics2D::drawLine);
		add_call("renderer2D_drawQuad(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Graphics2D::drawQuad);
		add_call("renderer2D_drawTexturedQuad(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&,Stulu.Vector2&,ulong)", Graphics2D::drawQuadTexture);
		add_call("renderer2D_drawCircle(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&,single,single)", Graphics2D::drawCircle);
		add_call("renderer2D_drawLineRect(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Graphics2D::drawLineRect);
		add_call("renderer2D_drawLineCube(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Graphics2D::drawLineCube);

		add_call("gizmo_drawLine(Stulu.Vector3&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawLine);
		add_call("gizmo_drawRect(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawRect);
		add_call("gizmo_drawRect(Stulu.Matrix4&,Stulu.Vector4&)", Gizmo::drawRectMatrix);
		add_call("gizmo_drawOutlineCube(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawOutlineCube);
		add_call("gizmo_drawCircle(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&,single,single)", Gizmo::drawCircle);
		add_call("gizmo_drawCube(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawCube);
		add_call("gizmo_drawSphere(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawSphere);

		manager->RegisterFunction("Stulu.Texture2D::GetWidth(ulong)", Texture2D::getWidth);
		manager->RegisterFunction("Stulu.Texture2D::GetHeight(ulong)", Texture2D::getHeight);
		manager->RegisterFunction("Stulu.Texture2D::GetPath(ulong)", Texture2D::getPath);
		manager->RegisterFunction("Stulu.Texture2D::Findbypath(string)", Texture2D::findByPath);
		manager->RegisterFunction("Stulu.Texture2D::GetWhiteTexture()", Texture2D::getWhite);
		manager->RegisterFunction("Stulu.Texture2D::GetBlackTexture()", Texture2D::getBlack);
		manager->RegisterFunction("Stulu.Texture2D::GetTextureSettings(ulong,Stulu.TextureSettings&)", Texture2D::GetTextureSettings);
		manager->RegisterFunction("Stulu.Texture2D::SetTextureSettings(ulong,Stulu.TextureSettings&)", Texture2D::SetTextureSettings);

		add_call("quaternion_fromEuler(Stulu.Quaternion&,Stulu.Vector3&)", Quaternion::FromEuler);
		add_call("quaternion_toEuler(Stulu.Quaternion&,Stulu.Vector3&)", Quaternion::ToEuler);

		add_call("folders_assetPath()", Folders::AssetsPath);

		manager->RegisterFunction("Stulu.AssetHandle::DropAssetHandle(uint,ulong)", AssetHandle::ReleaseHandle);
		manager->RegisterFunction("Stulu.AssetHandle::InitAssetHandle(object,ulong)", AssetHandle::InitHandle);
		manager->RegisterFunction("Stulu.AssetHandle::GetAssetLoaded(ulong)", AssetHandle::GetLoaded);
		manager->RegisterFunction("Stulu.AssetHandle::GetAssetPath(ulong)", AssetHandle::GetPath);

		manager->RegisterFunction("Stulu.Mesh::GetNameInternal(ulong)", Mesh::GetNameInternal);
		manager->RegisterFunction("Stulu.Mesh::SetNameInternal(ulong,string)", Mesh::SetNameInternal);
		manager->RegisterFunction("Stulu.Mesh::GetSubMeshCountInternal(ulong)", Mesh::GetSubMeshCountInternal);
		manager->RegisterFunction("Stulu.Mesh::GetSubMeshInternal(ulong,Stulu.SubMeshDescription&,int)", Mesh::GetSubMeshInternal);
		manager->RegisterFunction("Stulu.Mesh::SetSubMeshInternal(ulong,Stulu.SubMeshDescription&,int)", Mesh::SetSubMeshInternal);
		manager->RegisterFunction("Stulu.Mesh::SetVerticesInternal(ulong,Stulu.NativeArray`1<Stulu.Vertex>&)", Mesh::SetVerticesInternal);
		manager->RegisterFunction("Stulu.Mesh::GetVerticesInternal(ulong,Stulu.NativeArray`1<Stulu.Vertex>&)", Mesh::GetVerticesInternal);
		manager->RegisterFunction("Stulu.Mesh::SetIndicesInternal(ulong,Stulu.NativeArray`1<uint>&)", Mesh::SetIndicesInternal);
		manager->RegisterFunction("Stulu.Mesh::GetIndicesInternal(ulong,Stulu.NativeArray`1<uint>&)", Mesh::GetIndicesInternal);
		manager->RegisterFunction("Stulu.Mesh::CalculateBoundsInternal(ulong)", Mesh::CalculateBoundsInternal);
		manager->RegisterFunction("Stulu.Mesh::CalculateNormalsInternal(ulong,bool)", Mesh::CalculateNormalsInternal);
		manager->RegisterFunction("Stulu.Mesh::CreateVertexMeshInternal(ulong)", Mesh::CreateVertexMeshInternal);

		manager->RegisterFunction("Stulu.Material::GetNameInternal(ulong)", Material::GetName);
		manager->RegisterFunction("Stulu.Material::HasPropertyInternal(ulong,string)", Material::HasProperty);
		manager->RegisterFunction("Stulu.Material::GetPropertyCountIntenal(ulong)", Material::PropertyCount);
		manager->RegisterFunction("Stulu.Material::GetPropertyTypeIntenal(ulong,int)", Material::GetPropertyType);
		manager->RegisterFunction("Stulu.Material::GetPropertyNameIntenal(ulong,int)", Material::GetPropertyName);
		manager->RegisterFunction("Stulu.Material::GetPropertyValueFloat(ulong,string)",Material::GetPropertyValue<Stulu::MaterialFloatProperty>);
		manager->RegisterFunction("Stulu.Material::SetPropertyValueFloat(ulong,string,single&)",Material::SetPropertyValue<Stulu::MaterialFloatProperty>);
		manager->RegisterFunction("Stulu.Material::GetPropertyValueFloat2(ulong,string)",Material::GetPropertyValue<Stulu::MaterialFloat2Property>);
		manager->RegisterFunction("Stulu.Material::SetPropertyValueFloat2(ulong,string,Stulu.Vector2&)",Material::SetPropertyValue<Stulu::MaterialFloat2Property>);
		manager->RegisterFunction("Stulu.Material::GetPropertyValueFloat3(ulong,string)",Material::GetPropertyValue<Stulu::MaterialFloat3Property>);
		manager->RegisterFunction("Stulu.Material::SetPropertyValueFloat3(ulong,string,Stulu.Vector3&)",Material::SetPropertyValue<Stulu::MaterialFloat3Property>);
		manager->RegisterFunction("Stulu.Material::GetPropertyValueFloat4(ulong,string)",Material::GetPropertyValue<Stulu::MaterialFloat4Property>);
		manager->RegisterFunction("Stulu.Material::SetPropertyValueFloat4(ulong,string,Stulu.Vector4&)",Material::SetPropertyValue<Stulu::MaterialFloat4Property>);
		manager->RegisterFunction("Stulu.Material::GetPropertyValueInt(ulong,string)",Material::GetPropertyValue<Stulu::MaterialIntProperty>);
		manager->RegisterFunction("Stulu.Material::SetPropertyValueInt(ulong,string,int&)",Material::SetPropertyValue<Stulu::MaterialIntProperty>);
		manager->RegisterFunction("Stulu.Material::GetPropertyValueUInt(ulong,string)",Material::GetPropertyValue<Stulu::MaterialUIntProperty>);
		manager->RegisterFunction("Stulu.Material::SetPropertyValueUInt(ulong,string,uint&)",Material::SetPropertyValue<Stulu::MaterialUIntProperty>);
		manager->RegisterFunction("Stulu.Material::GetPropertyValueColor(ulong,string)",Material::GetPropertyValue<Stulu::MaterialColorProperty>);
		manager->RegisterFunction("Stulu.Material::SetPropertyValueColor(ulong,string,Stulu.MaterialColor&)",Material::SetPropertyValue<Stulu::MaterialColorProperty>);
		manager->RegisterFunction("Stulu.Material::GetPropertyValueSampler2D(ulong,string)",Material::GetPropertyValueSampler<Stulu::MaterialSampler2DProperty>);
		manager->RegisterFunction("Stulu.Material::SetPropertyValueSampler2D(ulong,string,ulong&)", Material::SetPropertyValueSampler<Stulu::MaterialSampler2DProperty>);

	}
	void LoadComponentsBindings() {
		Stulu::Ref<Stulu::AssemblyManager> manager = getManager();
		
		add_call("gameObject_create(string,string,Stulu.Vector3,Stulu.Quaternion,Stulu.Vector3)", GameObject::create);
		add_call("gameObject_add_component(ulong,System.Type)", GameObject::addComponent);
		add_call("gameObject_has_component(ulong,System.Type)", GameObject::hasComponent);
		add_call("gameObject_remove_component(ulong,System.Type)", GameObject::removeComponent);
		add_call("gameObject_get_component(ulong,System.Type)", GameObject::getComponent);
		add_call("gameObject_getName(ulong)", GameObject::getName);
		add_call("gameObject_getTag(ulong)", GameObject::getTag);
		add_call("gameObject_setName(ulong,string)", GameObject::setName);
		add_call("gameObject_setTag(ulong,string)", GameObject::setTag);
		add_call("gameObject_createSphere(string,string,Stulu.Vector3)", GameObject::createSphere);
		add_call("gameObject_createCube(string,string,Stulu.Vector3)", GameObject::createCube);
		add_call("gameObject_createCapsule(string,string,Stulu.Vector3)", GameObject::createCapsule);
		add_call("gameObject_createPlane(string,string,Stulu.Vector3)", GameObject::createPlane);
		add_call("gameObject_destroy(ulong)", GameObject::destroy);

		add_call("transformComp_setPos(ulong,Stulu.Vector3&)", Transform::setPos);
		add_call("transformComp_getPos(ulong,Stulu.Vector3&)", Transform::getPos);
		add_call("transformComp_getWorldPos(ulong,Stulu.Vector3&)", Transform::getWorldPos);
		add_call("transformComp_setRotation(ulong,Stulu.Quaternion&)", Transform::setRotation);
		add_call("transformComp_getRotation(ulong,Stulu.Quaternion&)", Transform::getRotation);
		add_call("transformComp_getWorldRotation(ulong,Stulu.Quaternion&)", Transform::getWorldRotation);
		add_call("transformComp_setScale(ulong,Stulu.Vector3&)", Transform::setScale);
		add_call("transformComp_getScale(ulong,Stulu.Vector3&)", Transform::getScale);
		add_call("transformComp_getWorldScale(ulong,Stulu.Vector3&)", Transform::getWorldScale);
		add_call("transformComp_forward(ulong,Stulu.Vector3&)", Transform::forward);
		add_call("transformComp_right(ulong,Stulu.Vector3&)", Transform::right);
		add_call("transformComp_up(ulong,Stulu.Vector3&)", Transform::up);

		add_call("spriteRenComp_getTexture(ulong)", SpriteRendererComponent::getTexture);
		add_call("spriteRenComp_setTexture(ulong,ulong)", SpriteRendererComponent::setTexture);
		add_call("spriteRenComp_getTiling(ulong,Stulu.Vector2&)", SpriteRendererComponent::getTiling);
		add_call("spriteRenComp_setTiling(ulong,Stulu.Vector2&)", SpriteRendererComponent::setTiling);
		add_call("spriteRenComp_getColor(ulong,Stulu.Vector4&)", SpriteRendererComponent::getColor);
		add_call("spriteRenComp_setColor(ulong,Stulu.Vector4&)", SpriteRendererComponent::setColor);

		add_call("scriptingComp_fetchScripts(ulong)", ScriptingComponent::FetchObjectArray);

		add_call("cameraComp_fov(ulong)", CameraComponent::cameraComp_GetFov);
		add_call("cameraComp_fov(ulong,single)", CameraComponent::cameraComp_SetFov);
		add_call("cameraComp_far(ulong)", CameraComponent::cameraComp_GetFar);
		add_call("cameraComp_far(ulong,single)", CameraComponent::cameraComp_SetFar);
		add_call("cameraComp_clearType(ulong)", CameraComponent::cameraComp_GetClearType);
		add_call("cameraComp_clearType(ulong,int)", CameraComponent::cameraComp_SetClearType);
		add_call("cameraComp_samples(ulong)", CameraComponent::cameraComp_GetSamples);
		add_call("cameraComp_samples(ulong,int)", CameraComponent::cameraComp_SetSamples);
		add_call("cameraComp_near(ulong)", CameraComponent::cameraComp_GetNear);
		add_call("cameraComp_near(ulong,single)", CameraComponent::cameraComp_SetNear);
		add_call("cameraComp_update(ulong)", CameraComponent::cameraComp_Update);
		add_call("cameraComp_aspect(ulong)", CameraComponent::cameraComp_GetAspect);

		manager->RegisterFunction("Stulu.MeshRendererComponent::SetMaterialInternal(ulong,ulong,int)", MeshRendererComponent::SetMaterialInternal);
		manager->RegisterFunction("Stulu.MeshRendererComponent::GetMaterialInternal(ulong,int)", MeshRendererComponent::GetMaterialInternal);
		manager->RegisterFunction("Stulu.MeshRendererComponent::GetMaterialCount(ulong)", MeshRendererComponent::GetMaterialCount);
		manager->RegisterFunction("Stulu.MeshRendererComponent::SetMaterialCount(ulong,int)", MeshRendererComponent::SetMaterialCount);
		manager->RegisterFunction("Stulu.MeshRendererComponent::SetCullModeInternal(ulong,uint)", MeshRendererComponent::SetCullModeInternal);
		manager->RegisterFunction("Stulu.MeshRendererComponent::GetCullModeInternal(ulong)", MeshRendererComponent::GetCullModeInternal);

		manager->RegisterFunction("Stulu.MeshFilterComponent::SetMeshInternal(ulong,ulong)", MeshFilterComponent::SetMeshInternal);
		manager->RegisterFunction("Stulu.MeshFilterComponent::GetMeshInternal(ulong)", MeshFilterComponent::GetMeshInternal);

	}

}