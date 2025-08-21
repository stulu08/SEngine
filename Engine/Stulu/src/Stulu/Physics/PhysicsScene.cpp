#include "st_pch.h"
#include "PhysicsScene.h"
#include "PhysicsModule.h"

#include "Stulu/ImGui/Gizmo.h"

#include "Components/RigidbodyComponent.h"
#include "Components/Collider.h"

#define PX_PHYSX_STATIC_LIB
#include "PxPhysicsAPI.h"


namespace Stulu {
    bool PhysicsScene::Initlize(Scene* scene) {
        m_scene = scene;
        m_enabled = scene->PhysicsEnable();
        return m_enabled;
    }
    
    void PhysicsScene::SceneStart() {
        if (!m_scene)
            return;
        
        m_enabled = m_scene->PhysicsEnable();
        if (!m_enabled)
            return;
        
        m_time = m_scene->GetSceneRuntime();

        auto& phyicsModule = PhysicsModule::Get();
        physx::PxSceneDesc sceneDesc(phyicsModule.GetPhysics()->getTolerancesScale());

#if PX_WINDOWS
        sceneDesc.cudaContextManager = phyicsModule.GetCudaContext();
#endif
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        sceneDesc.cpuDispatcher = phyicsModule.GetCpuDispatcher();
        sceneDesc.gravity = PhysicsHelper::Vec3ToPhysX(m_gravity);
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;

        m_physicsScene = phyicsModule.GetPhysics()->createScene(sceneDesc);
        if (!m_scene) {
            CORE_ERROR("Physics Scene creation failed!");
            return;
        }

        m_controllerManager = PxCreateControllerManager(*m_physicsScene);
        if (!m_controllerManager) {
            CORE_ERROR("Physics Controller Manager creation failed!");
            return;
        }

        CORE_INFO("Physics scene created!");

        CreatePhysicsObject();
    }

    void PhysicsScene::PreUpdate() {
        if (!m_enabled)
            return;

        float advance = glm::max(m_scene->GetSceneRuntime() - m_time, 0.0f);
        m_time = m_scene->GetSceneRuntime();
        if (advance <= 0.0f) {
            return;
        }

        ST_PROFILING_SCOPE("Scene - PhysX Update");
        m_physicsScene->simulate(advance);

        while (!m_physicsScene->fetchResults()) {
            m_scene->GeneralUpdates();
        }



        // only update dynamic objects
        for (auto id : m_scene->GetAllWith<RigidbodyComponent>()) {
            GameObject object = { id, m_scene };
            auto& rb = object.getComponent<RigidbodyComponent>();
            rb.ApplyTransformChanges();
        }

        CreatePhysicsObject();
    }
    void PhysicsScene::CreatePhysicsObject() {
        for (auto id : m_scene->GetAllWith<BoxColliderComponent>()) {
            GameObject object = { id, m_scene };
            if (object.getComponent<BoxColliderComponent>().GetShape() == nullptr)
                object.getComponent<BoxColliderComponent>().Create(this);
        }
        for (auto id : m_scene->GetAllWith<SphereColliderComponent>()) {
            GameObject object = { id, m_scene };
            if (object.getComponent<SphereColliderComponent>().GetShape() == nullptr)
                object.getComponent<SphereColliderComponent>().Create(this);
        }
        for (auto id : m_scene->GetAllWith<CapsuleColliderComponent>()) {
            GameObject object = { id, m_scene };
            if (object.getComponent<CapsuleColliderComponent>().GetShape() == nullptr)
                object.getComponent<CapsuleColliderComponent>().Create(this);
        }
        for (auto id : m_scene->GetAllWith<MeshColliderComponent>()) {
            GameObject object = { id, m_scene };
            if (object.getComponent<MeshColliderComponent>().GetShape() == nullptr)
                object.getComponent<MeshColliderComponent>().Create(this);
        }
    }

    void PhysicsScene::SceneExit() {
        if (m_controllerManager) {
            m_controllerManager->release();
            m_controllerManager = nullptr;
        }
        if (m_physicsScene) {
            m_physicsScene->release();
            m_physicsScene = nullptr;
        }
        m_scene = nullptr;
        m_enabled = false;
        m_time = 0.0f;
    }

    void PhysicsScene::GameObjectDestory(const GameObject& object) {
        if (object.hasComponent<RigidActorComponent>())
            object.removeComponent<RigidActorComponent>();
    }

    void PhysicsScene::SetGravity(const glm::vec3& value) {
        m_gravity = value;
        if (m_physicsScene)
            m_physicsScene->setGravity(PhysicsHelper::Vec3ToPhysX(value));
    }

    void PhysicsScene::SetDebugVisual(PhysicsHelper::DebugViuals option, float value) const {
        if (m_physicsScene)
            m_physicsScene->setVisualizationParameter((physx::PxVisualizationParameter::Enum)option, value);
    }
    float PhysicsScene::GetDebugVisual(PhysicsHelper::DebugViuals option) const {
        if (m_physicsScene)
            return m_physicsScene->getVisualizationParameter((physx::PxVisualizationParameter::Enum)option);
        return 0.0f;
    }
    void PhysicsScene::RenderSceneDebugData() const {
        if (!m_physicsScene)
            return;
        const auto& renderBuffer = m_physicsScene->getRenderBuffer();

        for (size_t i = 0; i < renderBuffer.getNbLines(); i++) {
            const physx::PxDebugLine& line = renderBuffer.getLines()[i];
            Renderer2D::drawLine(PhysicsHelper::PhysXToVec3(line.pos0), PhysicsHelper::PhysXToVec3(line.pos1), PhysicsHelper::ColorConvert(line.color0), PhysicsHelper::ColorConvert(line.color1));
        }

        for (size_t i = 0; i < renderBuffer.getNbPoints(); i++) {
            const physx::PxDebugPoint& point = renderBuffer.getPoints()[i];
            Gizmo::drawSphere(Math::createMat4(PhysicsHelper::PhysXToVec3(point.pos), glm::vec3(0.1f)), PhysicsHelper::ColorConvert(point.color));
        }

        for (size_t i = 0; i < renderBuffer.getNbTexts(); i++) {
            const physx::PxDebugText& text = renderBuffer.getTexts()[i];
            Gizmo::drawText(text.string, PhysicsHelper::PhysXToVec3(text.position), PhysicsHelper::ColorConvert(text.color));
        }
    }

    void PhysicsScene::SerializerScene(YAML::Emitter& out) {
        out << YAML::Key << "Physics3D" << YAML::Value << YAML::BeginMap;;
        out << YAML::Key << "m_gravity" << YAML::Value << this->m_gravity;;
        out << YAML::EndMap;;
    }
    void PhysicsScene::DeserializerScene(YAML::Node& data) {
        if (data["Physics3D"]) {
            YAML::Node settings = data["Physics3D"];
            if (settings["m_gravity"]) 
                m_gravity = settings["m_gravity"].as<glm::vec3>();;
        }
    }
}