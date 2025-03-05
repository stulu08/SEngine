#pragma once
#include "Stulu/Scripting/EventCaller.h"
#include "Stulu/Scripting/Managed/Mono.h"

namespace StuluBindings {
	Stulu::Ref<Stulu::AssemblyManager> getManager();
	MonoDomain* getCoreDomain();
	MonoImage* getCoreImage();

	struct Vector2 {
		float x, y;

		glm::vec2 toNative() { return *((glm::vec2*)this); }
		glm::vec2 toNative_s() { return glm::vec2(x, y); }
	};
	struct Vector3 {
		float x, y, z;

		glm::vec3 toNative() { return *((glm::vec3*)this); }
		glm::vec3 toNative_s() { return glm::vec3(x, y, z); }
	};
	struct Vector4 {
		float x, y, z, w;

		glm::vec4 toNative() { return *((glm::vec4*)this); }
		glm::vec4 toNative_s() { return glm::vec4(x, y, z, w); }
	};
	struct Quaternion {
		float x, y, z, w;

		glm::quat toNative() { return glm::quat(w, x, y, z); }
		glm::quat toNative_s() { return glm::quat(w, x, y, z); }
	};

	void LoadCoreBindings();
	void LoadComponentsBindings();

	STULU_API extern Stulu::Scene* GetCurrentScene();
	STULU_API extern void SetCurrentScene(Stulu::Scene* scene);
}