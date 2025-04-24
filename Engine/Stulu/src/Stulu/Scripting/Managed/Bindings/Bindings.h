#pragma once
#include "Stulu/Scripting/EventCaller.h"
#include "Stulu/Scripting/Managed/Mono.h"

namespace StuluBindings {
	STULU_API extern Stulu::Ref<Stulu::AssemblyManager> getManager();
	STULU_API extern MonoDomain* getCoreDomain();
	STULU_API extern MonoImage* getCoreImage();
	STULU_API extern Stulu::Registry* GetCurrentRegistry();
	STULU_API extern void SetCurrentRegistry(Stulu::Registry* scene);

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

		static inline void FromEuler(struct Quaternion* outQuat, struct Vector3* euler) {
			glm::quat quat = Stulu::Math::EulerToQuaternion(euler->toNative());
			outQuat->w = quat.w;
			outQuat->x = quat.x;
			outQuat->y = quat.y;
			outQuat->z = quat.z;
		}
		static inline void ToEuler(struct Quaternion* quat, struct Vector3* outEuler) {
			glm::vec3 euler = Stulu::Math::QuaternionToEuler(quat->toNative());
			outEuler->x = euler.x;
			outEuler->y = euler.y;
			outEuler->z = euler.z;
		}
	};
	struct Matrix4 {
		float D00, D10, D20, D30;  // Column 0
		float D01, D11, D21, D31;  // Column 1
		float D02, D12, D22, D32;  // Column 2
		float D03, D13, D23, D33;  // Column 3

		glm::mat4 toNative() { return *((glm::mat4*)this); }
	};

	void LoadCoreBindings();
	void LoadComponentsBindings();
}