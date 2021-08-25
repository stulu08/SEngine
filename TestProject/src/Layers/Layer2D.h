#include <Stulu.h>
#include "ParticleSystem.h"
struct ImVec2;
struct GameObject {
	Stulu::Transform transform;
	Stulu::Ref<Stulu::Texture2D> texture;
	glm::vec4 color = COLOR_WHITE;
	Stulu::Quad hitbox() { return Stulu::Quad{ glm::vec2(transform.position.x - transform.scale.x / 2.0f, transform.position.y - transform.scale.y / 2.0f), transform.scale.x, transform.scale.y }; }
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
	int shootsPerSecond = 10;
	bool canShoot = true;

	float alive = .0f;
	int kills = 0;
	int ammo = 90;
	bool knife = false;

	char** map;

	float difficulty = 30.0f;

	int mapSize = 500;
	float treeDensity = .2f;
	uint32_t seed = 0;

	Stulu::OrthographicCameraController cameraController;
	Stulu::Ref<Stulu::Texture2D> m_textures[17];

	GameState state = GameState::Menu;
	std::vector<Bullet*> bullets;
	std::vector<GameObject*> enemys;
	GameObject player;

	void drawMenu(float timestep);
	void drawGame(float timestep);
	void drawGameOver();
	void drawMenuGUI();
	void drawGameGUI(float timestep);
	void drawGameOverGUI();

	void drawText(ImVec2 pos, glm::vec4 color, std::string text, glm::vec4 colorBG = glm::vec4(.15f,.15f, .15f, .75f));

	void startGame();

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

	void onGameOver();
};

