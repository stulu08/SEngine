#pragma once
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Renderer/Renderer.h"

#include "Stulu/ScriptCore/Bindings/Types.h"

namespace StuluBindings {
	class Graphics {
	public:
		static inline void drawLine(struct Vector3* start, struct Vector3* end, struct Vector4* color) {
			Stulu::Renderer2D::drawLine(start->toNative_s(), end->toNative_s(), color->toNative_s());
		}
		static inline void drawQuad(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color) {
			Stulu::Renderer2D::drawQuad(
				Stulu::Math::createMat4(pos->toNative_s(), rotation->toNative_s(), scale->toNative_s()),
				color->toNative_s());
		}
		static inline void drawCircle(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color, float thickness, float fade) {
			Stulu::Renderer2D::drawCircle(
				Stulu::Math::createMat4(pos->toNative_s(), rotation->toNative_s(), scale->toNative_s()),
				color->toNative_s(), thickness, fade);
		}
		static inline void drawLineRect(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color) {
			Stulu::Renderer2D::drawLineRect(
				Stulu::Math::createMat4(pos->toNative_s(), rotation->toNative_s(), scale->toNative_s()),
				color->toNative_s());
		}
		static inline void drawLineCube(struct Vector3* pos, struct Quaternion* rotation, struct Vector3* scale, struct Vector4* color) {
			glm::mat4 transform = Stulu::Math::createMat4(pos->toNative_s(), rotation->toNative_s(), scale->toNative_s());
			static glm::vec4 vertexPositions[8]{
			{ -0.5f, -0.5f, -0.5f, 1.0f },
			{ 0.5f, -0.5f, -0.5f, 1.0f },
			{ 0.5f,  0.5f, -0.5f, 1.0f },
			{ -0.5f,  0.5f, -0.5f, 1.0f },
			{ -0.5f, -0.5f, 0.5f, 1.0f },
			{ 0.5f, -0.5f, 0.5f, 1.0f },
			{ 0.5f,  0.5f, 0.5f, 1.0f },
			{ -0.5f,  0.5f, 0.5f, 1.0f },
			};
			glm::vec3 lineVertices[8];
			for (size_t i = 0; i < 8; i++)
				lineVertices[i] = transform * vertexPositions[i];

			//back
			Stulu::Renderer2D::drawLine(lineVertices[0], lineVertices[1], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[1], lineVertices[2], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[2], lineVertices[3], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[3], lineVertices[0], color->toNative_s());
			//front
			Stulu::Renderer2D::drawLine(lineVertices[4], lineVertices[5], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[5], lineVertices[6], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[6], lineVertices[7], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[7], lineVertices[4], color->toNative_s());
			//connections between front and back
			Stulu::Renderer2D::drawLine(lineVertices[0], lineVertices[4], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[1], lineVertices[5], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[2], lineVertices[6], color->toNative_s());
			Stulu::Renderer2D::drawLine(lineVertices[3], lineVertices[7], color->toNative_s());
		}
	};
}