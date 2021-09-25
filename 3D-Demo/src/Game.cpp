#include <Stulu.h>
#include <Stulu/Core/EntryPoint.h>
#include "MainLayer.h"
//https://github.com/b3agz/Code-A-Game-Like-Minecraft-In-Unity
using namespace Stulu;

class Game : public Application {
public:
	Game() 
		:Application("3D Demo") {
		RenderCommand::setClearColor(glm::vec4(.0f));
		RenderCommand::clear();

		pushLayer(new MainLayer());
	}
	~Game() {

	}
};

Application* Stulu::CreateApplication() {
	return new Game();
}