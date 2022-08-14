#pragma once
#include "Stulu/Math/Math.h"

namespace Stulu {
	class STULU_API Mesh;
	class STULU_API CameraComponent;
	class STULU_API TransformComponent;
	enum class BoundingBoxType {
		None = 0,
		AABB = 1,
	};
	class STULU_API BoundingBox {
	public:
		virtual bool isOnFrustum(const Frustum& camFrustum, const TransformComponent& transform) const = 0;
		virtual bool isOnOrForwardPlan(const Plane& plan) const = 0;

		virtual BoundingBoxType getType() const = 0;
	};
	class STULU_API BoundingBoxAABB : public BoundingBox{
	public:
		BoundingBoxAABB(const Mesh* mesh);
		BoundingBoxAABB(const AABB& aabb)
			: m_center{ (aabb.max + aabb.min) * 0.5f }, m_extents{ aabb.max.x - m_center.x, aabb.max.y - m_center.y, aabb.max.z - m_center.z }, m_aabb(aabb) {}
		BoundingBoxAABB(const glm::vec3& center, const glm::vec3& extents)
			: m_center(center), m_extents(extents), m_aabb{ center - extents, center + extents } {}

		virtual bool isOnFrustum(const Frustum& camFrustum, const TransformComponent& transform) const override;
		virtual bool isOnOrForwardPlan(const Plane& plan) const override;

		virtual BoundingBoxType getType() const override { return BoundingBoxType::AABB; }
		const glm::vec3& getCenter() const { return m_center; };
		const glm::vec3& getExtents() const { return m_extents; };
		const glm::vec3& getMin() const { return m_aabb.min; };
		const glm::vec3& getMax() const { return m_aabb.max; };
		const AABB& getAABB() const{ return m_aabb; };

	private:
		AABB m_aabb = {glm::vec3(.0f), glm::vec3(.0f) };
		glm::vec3 m_center = glm::vec3(.0f), m_extents = glm::vec3(.0f);
	};
	class STULU_API VFC {
	public:
		static Ref<BoundingBox> createBoundingBox(const Mesh* mesh);
		static bool isInView(const Ref<BoundingBox>& boundingBox, const TransformComponent& transform);
		static Frustum setCamera(float aspect, float zNear, float zFar, float fovY, TransformComponent& transform);

		static void setEnabled(bool value) { s_isEnabled = value; }
	private:
		static inline Frustum s_frustum;
		static inline bool s_isEnabled = true;
		static inline BoundingBoxType s_mode = BoundingBoxType::AABB;
	};
}