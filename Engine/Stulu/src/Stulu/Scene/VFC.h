#pragma once
#include "Stulu/Math/Math.h"
#include "Stulu/Core/Log.h"

namespace Stulu {
	class STULU_API Mesh;
	class STULU_API CameraComponent;
	class STULU_API TransformComponent;
	
	class STULU_API BoundingBox {
	public:
		BoundingBox() = default;
		BoundingBox(const AABB& aabb)
			: m_center{ (aabb.max + aabb.min) * 0.5f }, m_extents{ aabb.max.x - m_center.x, aabb.max.y - m_center.y, aabb.max.z - m_center.z },
			m_transformedCenter(m_center), m_transformedExtents(m_extents) {}
		BoundingBox(const glm::vec3& center, const glm::vec3& extents)
			: m_center(center), m_extents(extents),
			m_transformedCenter(m_center), m_transformedExtents(m_extents) {}

		void applyTransform(const TransformComponent& transform);

		inline bool isOnFrustum(const Frustum& camFrustum) const {
			return (isOnOrForwardPlan(camFrustum.leftFace) &&
				isOnOrForwardPlan(camFrustum.rightFace) &&
				isOnOrForwardPlan(camFrustum.topFace) &&
				isOnOrForwardPlan(camFrustum.bottomFace) &&
				isOnOrForwardPlan(camFrustum.nearFace) &&
				isOnOrForwardPlan(camFrustum.farFace));
		}
		bool isOnOrForwardPlan(const Plane& plan) const {
			// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
			const float r = m_transformedExtents.x * std::abs(plan.normal.x) + m_transformedExtents.y * std::abs(plan.normal.y) +
				m_transformedExtents.z * std::abs(plan.normal.z);
			return -r <= plan.getSignedDistanceToPlan(m_transformedCenter);
		}

		inline const glm::vec3& getCenter() const { return m_center; };
		inline const glm::vec3& getTransformedCenter() const { return m_transformedCenter; };
		inline const glm::vec3& getExtents() const { return m_extents; };
		inline const glm::vec3& getTransformedExtents() const { return m_transformedExtents; };
		inline void setTransformedCenter(const glm::vec3& transformedCenter) { m_transformedCenter = transformedCenter; }
		inline void setTransformedExtents(const glm::vec3& transformedExtents) { m_transformedExtents = transformedExtents; }
	private:
		glm::vec3 m_center = glm::vec3(.0f), m_extents = glm::vec3(.0f);
		glm::vec3 m_transformedCenter = glm::vec3(.0f), m_transformedExtents = glm::vec3(.0f);
	};

	class STULU_API VFC {
	public:
		static bool isInView(const Ref<BoundingBox>& boundingBox);
		static bool isInView(const BoundingBox* boundingBox);
		static Frustum setCamera(float aspect, float zNear, float zFar, float fovY, TransformComponent& transform);
		static Frustum setCamera(float aspect, float zNear, float zFar, float fovY, const glm::vec3& pos, const glm::quat& rotation);
		static inline void setCamera(const Frustum& frustum) { s_frustum = frustum; }
		static Frustum createFrustum(float aspect, float zNear, float zFar, float fovY, const TransformComponent& transform);
		static Frustum createFrustum(float aspect, float zNear, float zFar, float fovY, const glm::vec3& pos, const glm::quat& rotation);
		static Frustum createFrustum_ortho(float left, float right, float bottom, float top, float zNear, float zFar, const TransformComponent& transform);
		static Frustum createFrustum_ortho(float left, float right, float bottom, float top, float zNear, float zFar, const glm::vec3& pos, const glm::quat& rotation);

		static void setEnabled(bool value) { s_isEnabled = value; }
	private:
		static Frustum s_frustum;
		static bool s_isEnabled;
	};
}