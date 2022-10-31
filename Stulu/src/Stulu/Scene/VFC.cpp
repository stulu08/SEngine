#include "st_pch.h"
#include "VFC.h"

#include "Stulu/Scene/Components/Camera.h"
#include "Stulu/Scene/Components/Components.h"

namespace Stulu {

    Ref<BoundingBox> Stulu::VFC::createBoundingBox(const Mesh* mesh) {
        switch (s_mode)
        {
        case Stulu::BoundingBoxType::None:
            break;
        case Stulu::BoundingBoxType::AABB:
            return createRef<BoundingBoxAABB>(mesh);
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
        const glm::vec3 frontMultFar = zFar * tc.forward;

        frustum.nearFace = Plane(tc.worldPosition + zNear * tc.forward, tc.forward);
        frustum.farFace = Plane(tc.worldPosition + frontMultFar, -tc.forward);
        frustum.rightFace = Plane(tc.worldPosition, glm::cross(tc.up, frontMultFar + tc.right * halfHSide));
        frustum.leftFace = Plane(tc.worldPosition, glm::cross(frontMultFar - tc.right * halfHSide, tc.up));
        frustum.topFace = Plane(tc.worldPosition, glm::cross(tc.right, frontMultFar - tc.up * halfVSide));
        frustum.bottomFace = Plane(tc.worldPosition, glm::cross(frontMultFar + tc.up * halfVSide, tc.right));
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
        return createFrustum_ortho(left, right, bottom, top, zNear, zFar, transform.worldPosition, transform.worldRotation);
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
    BoundingBoxAABB::BoundingBoxAABB(const Mesh* mesh) {
        glm::vec3 min;
        glm::vec3 max;

        if (mesh->getVertices().size() <= 0) {
            min = glm::vec3(.0f);
            max = glm::vec3(.0f);
        }
        else {
            min = mesh->getVertices()[0].pos;
            max = mesh->getVertices()[0].pos;
            //get furthest point on each axis
            for (auto& vertex : mesh->getVertices()) {
                //min
                if (min.x > vertex.pos.x)
                    min.x = vertex.pos.x;
                if (min.y > vertex.pos.y)
                    min.y = vertex.pos.y;
                if (min.z > vertex.pos.z)
                    min.z = vertex.pos.z;
                //max
                if (max.x < vertex.pos.x)
                    max.x = vertex.pos.x;
                if (max.y < vertex.pos.y)
                    max.y = vertex.pos.y;
                if (max.z < vertex.pos.z)
                    max.z = vertex.pos.z;
            }
        }
        m_center = { (max + min) * 0.5f };
        m_extents = { max.x - m_center.x, max.y - m_center.y, max.z - m_center.z };
        m_aabb = { min, max };
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
        const glm::vec3 right = transform.right * m_extents.x * transform.worldScale.x;
        const glm::vec3 up = transform.up * m_extents.y * transform.worldScale.y;
        const glm::vec3 forward = transform.forward * m_extents.z * transform.worldScale.z;

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
        m_transformedCenter = transform.transform * glm::vec4(m_center, 1.f);
    }
    bool BoundingBoxAABB::isOnOrForwardPlan(const Plane& plan) const {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = m_transformedExtents.x * std::abs(plan.normal.x) + m_transformedExtents.y * std::abs(plan.normal.y) +
            m_transformedExtents.z * std::abs(plan.normal.z);
        return -r <= plan.getSignedDistanceToPlan(m_transformedCenter);
    }
}