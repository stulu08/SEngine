#include "Game.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "World.h"
namespace Demo {
	Game::Game() {
	}
	void Game::initilize() {
		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
		m_scene = createRef<Scene>();

		//create player
		m_playerObject = m_scene->createGameObject("Player");
		m_playerObject.addComponent<NativeBehaviourComponent>().bind<Player>();

		m_world = m_scene->createGameObject("World");
		m_world.addComponent<NativeBehaviourComponent>().bind<World>();

		m_scene->onViewportResize(Stulu::Application::get().getWindow().getWidth(), Stulu::Application::get().getWindow().getHeight());
		m_scene->createGameObject("unnamed Mesh").addComponent<MeshFilterComponent>().mesh = Resources::getCubeMeshAsset();

		m_light = m_scene->createGameObject("Directional Light");
		m_light.getComponent<TransformComponent>().rotation = glm::radians(glm::vec3(-90.0f,30.0f,30.0f));
		m_light.addComponent<LightComponent>(LightComponent::Directional).strength = 2.0f;
		Math::setPerlinSeed(Random::getInt(0,99999999));
	}
	void Game::start() {
		Input::setCursorMode(Input::CursorMode::Disabled);
		m_scene->onRuntimeStart();
		m_playerCam = m_playerObject.getComponent<CameraComponent>().getNativeCamera();
	}
	void Game::update(Timestep t) {
		m_scene->onUpdateRuntime(t);
	}
	void Game::drawFrameBuffer() {
		glm::vec2 wp = Application::get().getWindow().getWindowPos();
		ImVec2 wPos = ImVec2(wp.x,wp.y);
		ImRect bb(wPos, wPos + ImVec2((float)m_playerCam->getFrameBuffer()->getTexture()->getWidth(), (float)m_playerCam->getFrameBuffer()->getTexture()->getHeight()));
		ImGui::GetBackgroundDrawList()->AddImage((void*)(uint64_t)m_playerCam->getFrameBuffer()->getTexture()->getColorAttachmentRendereID(), bb.Min, bb.Max, ImVec2(0, 1), ImVec2(1, 0), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
		RenderCommand::clear();
	}
	void Game::stop() {
		m_scene->onRuntimeStop();
		m_scene.reset();
	}
}