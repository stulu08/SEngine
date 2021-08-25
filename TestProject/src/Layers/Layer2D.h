#include <Stulu.h>
#include "ParticleSystem.h"

struct GameObject {
	Stulu::Transform transform;
	Stulu::Ref<Stulu::Texture2D> texture;
	glm::vec4 color = COLOR_WHITE;
	Stulu::Quad hitbox = {glm::vec2(transform.position.x - transform.scale.x/2.0f, transform.position.y - transform.scale.y/2.0f), transform.scale.x, transform.scale.y};
};
struct Bullet{
	Stulu::Transform transform;
	Stulu::Ref<Stulu::Texture2D> texture;
	glm::vec4 color = COLOR_WHITE;
	glm::vec3 velocity;
	float speed;
};
enum GameState{Menu,Running,Over};
class Layer2D : public Stulu::Layer
{
public:
	Layer2D()
		:Layer("2D"), cameraController(1280.0f / 720.0f, true){
	}

	void onAttach() override;
	void onUpdate(Stulu::Timestep timestep) override;
	void onEvent(Stulu::Event& e) override;
	void onImguiRender(Stulu::Timestep timestep) override;

private:
	int viewDistance = 20;
	int mapSize = 500;
	int shootsPerSecond = 10;
	bool canShoot = true;

	char** map;

	uint32_t seed = 0;

	Stulu::OrthographicCameraController cameraController;
	Stulu::Ref<Stulu::Texture2D> m_textures[10];

	GameState state = GameState::Running;
	std::vector<Bullet*> bullets;
	std::vector<GameObject*> enemys;
	GameObject player;

	void drawMenu();
	void drawGame(float timestep);
	void drawGameOver();
	void drawMenuGUI();
	void drawGameGUI(float timestep);
	void drawGameOverGUI();

	char generateTile(glm::vec2 pos);
	void newMap();
	bool canWalkOn(glm::vec2 pos);
	bool inMap(glm::vec2 pos);
	void findPlayerSpawn(int dist);
	void handlePlayer(float timestep);
	void shoot();
	void handleEnemys(float timestep);
	void drawMap();
	void drawPlayerAndBullets(float timestep);
};

