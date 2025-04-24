#include "st_pch.h"
#include "VFC.h"

#include "Stulu/Scene/Components/Camera.h"
#include "Stulu/Scene/Components/Components.h"

namespace Stulu {

    Frustum VFC::s_frustum;
    bool VFC::s_isEnabled = true;
    BoundingBoxType VFC::s_mode = BoundingBoxType::AABB;

    Ref<BoundingBox> VFC::createBoundingBox(const glm::vec3& center, const glm::vec3& extents) {
        switch (s_mode)
        {
        case Stulu::BoundingBoxType::None:
            break;
        case Stulu::BoundingBoxType::AABB:
            return createRef<BoundingBoxAABB>(center, extents);
        }
        CORE_ERROR("Could not create BoundingBox!");
        return nullptr;
    }
    Ref<BoundingBox> VFC::createBoundingBox(const AABB& aabb) {
        switch (s_mode)
        {
        case Stulu::BoundingBoxType::None:
            break;
        case Stulu::BoundingBoxType::AABB:
            return createRef<BoundingBoxAABB>(aabb);
        }
        CORE_ERROR("Could not create BoundingBox!");
        return nullptr;
    }


    bool VFC::isInView(const Ref<BoundingBox>& boundingBox) {
        if (!s_isEnabled)
            return true;
        return boundingBox->isOnFrustum(s_frustum);
    }
    bool VFC::isInView(const BoundingBox* boundingBox) {
        if (!s_isEnabled)
            return true;
        return boundingBox->isOnFrustum(s_frustum);
    }
    Frustum VFC::setCamera(float aspect, float zNear, float zFar, float DegfovY, TransformComponent& tc) {
        s_frustum = createFrustum(aspect, zNear, zFar, DegfovY, tc);
        return s_frustum;
    }
    Frustum VFC::setCamera(float aspect, float zNear, float zFar, float DegfovY, const glm::vec3& pos, const glm::quat& rotation) {
        s_frustum = createFrustum(aspect, zNear, zFar, DegfovY, pos, rotation);
        return s_frustum;
    }
    Frustum VFC::createFrustum(float aspect, float zNear, float zFar, float DegfovY, TransformComponent& tc) {
        Frustum frustum;

        float fovY = glm::radians(DegfovY);

        const float halfVSide = zFar * tanf(fovY * .5f);
        const float halfHSide = halfVSide * aspect;
        const glm::vec3 frontMultFar = zFar * tc.GetForward();

        frustum.nearFace = Plane(tc.GetWorldPosition() + zNear * tc.GetForward(), tc.GetForward());
        frustum.farFace = Plane(tc.GetWorldPosition() + frontMultFar, -tc.GetForward());
        frustum.rightFace = Plane(tc.GetWorldPosition(), glm::cross(tc.GetUp(), frontMultFar + tc.GetRight() * halfHSide));
        frustum.leftFace = Plane(tc.GetWorldPosition(), glm::cross(frontMultFar - tc.GetRight() * halfHSide, tc.GetUp()));
        frustum.topFace = Plane(tc.GetWorldPosition(), glm::cross(tc.GetRight(), frontMultFar - tc.GetUp() * halfVSide));
        frustum.bottomFace = Plane(tc.GetWorldPosition(), glm::cross(frontMultFar + tc.GetUp() * halfVSide, tc.GetRight()));
        return frustum;
    }
    Frustum VFC::createFrustum(float aspect, float zNear, float zFar, float DegfovY, const glm::vec3& worldPosition, const glm::quat& rotation) {
        Frustum frustum;

        glm::vec3 forward, up, right;
        forward = glm::rotate(rotation, TRANSFORM_FOREWARD_DIRECTION);
        up = glm::rotate(rotation, TRANSFORM_UP_DIRECTION);
        right = glm::rotate(rotation, TRANSFORM_RIGHT_DIRECTION);

        float fovY = glm::radians(DegfovY);

        const float halfVSide = zFar * tanf(fovY * .5f);
        const float halfHSide = halfVSide * aspect;
        const glm::vec3 frontMultFar = zFar * forward;

        frustum.nearFace = Plane(worldPosition + zNear * forward, forward);
        frustum.farFace = Plane(worldPosition + frontMultFar, -forward);
        frustum.rightFace = Plane(worldPosition, glm::cross(up, frontMultFar + right * halfHSide));
        frustum.leftFace = Plane(worldPosition, glm::cross(frontMultFar - right * halfHSide, up));
        frustum.topFace = Plane(worldPosition, glm::cross(right, frontMultFar - up * halfVSide));
        frustum.bottomFace = Plane(worldPosition, glm::cross(frontMultFar + up * halfVSide, right));
        return frustum;
    }
    Frustum VFC::createFrustum_ortho(float left, float right, float bottom, float top, float zNear, float zFar, TransformComponent& transform) {
        return createFrustum_ortho(left, right, bottom, top, zNear, zFar, transform.GetWorldPosition(), transform.GetWorldRotation());
    }
    Frustum VFC::createFrustum_ortho(float left, float right, float bottom, float top, float zNear, float zFar, const glm::vec3& worldPosition, const glm::quat& rotation) {
        struct TC {
            glm::vec3 forward, up, right, worldPosition;
        }tc;
        tc.forward = glm::rotate(rotation, TRANSFORM_FOREWARD_DIRECTION);
        tc.up = glm::rotate(rotation, TRANSFORM_UP_DIRECTION);
        tc.right = glm::rotate(rotation, TRANSFORM_RIGHT_DIRECTION);
        tc.worldPosition = worldPosition;

        Frustum frustum;

        frustum.nearFace = Plane(tc.worldPosition + zNear * tc.forward, tc.forward);
        frustum.farFace = Plane(tc.worldPosition + zFar  * tc.forward, -tc.forward);
        frustum.rightFace = Plane(tc.worldPosition + tc.right * right, -tc.right);
        frustum.leftFace = Plane(tc.worldPosition + tc.right * left, tc.right);
        frustum.topFace = Plane(tc.worldPosition + tc.up * top, -tc.up);
        frustum.bottomFace = Plane(tc.worldPosition + tc.up * bottom, tc.up);
        return frustum;
    }
    bool BoundingBoxAABB::isOnFrustum(const Frustum& camFrustum) const {
        return (isOnOrForwardPlan(camFrustum.leftFace) &&
            isOnOrForwardPlan(camFrustum.rightFace) &&
            isOnOrForwardPlan(camFrustum.topFace) &&
            isOnOrForwardPlan(camFrustum.bottomFace) &&
            isOnOrForwardPlan(camFrustum.nearFace) &&
            isOnOrForwardPlan(camFrustum.farFace));
    }
    void BoundingBoxAABB::applyTransform(const TransformComponent& transform) {
        // Scaled orientation
        const glm::vec3 right = transform.GetRight() * m_extents.x * transform.GetWorldScale().x;
        const glm::vec3 up = transform.GetUp() * m_extents.y * transform.GetWorldScale().y;
        const glm::vec3 forward = transform.GetForward() * m_extents.z * transform.GetWorldScale().z;

        const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

        const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

        const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

        m_transformedExtents = glm::vec3{ newIi, newIj, newIk };
        //Get global scale thanks to our transform
        m_transformedCenter = transform.GetWorldTransform() * glm::vec4(m_center, 1.f);
    }
    bool BoundingBoxAABB::isOnOrForwardPlan(const Plane& plan) const {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = m_transformedExtents.x * std::abs(plan.normal.x) + m_transformedExtents.y * std::abs(plan.normal.y) +
            m_transformedExtents.z * std::abs(plan.normal.z);
        return -r <= plan.getSignedDistanceToPlan(m_transformedCenter);
    }
}