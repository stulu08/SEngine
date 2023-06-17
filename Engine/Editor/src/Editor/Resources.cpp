#include "Resources.h"

namespace Stulu {

	Ref<Texture>& EditorResources::getEmptySlotTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/empty.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getFileTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/file.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getFolderTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/folder.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getObjectTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/object.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getSceneTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/scene.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getNoCameraTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/nocam.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getPlayTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/play.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getPauseTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/pause.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getStopTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/stop.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getSkyBoxTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/skybox.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getScriptTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/code.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getLightTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/Gizmo/light.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getDirectionalLightTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/Gizmo/directionallight.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getSpotLightTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/Gizmo/spotlight.png");
		return s_texture;
	}
	Ref<Texture>& EditorResources::getCameraTexture() {
		static Ref<Texture> s_texture = Texture2D::create("Editor/Textures/Gizmo/camera.png");
		return s_texture;
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
        isRenderTarget: false
        aspectRatio: 1.77777779
        textureWidth: 1920
        textureHeight: 1080
      mode: 0
      depth: -1
      renderTexture: 0
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
		static Ref<Shader> s_outlineShader = Shader::create("Editor Outline", R"(
		##include "Stulu/Vertex"
		##type fragment
		#version 460 core
		out vec4 FragColor;
		void main()
		{
		    FragColor = vec4(1.0f,0.541f,.0f, 1.0f);
		})");;
		return s_outlineShader;
	}
	Ref<Shader>& EditorResources::getGreenColorShader() {
		static Ref<Shader> s_shader = Shader::create("Green Color Shader", R"(
		##include "Stulu/Vertex"
		##type fragment
		#version 460 core
		out vec4 FragColor;
		void main()
		{
		    FragColor = vec4(0.0f,1.0f,.0f, 1.0f);
		})");
		return s_shader;
	}
	Ref<Shader>& EditorResources::getTransparentShader() {
		static Ref<Shader> s_shader = Shader::create("Green Color Shader", R"(
		##include "Stulu/Vertex"
		##type fragment
		#version 460 core
		out vec4 FragColor;
		void main()
		{
		    FragColor = vec4(0.0f,0.0f,0.0f,0.0f);
		})");
		return s_shader;
	}
}