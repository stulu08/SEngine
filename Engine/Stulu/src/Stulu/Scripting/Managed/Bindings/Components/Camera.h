#pragma once
#include "Stulu/Scripting/Managed/Bindings/Bindings.h"
#include "Stulu/Scene/Components/Camera.h"
namespace StuluBindings {
	class CameraComponent {
	public:
		static float cameraComp_GetFov(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return go.getComponent<Stulu::CameraComponent>().GetFov();
		}
		static void cameraComp_SetFov(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CameraComponent>().SetFov(value);
		}
		static float cameraComp_GetFar(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return go.getComponent<Stulu::CameraComponent>().GetFar();
		}
		static void cameraComp_SetFar(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CameraComponent>().SetFar(value);
		}
		static int cameraComp_GetClearType(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return (int)go.getComponent<Stulu::CameraComponent>().GetClearType();
		}
		static void cameraComp_SetClearType(uint64_t entityId, int value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CameraComponent>().SetClearType((Stulu::ClearType)value);
		}
		static int cameraComp_GetSamples(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return (int)go.getComponent<Stulu::CameraComponent>().GetSamples();
		}
		static void cameraComp_SetSamples(uint64_t entityId, int value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CameraComponent>().SetSamples((Stulu::MSAASamples)value);
		}
		static float cameraComp_GetNear(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return go.getComponent<Stulu::CameraComponent>().GetNear();
		}
		static void cameraComp_SetNear(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CameraComponent>().SetNear(value);
		}
		static void cameraComp_Update(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			go.getComponent<Stulu::CameraComponent>().UpdateCamera();
		}
		static float cameraComp_GetAspect(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentRegistry() };
			return go.getComponent<Stulu::CameraComponent>().GetAspect();
		}
	};
}