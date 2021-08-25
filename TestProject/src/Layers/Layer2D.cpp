#include "Layer2D.h"

#include "TestProject.h"
#include <imgui/imgui.h>

void Layer2D::onAttach() {
	seed = (uint32_t)Stulu::Random::getInt(0, 99999);
	std::async(&Layer2D::newMap, this);
	Stulu::RenderCommand::setClearColor(glm::vec4(.15f));
	m_textures[0] = Stulu::Texture2D::create("assets/game/textures/world/grass.png");
	m_textures[1] = Stulu::Texture2D::create("assets/game/textures/world/water.png");
	m_textures[2] = Stulu::Texture2D::create("assets/game/textures/world/bright-grass.png");
	m_textures[3] = Stulu::Texture2D::create("assets/game/textures/world/stone.png");
	m_textures[4] = Stulu::Texture2D::create("assets/game/textures/world/sand.png");
	m_textures[5] = Stulu::Texture2D::create("assets/game/textures/world/tree.png");

	m_textures[6] = Stulu::Texture2D::create("assets/game/textures/player/player-idle.png");
	m_textures[7] = Stulu::Texture2D::create("assets/game/textures/player/player-shoot.png");
	m_textures[8] = Stulu::Texture2D::create("assets/game/textures/player/player-walk.png");
	m_textures[9] = Stulu::Texture2D::create("assets/game/textures/player/bullet.png");

	player.texture = m_textures[5];
	findPlayerSpawn(1);


}
float shootTimer = .0f;
void Layer2D::onUpdate(Stulu::Timestep timestep) {
	cameraController.onUpdate(timestep);


	Stulu::RenderCommand::clear();
	Stulu::Renderer2D::beginScene(cameraController.getCamera());

	switch (state)
	{
	case Menu:
		drawMenu();
		break;
	case Running:
		drawGame(timestep);
		break;
	case Over:
		drawGameOver();
		break;
	}

	Stulu::Renderer2D::endScene();

}
void Layer2D::onEvent(Stulu::Event& e) {
	cameraController.onEvent(e);
}
void Layer2D::onImguiRender(Stulu::Timestep timestep) {
	ST_PROFILING_FUNCTION();
	switch (state)
	{
	case Menu:
		drawMenuGUI();
		break;
	case Running:
		drawGameGUI(timestep);
		break;
	case Over:
		drawGameOverGUI();
		break;
	}
}


void Layer2D::drawMenu(){
	
}
void Layer2D::drawGame(float timestep){
	cameraController.getTransform().position = player.transform.position;
	handlePlayer(timestep);
	handleEnemys(timestep);
	drawMap();
	drawPlayerAndBullets(timestep);
}
void Layer2D::drawGameOver() {
	
}

void drawMenuGUI() {

}
void drawGameGUI(float timestep) {

}
void drawGameOverGUI() {

}

char Layer2D::generateTile(glm::vec2 pos) {
	char m;
	float y = Stulu::Math::perlinAccumalatedNosie((float)pos.x, (float)pos.y, 16, 5.0f, (float)mapSize, (float)mapSize);
	int z = Stulu::Random::getInt(0, 5);
	if (y < .4f) {
		m = 'W';
	}
	else if (y < .42) {
		m = 'B';
	}
	else if (y < .5) {
		m = 'G';
	}
	else if (y < .7) {
		m = (z != 2 ? ' ' : 'T');
	}
	else if (y < .8) {
		m = 'S';
	}
	else {
		m = (z != 2 ? ' ' : 'T');
	}
	return m;
}
void Layer2D::newMap() {
	Stulu::Math::setPerlinSeed(seed);

	char** map;

	map = new char* [mapSize];

	for (int i = 0; i < mapSize; i++) {
		map[i] = new char[mapSize];
		for (int j = 0; j < mapSize; j++) {
			map[i][j] = generateTile(glm::vec2(i, j));
		}
	}
	this->map = map;
}
bool Layer2D::canWalkOn(glm::vec2 pos) {
	if (!inMap(pos))
		return false;

	char tile = map[(int)pos.x][(int)pos.y];
	
	if (tile == ' ' || tile == 'T' || tile == 'B' || tile == 'G')
		return true;

	return false;
}
bool Layer2D::inMap(glm::vec2 pos) {
	Stulu::Quad mapQuad = { glm::vec2(.0f), (float)mapSize, (float)mapSize };

	if (!Stulu::Math::isPosOverQuad(mapQuad, pos))
		return false;

	return true;
}
void Layer2D::findPlayerSpawn(int dist) {
	for (int x = mapSize / 2 - dist; x < mapSize / 2 + dist; x++) {
		for (int y = mapSize / 2 - dist; y < mapSize / 2 + dist; y++) {
			if (canWalkOn(glm::vec2(x, y))) {
				player.transform.position = glm::vec3(x, y, 1.0f);
				return;
			}
		}
	}
	findPlayerSpawn(dist + 1);
}
void Layer2D::handlePlayer(float timestep) {
	glm::vec3 mPos = Stulu::Math::screenToWorld(glm::vec2((float)Stulu::Input::getMouseX(), (float)Stulu::Input::getMouseY()), cameraController.getCamera(), cameraController.getScreenSize());
	player.transform.rotation.z = Stulu::Math::lookAt2D(player.transform.position, mPos);

	player.transform.scale = glm::vec3(1.0f);
	player.texture = m_textures[6];


	if (Stulu::Input::isKeyDown(KEY_W)) {
		glm::vec3 direction = player.transform.upDirection();
		if (canWalkOn(glm::vec2(player.transform.position + direction))) {
			player.transform.position += direction * 6.0f * timestep;
		}
		else if (inMap(glm::vec2(player.transform.position + direction))) {
			player.transform.position += direction * 1.0f * timestep;
		}
		player.transform.scale = glm::vec3(1.0f);
		player.texture = m_textures[8];
	}
	else if (Stulu::Input::isKeyDown(KEY_S)) {
		glm::vec3 direction = -player.transform.upDirection();
		if (canWalkOn(glm::vec2(player.transform.position + direction))) {
			player.transform.position += direction * 2.0f * timestep;
		}
		else if (inMap(glm::vec2(player.transform.position + direction))) {
			player.transform.position += direction * .35f * timestep;
		}
		player.transform.scale = glm::vec3(1.0f);
		player.texture = m_textures[8];
	}
	if (Stulu::Input::isKeyDown(KEY_A)) {
		glm::vec3 direction = -player.transform.rightDirection();
		if (canWalkOn(glm::vec2(player.transform.position + direction))) {
			player.transform.position += direction * 3.0f * timestep;
		}
		else if (inMap(glm::vec2(player.transform.position + direction))) {
			player.transform.position += direction * .5f * timestep;
		}
		player.transform.scale = glm::vec3(1.0f);
		player.texture = m_textures[8];
	}
	else if (Stulu::Input::isKeyDown(KEY_D)) {
		glm::vec3 direction = player.transform.rightDirection();
		if (canWalkOn(glm::vec2(player.transform.position + direction))) {
			player.transform.position += direction * 3.0f * timestep;
		}
		else if (inMap(glm::vec2(player.transform.position + direction))) {
			player.transform.position += direction * .5f * timestep;
		}

		player.transform.scale = glm::vec3(1.0f);
		player.texture = m_textures[8];
	}
	if (Stulu::Input::isMouseDown(MOUSE_BUTTON_1)) {
		shoot();
	};
	if (!inMap(player.transform.position))
		findPlayerSpawn(1);

	if (shootTimer > 1.0f / shootsPerSecond) {
		shootTimer = 0.0f;
		canShoot = true;
	}
	else {
		shootTimer += timestep;

	}
}
void Layer2D::shoot() {
	player.transform.scale = glm::vec3(1.0f,2.0f,1.0f);
	player.texture = m_textures[7];
	if (!canShoot)
		return;
	Bullet* bullet = new Bullet();
	bullet->speed = 15.0f;
	bullet->texture = m_textures[9];
	bullet->transform = Stulu::Transform(glm::vec3(player.transform.position.x ,player.transform.position.y, -.1f) + glm::rotate(player.transform.getOrientation() ,glm::vec3(.15f,.3f,1.0f) / 1.5f), player.transform.rotation, glm::vec3(.07f,.25f,1.0f) / 1.5f);
	bullet->velocity = player.transform.upDirection();
	bullets.push_back(bullet);
	canShoot = false;
}
void Layer2D::handleEnemys(float timestep) {

}
void Layer2D::drawMap() {
	Stulu::Renderer2D::drawTexturedQuad(m_textures[0], glm::vec3(((float)(mapSize) / 2.0f) - .5f, ((float)(mapSize) / 2.0f) - .5f, -1.1f), glm::vec2((float)mapSize), glm::vec2((float)mapSize));
	for (int x = (int)player.transform.position.x - viewDistance; x < player.transform.position.x + viewDistance; x++) {

		for (int y = (int)player.transform.position.y - viewDistance; y < player.transform.position.y + viewDistance; y++) {
			if (glm::distance(player.transform.position, glm::vec3(x, y, -1.0f)) > (float)viewDistance || !inMap(glm::vec2(x, y))) {
				continue;
			}


			switch (map[x][y])
			{
			case 'W'://water
				Stulu::Renderer2D::drawTexturedQuad(m_textures[1], glm::vec3(x, y, -1.0f), glm::vec2(1.0f));
				break;
			case 'G'://Bright Grass
				Stulu::Renderer2D::drawTexturedQuad(m_textures[2], glm::vec3(x, y, -1.0f), glm::vec2(1.0f));
				break;
			case 'S'://Stone
				Stulu::Renderer2D::drawTexturedQuad(m_textures[3], glm::vec3(x, y, -1.0f), glm::vec2(1.0f));
				break;
			case 'B'://Beach Sand
				Stulu::Renderer2D::drawTexturedQuad(m_textures[4], glm::vec3(x, y, -1.0f), glm::vec2(1.0f));
				break;
			case 'T'://tree
				Stulu::Renderer2D::drawTexturedQuad(m_textures[5], glm::vec3(x, y, -.3f), glm::vec2(.73f, 1.0f));
				break;
			}
		}
	}
}
void Layer2D::drawPlayerAndBullets(float timestep) {
	for (int i = 0; i < bullets.size(); i++) {
		if (glm::distance(bullets[i]->transform.position, player.transform.position) > 30.0f) {
			bullets.erase(bullets.begin() + i);
			continue;
		}
		bullets[i]->transform.position += bullets[i]->velocity * bullets[i]->speed * timestep;
		Stulu::Renderer2D::drawTexturedQuad(bullets[i]->transform, bullets[i]->texture);
	}
	player.transform.scale /= 1.5f;
	Stulu::Renderer2D::drawTexturedQuad(player.transform, player.texture);
}