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
        physx::PxTriangleMesh* CreateTriangleMesh(Ref<Mesh>& mesh) {
            Ref<Mesh>& theMesh = createRef<Mesh>(Mesh::combine(*mesh.get()));
            auto& phyicsModule = PhysicsModule::Get();

            physx::PxTriangleMeshDesc meshDesc;
            meshDesc.points.count = theMesh->getVerticesCount();
            meshDesc.points.stride = theMesh->getVertexBuffer()->getStride();
            meshDesc.points.data = theMesh->getVertices().data();

            meshDesc.triangles.count = theMesh->getIndicesCount() / 3;
            meshDesc.triangles.stride = 3 * theMesh->getIndexBuffer()->getStride();
            meshDesc.triangles.data = theMesh->getIndices().data();


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
        physx::PxConvexMesh* CreateConvexMesh(Ref<Mesh>& mesh) {
            Ref<Mesh>& theMesh = createRef<Mesh>(Mesh::combine(*mesh.get()));
            auto& phyicsModule = PhysicsModule::Get();

            physx::PxConvexMeshDesc convexDesc;
            convexDesc.points.count = theMesh->getVerticesCount();
            convexDesc.points.stride = theMesh->getVertexBuffer()->getStride();
            convexDesc.points.data = theMesh->getVertices().data();
            convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX | physx::PxConvexFlag::eDISABLE_MESH_VALIDATION | physx::PxConvexFlag::eFAST_INERTIA_COMPUTATION;

            convexDesc.indices.count = theMesh->getIndicesCount();
            convexDesc.indices.stride = theMesh->getIndexBuffer()->getStride();
            convexDesc.indices.data = theMesh->getIndices().data();

            physx::PxDefaultMemoryOutputStream buf;
            physx::PxConvexMeshCookingResult::Enum result;
            if (!phyicsModule.GetCooking()->cookConvexMesh(convexDesc, buf, &result))
                return NULL;
            physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
            return phyicsModule.GetPhysics()->createConvexMesh(input);
        }

    }
}