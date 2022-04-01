#include "Resources.h"

namespace Stulu {

	void EditorResources::loadAll() {
		s_emptyTexture = Texture2D::create("Stulu/assets/Textures/Icons/empty.png");
		s_fileTexture = Texture2D::create("Stulu/assets/Textures/Icons/file.png");
		s_folderTexture = Texture2D::create("Stulu/assets/Textures/Icons/folder.png");
		s_objectTexture = Texture2D::create("Stulu/assets/Textures/Icons/object.png");
		s_sceneTexture = Texture2D::create("Stulu/assets/Textures/Icons/scene.png");
		s_noCameraTexture = Texture2D::create("Stulu/assets/Textures/Icons/nocam.png");
		s_playTexture = Texture2D::create("Stulu/assets/Textures/Icons/play.png");
		s_pauseTexture = Texture2D::create("Stulu/assets/Textures/Icons/pause.png");
		s_stopTexture = Texture2D::create("Stulu/assets/Textures/Icons/stop.png");
		s_skyboxTexture = Texture2D::create("Stulu/assets/Textures/Icons/skybox.png");

		std::string src = R"(
		##add ST_vertex
		##type fragment
		#version 460 core
		out vec4 FragColor;
		void main()
		{
		    FragColor = vec4(1.0f,0.541f,.0f, 1.0f);
		})";
		s_outlineShader = Shader::create("Editor Outline",src);
	}
	Ref<Texture>& EditorResources::getEmptySlotTexture() {
		return s_emptyTexture;
	}
	Ref<Texture>& EditorResources::getFileTexture() {
		return s_fileTexture;
	}
	Ref<Texture>& EditorResources::getFolderTexture() {
		return s_folderTexture;
	}
	Ref<Texture>& EditorResources::getObjectTexture() {
		return s_objectTexture;
	}
	Ref<Texture>& EditorResources::getSceneTexture() {
		return s_sceneTexture;
	}
	Ref<Texture>& EditorResources::getNoCameraTexture() {
		return s_noCameraTexture;
	}
	Ref<Texture>& EditorResources::getPlayTexture() {
		return s_playTexture;
	}
	Ref<Texture>& EditorResources::getPauseTexture() {
		return s_pauseTexture;
	}
	Ref<Texture>& EditorResources::getStopTexture() {
		return s_stopTexture;
	}
	Ref<Texture>& EditorResources::getSkyBoxTexture() {
		return s_skyboxTexture;
	}
	const std::string EditorResources::getDefaultSceneSource()
	{
		return R"(Scene: Untitled
Settings:
  gamma: 2.20000005
  toneMappingExposure: 1
  framebuffer16bit: true
  enablePhsyics3D: true
  physicsData.gravity: [0, -9.80000019, 0]
  physicsData.length: 1
  physicsData.PhysXGpu: true
  physicsData.speed: 9.81000042
  physicsData.workerThreads: 4
GameObjects:
  - GameObject: 293002038549553951
    GameObjectBaseComponent:
      name: Perspective Camera
      tag: MainCam
    TransformComponent:
      position: [0, 0, 10]
      rotation: [0, 0, 0, 1]
      scale: [1, 1, 1]
    CameraComponent:
      Camera Settings:
        clearType: 1
        clearColor: [0, 0, 0, 1]
        fov: 80
        zoom: 1
        zNear: 0.00999999978
        zFar: 250
        staticAspect: false
        aspectRatio: 1.77777779
        textureWidth: 1920
        textureHeight: 1080
      mode: 0
      depth: -1
    SkyBoxComponent :
      texture: 11
      blur: 0
      mapType: 0
  - GameObject: 91324720934723243
    GameObjectBaseComponent:
      name: Directional Light
      tag: default
    TransformComponent :
      position: [-2.45348263, 3.46975136, 2.45348144]
      rotation: [-0.353553414, -0.353553414, -0.14644663, 0.853553355]
      scale: [0.999999464, 0.999998391, 0.999997616]
    LightComponent:
      color: [1, 1, 1]
      lightType: 0
      strength: 1
      areaRadius: 1
      spotLight_cutOff: 10
      spotLight_outerCutOff: 15
  - GameObject: 6234263611873422275
    GameObjectBaseComponent:
      name: Cube
      tag: default
    TransformComponent :
      position: [0, 0, 0]
      rotation: [0, 0, 0, 1]
      scale: [1, 1, 1]
    MeshRendererComponent:
      {}
    MeshFilterComponent:
      mesh: 401
    BoxColliderComponent:
      dynamicFriction: 0.100000001
      staticFriction: 0.100000001
      restitution: 0
      size: [0.5, 0.5, 0.5]
      offset: [0, 0, 0])";
	}
	Ref<Shader>& EditorResources::getOutlineShader() {
		return s_outlineShader;
	}
}