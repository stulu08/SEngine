#pragma once
#include "Stulu/Math/Math.h"
#include "Stulu/Core/Log.h"

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
		virtual void applyTransform(const TransformComponent& transform) = 0;
		virtual bool isOnFrustum(const Frustum& camFrustum) const = 0;
		virtual bool isOnOrForwardPlan(const Plane& plan) const = 0;
		virtual bool isRayIntersecting(const glm::vec3& pos, const glm::vec3& normal) const = 0;
		virtual BoundingBoxType getType() const = 0;
		virtual Ref<BoundingBox> copy() const = 0;
	};
	class STULU_API BoundingBoxAABB : public BoundingBox {
	public:
		BoundingBoxAABB() = default;
		BoundingBoxAABB(const AABB& aabb)
			: m_center{ (aabb.max + aabb.min) * 0.5f }, m_extents{ aabb.max.x - m_center.x, aabb.max.y - m_center.y, aabb.max.z - m_center.z }, m_aabb(aabb),
			m_transformedCenter(m_center), m_transformedExtents(m_extents) {}
		BoundingBoxAABB(const glm::vec3& center, const glm::vec3& extents)
			: m_center(center), m_extents(extents), m_aabb{ center - extents, center + extents },
			m_transformedCenter(m_center), m_transformedExtents(m_extents) {}

		virtual void applyTransform(const TransformComponent& transform) override;
		virtual bool isOnFrustum(const Frustum& camFrustum) const override;
		virtual bool isOnOrForwardPlan(const Plane& plan) const override;
		virtual bool isRayIntersecting(const glm::vec3& pos, const glm::vec3& normal) const override  {
			//https://gdbooks.gitbooks.io/3dcollisions/content/Chapter3/raycast_aabb.html
			float t1 = (m_aabb.min.x - pos.x) / normal.x;
			float t2 = (m_aabb.max.x - pos.x) / normal.x;
			float t3 = (m_aabb.min.y - pos.x) / normal.y;
			float t4 = (m_aabb.max.y - pos.x) / normal.y;
			float t5 = (m_aabb.min.z - pos.x) / normal.z;
			float t6 = (m_aabb.max.z - pos.x) / normal.z;

			float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
			float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

			// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
			if (tmax < 0) {
				return false;
			}

			// if tmin > tmax, ray doesn't intersect AABB
			if (tmin > tmax) {
				return false;
			}
			return true;
			/*
			if (tmin < 0f) {
				return tmax;
			}
			return tmin;
			*/
		}

		virtual BoundingBoxType getType() const override { return BoundingBoxType::AABB; }
		const glm::vec3& getCenter() const { return m_center; };
		const glm::vec3& getTransformedCenter() const { return m_transformedCenter; };
		const glm::vec3& getExtents() const { return m_extents; };
		const glm::vec3& getTransformedExtents() const { return m_transformedExtents; };
		const glm::vec3& getMin() const { return m_aabb.min; };
		const glm::vec3& getMax() const { return m_aabb.max; };
		const AABB& getAABB() const{ return m_aabb; };
		const AABB getTransformedAABB() const { return AABB{ m_transformedCenter - m_transformedExtents, m_transformedCenter + m_transformedExtents }; };
		virtual Ref<BoundingBox> copy() const {
			Ref<BoundingBoxAABB> result = createRef<BoundingBoxAABB>(m_center, m_extents);
			result->setTransformedCenter(m_transformedCenter);
			result->setTransformedExtents(m_transformedExtents);
			return result;
		};

		void setTransformedCenter(glm::vec3 transformedCenter) { m_transformedCenter = transformedCenter; }
		void setTransformedExtents(glm::vec3 transformedExtents) { m_transformedExtents = transformedExtents; }
	private:
		AABB m_aabb = {glm::vec3(.0f), glm::vec3(.0f) };
		glm::vec3 m_center = glm::vec3(.0f), m_extents = glm::vec3(.0f);
		glm::vec3 m_transformedCenter = glm::vec3(.0f), m_transformedExtents = glm::vec3(.0f);
	};
	class STULU_API VFC {
	public:
		static Ref<BoundingBox> createBoundingBox(const glm::vec3& center, const glm::vec3& extents);
		static Ref<BoundingBox> createBoundingBox(const AABB& aabb);
		static bool isInView(const Ref<BoundingBox>& boundingBox);
		static bool isInView(const BoundingBox* boundingBox);
		static Frustum setCamera(float aspect, float zNear, float zFar, float fovY, TransformComponent& transform);
		static Frustum setCamera(float aspect, float zNear, float zFar, float fovY, const glm::vec3& pos, const glm::quat& rotation);
		static inline void setCamera(const Frustum& frustum) { s_frustum = frustum; }
		static Frustum createFrustum(float aspect, float zNear, float zFar, float fovY, TransformComponent& transform);
		static Frustum createFrustum(float aspect, float zNear, float zFar, float fovY, const glm::vec3& pos, const glm::quat& rotation);
		static Frustum createFrustum_ortho(float left, float right, float bottom, float top, float zNear, float zFar, TransformComponent& transform);
		static Frustum createFrustum_ortho(float left, float right, float bottom, float top, float zNear, float zFar, const glm::vec3& pos, const glm::quat& rotation);

		static void setEnabled(bool value) { s_isEnabled = value; }
	private:
		static Frustum s_frustum;
		static bool s_isEnabled;
		static BoundingBoxType s_mode;
	};
}