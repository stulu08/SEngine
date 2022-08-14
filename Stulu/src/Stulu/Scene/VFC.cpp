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
    bool VFC::isInView(const Ref<BoundingBox>& boundingBox, const TransformComponent& transform) {
        if (!s_isEnabled)
            return true;

        return boundingBox->isOnFrustum(s_frustum, transform);
        
    }
    Frustum VFC::setCamera(float aspect, float zNear, float zFar, float DegfovY, TransformComponent& tc) {
        Frustum     frustum;

        float fovY = glm::radians(DegfovY);

        const float halfVSide = zFar * tanf(fovY * .5f);
        const float halfHSide = halfVSide * aspect;
        const glm::vec3 frontMultFar = zFar * tc.forward;

        s_frustum.nearFace = { tc.worldPosition + zNear * tc.forward, tc.forward };
        s_frustum.farFace = { tc.worldPosition + frontMultFar, -tc.forward };
        s_frustum.rightFace = { tc.worldPosition, glm::cross(tc.up, frontMultFar + tc.right * halfHSide) };
        s_frustum.leftFace = { tc.worldPosition, glm::cross(frontMultFar - tc.right * halfHSide, tc.up) };
        s_frustum.topFace = { tc.worldPosition, glm::cross(tc.right, frontMultFar - tc.up * halfVSide) };
        s_frustum.bottomFace = { tc.worldPosition, glm::cross(frontMultFar + tc.up * halfVSide, tc.right) };
        return s_frustum;
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
    bool BoundingBoxAABB::isOnFrustum(const Frustum& camFrustum, const TransformComponent& transform) const {
        //Get global scale thanks to our transform
        const glm::vec3 globalCenter{ transform.transform * glm::vec4(m_center, 1.f) };

        // Scaled orientation
        const glm::vec3 right = transform.right * m_extents.x * transform.worldScale.x;
        const glm::vec3 up = transform.up * m_extents.y * transform.worldScale.x;
        const glm::vec3 forward = transform.forward * m_extents.z * transform.worldScale.x;

        const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

        const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

        const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

        const BoundingBoxAABB globalAABB(globalCenter, { newIi, newIj, newIk });

        return (globalAABB.isOnOrForwardPlan(camFrustum.leftFace) &&
            globalAABB.isOnOrForwardPlan(camFrustum.rightFace) &&
            globalAABB.isOnOrForwardPlan(camFrustum.topFace) &&
            globalAABB.isOnOrForwardPlan(camFrustum.bottomFace) &&
            globalAABB.isOnOrForwardPlan(camFrustum.nearFace) &&
            globalAABB.isOnOrForwardPlan(camFrustum.farFace));
    }
    bool BoundingBoxAABB::isOnOrForwardPlan(const Plane& plan) const {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = m_extents.x * std::abs(plan.normal.x) + m_extents.y * std::abs(plan.normal.y) +
            m_extents.z * std::abs(plan.normal.z);

        return -r <= plan.getSignedDistanceToPlan(m_center);
    }
}