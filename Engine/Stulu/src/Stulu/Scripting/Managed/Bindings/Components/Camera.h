#pragma once
#include "Stulu/Scripting/Managed/Bindings/Bindings.h"

namespace StuluBindings {
	class CameraComponent {
	public:
		static float cameraComp_GetFov(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentScene() };
			return go.getComponent<Stulu::CameraComponent>().settings.fov;
		}
		static void cameraComp_SetFov(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentScene() };
			go.getComponent<Stulu::CameraComponent>().settings.fov = value;
		}
		static float cameraComp_GetFar(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentScene() };
			return go.getComponent<Stulu::CameraComponent>().settings.zFar;
		}
		static void cameraComp_SetFar(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentScene() };
			go.getComponent<Stulu::CameraComponent>().settings.zFar = value;
		}
		static float cameraComp_GetNear(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentScene() };
			return go.getComponent<Stulu::CameraComponent>().settings.zNear;
		}
		static void cameraComp_SetNear(uint64_t entityId, float value) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentScene() };
			go.getComponent<Stulu::CameraComponent>().settings.zNear = value;
		}
		static void cameraComp_Update(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentScene() };
			go.getComponent<Stulu::CameraComponent>().updateMode();
			go.getComponent<Stulu::CameraComponent>().updateProjection();
			go.getComponent<Stulu::CameraComponent>().updateSize();
		}
		static float cameraComp_GetAspect(uint64_t entityId) {
			Stulu::GameObject go = { (entt::entity)entityId, GetCurrentScene() };
			return go.getComponent<Stulu::CameraComponent>().settings.aspectRatio;
		}
	};
}