#pragma once
#include "Stulu/ImGui/Gizmo.h"

namespace StuluBindings {
	class Gizmo {
	public:
		static inline void drawLine(struct Vector3* start, struct Vector3* end, struct Vector4* color) {
			Stulu::Gizmo::drawLine(start->toNative_s(), end->toNative_s(), color->toNative_s());
		}
		static inline void drawRect(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color) {
			Stulu::Gizmo::drawRect(
				Stulu::Math::createMat4(pos->toNative_s(), rotation->toNative_s(), scale->toNative_s()),
				color->toNative_s());
		}
		static inline void drawOutlineCube(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color) {
			Stulu::Gizmo::drawOutlineCube(
				Stulu::Math::createMat4(pos->toNative_s(), rotation->toNative_s(), scale->toNative_s()),
				color->toNative_s());
		}
		static inline void drawCircle(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color, float thickness, float fade) {
			Stulu::Gizmo::drawCircle(
				pos->toNative_s(), scale->toNative_s(), rotation->toNative_s(),
				color->toNative_s(), thickness, fade);
		}
		static inline void drawCube(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color) {
			Stulu::Gizmo::drawCube(
				Stulu::Math::createMat4(pos->toNative_s(), rotation->toNative_s(), scale->toNative_s()),
				color->toNative_s());
		}
		static inline void drawSphere(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color) {
			Stulu::Gizmo::drawSphere(
				Stulu::Math::createMat4(pos->toNative_s(), rotation->toNative_s(), scale->toNative_s()),
				color->toNative_s());
		}
		static inline void drawRectMatrix(struct Matrix4* matrix, struct Vector4* color) {
			Stulu::Gizmo::drawRect(matrix->toNative(), color->toNative_s());
		}
	};
}