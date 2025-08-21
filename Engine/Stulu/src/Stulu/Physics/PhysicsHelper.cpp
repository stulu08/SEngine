#include "st_pch.h"

#define PX_PHYSX_STATIC_LIB
#include "PxPhysicsAPI.h"

#include "PhysicsModule.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/ImGui/Gizmo.h"

//https://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/API.html
namespace Stulu {
    namespace PhysicsHelper {
        physx::PxVec3 Vec3ToPhysX(const glm::vec3& val) {
            return physx::PxVec3(val.x, val.y, val.z);
        }
        glm::vec3 PhysXToVec3(const physx::PxVec3& val) {
            return glm::vec3(val.x, val.y, val.z);
        }
        physx::PxQuat QuatToPhysX(const glm::quat& val) {
            return physx::PxQuat(val.x, val.y, val.z, val.w);
        }
        glm::quat PhysXToQuat(const physx::PxQuat& val) {
            return glm::quat(val.w, val.x, val.y, val.z);
        }
        //mesh needs to be combined
        physx::PxTriangleMesh* CreateTriangleMesh(Mesh* mesh) {
            auto& phyicsModule = PhysicsModule::Get();

            const BufferElement Element = mesh->GetPositionLayoutElement();

            physx::PxTriangleMeshDesc meshDesc;
            meshDesc.points.count = (uint32_t)mesh->GetVerticesCount();
            meshDesc.points.stride = (uint32_t)mesh->GetStride();
            meshDesc.points.data = reinterpret_cast<const uint8_t*>(mesh->GetVertices()) + Element.offset;

            meshDesc.triangles.count = (uint32_t)(mesh->GetIndices().size() / 3);
            meshDesc.triangles.stride = (uint32_t)(3 * mesh->GetVertexArray()->getIndexBuffer()->getStride());
            meshDesc.triangles.data = mesh->GetIndices().data();


            physx::PxDefaultMemoryOutputStream writeBuffer;
            physx::PxTriangleMeshCookingResult::Enum result;
            bool status = phyicsModule.GetCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
            if (!status)
                return NULL;
            if (result == physx::PxTriangleMeshCookingResult::Enum::eFAILURE) {
                CORE_ERROR("Triangle Cooking failed");
                return nullptr;
            }
            physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
            return phyicsModule.GetPhysics()->createTriangleMesh(readBuffer);
        }

        physx::PxConvexMesh* CreateConvexMesh(Mesh* mesh) {
            auto& phyicsModule = PhysicsModule::Get();

            const size_t limit = static_cast<size_t>(255) * 3;
            const size_t skipCount = (size_t)glm::ceil(mesh->GetVerticesCount() / limit);
            const size_t count = glm::min(mesh->GetVerticesCount(), limit);

            const BufferElement Element = mesh->GetPositionLayoutElement();

            std::vector<glm::vec3> sampledPoints;
            sampledPoints.reserve(255);
            for (size_t i = 0; i < mesh->GetVerticesCount() && sampledPoints.size() < 255; i++) {
                glm::vec3 pos = *(glm::vec3*)((uint8_t*)mesh->GetVertices() + i * mesh->GetStride() + Element.offset);
                sampledPoints.push_back(pos);
            }

            physx::PxConvexMeshDesc convexDesc;
            convexDesc.points.count = (uint32_t)sampledPoints.size();
            convexDesc.points.stride = (uint32_t)sizeof(glm::vec3);
            convexDesc.points.data = sampledPoints.data();
            convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX
                | physx::PxConvexFlag::eDISABLE_MESH_VALIDATION
                | physx::PxConvexFlag::eFAST_INERTIA_COMPUTATION;

            physx::PxDefaultMemoryOutputStream buf;
            physx::PxConvexMeshCookingResult::Enum result;
            if (!phyicsModule.GetCooking()->cookConvexMesh(convexDesc, buf, &result)) {
                return nullptr;
            }
            if (result == physx::PxTriangleMeshCookingResult::Enum::eFAILURE) {
                CORE_ERROR("Triangle Cooking failed");
                return nullptr;
            }

            physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
            return phyicsModule.GetPhysics()->createConvexMesh(input);
        }

    }
}