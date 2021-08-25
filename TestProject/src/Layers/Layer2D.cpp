#include "Layer2D.h"

#include "TestProject.h"
#include <imgui/imgui.h>

void Layer2D::onAttach() {
	seed = (uint32_t)Stulu::Random::getInt(0, 99999);

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
	m_textures[15] = Stulu::Texture2D::create("assets/game/textures/player/knife.png");
	m_textures[16] = Stulu::Texture2D::create("assets/game/textures/player/knifeAttack.png");

	m_textures[10] = Stulu::Texture2D::create("assets/game/textures/ui/cursor.png");
	m_textures[11] = Stulu::Texture2D::create("assets/game/textures/ui/cursor-2.png");
	m_textures[12] = Stulu::Texture2D::create("assets/game/textures/ui/start.png");
	m_textures[14] = Stulu::Texture2D::create("assets/game/textures/ui/gameover.png");

	m_textures[13] = Stulu::Texture2D::create("assets/game/textures/enemy/enemy.png");

}
float shootTimer = .0f;
void Layer2D::onUpdate(Stulu::Timestep timestep) {
	cameraController.onUpdate(timestep);


	Stulu::RenderCommand::clear();
	Stulu::Renderer2D::beginScene(cameraController.getCamera());

	switch (state)
	{
	case Menu:
		drawMenu(timestep);
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

Stulu::Quad StartbuttonQuad = { glm::vec2(-.92f / 2.0f, -.36f / 2.0f), .92f, .36f };
bool gameOverMouseDown = false;
void Layer2D::drawMenu(float timestep){

	cameraController.setZoomLevel(cameraController.minZoom);
	glm::vec3 mPos = Stulu::Math::screenToWorld(glm::vec2((float)Stulu::Input::getMouseX(), (float)Stulu::Input::getMouseY()), cameraController.getCamera(), cameraController.getScreenSize());
	if ((Stulu::Math::isPosOverQuad(StartbuttonQuad, glm::vec2(mPos)) || Stulu::Input::isKeyDown(KEY_ENTER))) {

		if (Stulu::Input::isMouseDown(MOUSE_BUTTON_1) && !gameOverMouseDown)
			startGame();

		Stulu::Renderer2D::drawTexturedQuad(m_textures[12], glm::vec2(.0f), glm::vec2(.92f, .36f), glm::vec2(1.0f), glm::vec4(.85f));
	}
	else
		Stulu::Renderer2D::drawTexturedQuad(m_textures[12], glm::vec2(.0f), glm::vec2(.92f, .36f));

	if (gameOverMouseDown)
		gameOverMouseDown = Stulu::Input::isMouseDown(MOUSE_BUTTON_1);

}
void Layer2D::drawGame(float timestep){
	cameraController.getTransform().position = player.transform.position;
	handlePlayer(timestep);
	drawMap();
	handleEnemys(timestep);
	if(state == GameState::Running)
		drawPlayerAndBullets(timestep);
}
Stulu::Quad GameOverbuttonQuad = { glm::vec2(-1.0f / 2.0f, -.71 / 2.0f), 1.0f, .71f };
void Layer2D::drawGameOver() {
	cameraController.setZoomLevel(cameraController.minZoom);
	glm::vec3 mPos = Stulu::Math::screenToWorld(glm::vec2((float)Stulu::Input::getMouseX(), (float)Stulu::Input::getMouseY()), cameraController.getCamera(), cameraController.getScreenSize());
	if (Stulu::Math::isPosOverQuad(GameOverbuttonQuad, glm::vec2(mPos)) || Stulu::Input::isKeyDown(KEY_ENTER)) {

		if (Stulu::Input::isMouseDown(MOUSE_BUTTON_1)) {
			gameOverMouseDown = true;
			state = GameState::Menu;
		}

		Stulu::Renderer2D::drawTexturedQuad(m_textures[14], glm::vec2(.0f), glm::vec2(1.0f, .71f), glm::vec2(1.0f), glm::vec4(.85f));
	}
	else
		Stulu::Renderer2D::drawTexturedQuad(m_textures[14], glm::vec2(.0f), glm::vec2(1.0f, .71f));
}

void Layer2D::drawMenuGUI() {
	ImGui::Begin("World Setting");
	ImGui::DragInt("Seed", (int*)&seed, 1.0f, 1, 999999);
	ImGui::Text("Map size is: size * size!");
	ImGui::DragInt("Map Size", &mapSize, 1.0f, 20, 50000);
	ImGui::SliderFloat("Tree density", &treeDensity, .0f, 1.0f);
	ImGui::End();
}
void Layer2D::drawGameGUI(float timestep) {
	ImVec2 wpos = ImGui::GetWindowPos();
	drawText(wpos, COLOR_WHITE, std::string("FPS: ") + std::to_string(1.0f / timestep));
	drawText(ImVec2(wpos.x, wpos.y + 20.0f), COLOR_WHITE, std::string("Score: ") + std::to_string((int)(alive * kills)));
	drawText(ImVec2(wpos.x, wpos.y + 40.0f), COLOR_WHITE, std::string("Ammo: ") + std::to_string(ammo));
	drawText(ImVec2(wpos.x, wpos.y + 60.0f), COLOR_WHITE, std::string("Difficulty: ") + std::to_string(difficulty));
	ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	ImVec2 cursorPos = ImGui::GetMousePos();
	if (Stulu::Input::isMouseDown(MOUSE_BUTTON_1))
		ImGui::GetForegroundDrawList()->AddImage((void*)m_textures[11]->getRendererID(), ImVec2(cursorPos.x - 15.0f, cursorPos.y - 15.0f), ImVec2(cursorPos.x + 15.0f, cursorPos.y + 15.0f));
	else
		ImGui::GetForegroundDrawList()->AddImage((void*)m_textures[10]->getRendererID(), ImVec2(cursorPos.x - 15.0f, cursorPos.y - 15.0f), ImVec2(cursorPos.x + 15.0f, cursorPos.y + 15.0f));
}
void Layer2D::drawGameOverGUI() {

}

void Layer2D::drawText(ImVec2 pos, glm::vec4 color, std::string text, glm::vec4 colorBG) {
	ImGui::GetForegroundDrawList()->AddRectFilled(pos, ImVec2(pos.x + 10.0f * text.length(), pos.y + 20.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(colorBG.x, colorBG.y, colorBG.z, colorBG.w)));
	ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 18.0f, pos, ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, color.w)), text.c_str());
}

void Layer2D::startGame() {
	newMap();
	player.texture = m_textures[5];
	findPlayerSpawn(1);
	state = GameState::Running;
}

char Layer2D::generateTile(glm::vec2 pos) {
	char m;
	float y = Stulu::Math::perlinAccumalatedNosie((float)pos.x, (float)pos.y, 16, 5.0f, (float)mapSize, (float)mapSize);
	float z = Stulu::Random::getFloat();
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
		m = (z >= treeDensity ? ' ' : 'T');
	}
	else if (y < .8) {
		m = 'S';
	}
	else {
		m = (z >= treeDensity ? ' ' : 'T');
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
	knife = ammo < 1;
	player.transform.scale = glm::vec3(1.0f);
	player.texture = m_textures[8];



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
	if (ammo < 1) {
		return;
	}
	player.transform.scale = glm::vec3(1.0f,2.0f,1.0f);
	player.texture = m_textures[7];
	if (!canShoot)
		return;

	ammo--;
	Bullet* bullet = new Bullet();
	bullet->speed = 15.0f;
	bullet->texture = m_textures[9];
	bullet->transform = Stulu::Transform(glm::vec3(player.transform.position.x ,player.transform.position.y, -.1f) + glm::rotate(player.transform.getOrientation() ,glm::vec3(.15f,.3f,1.0f) / 1.5f), player.transform.rotation, glm::vec3(.07f,.25f,1.0f) / 1.5f);
	bullet->velocity = player.transform.upDirection();
	bullets.push_back(bullet);
	canShoot = false;
}
void Layer2D::handleEnemys(float timestep) {
	if (enemys.size() < (int)std::floor(difficulty)) {
		GameObject* en = new GameObject();
		en->texture = m_textures[13];
		en->transform.scale = glm::vec3(.4f,.4f, 1.0f);
		en->transform.position = glm::vec3(Stulu::Random::getVector2(glm::vec2(player.transform.position.x - viewDistance / 2, player.transform.position.y - viewDistance / 2), glm::vec2(player.transform.position.x + viewDistance / 2, player.transform.position.y + viewDistance / 2)), -.15f);
		while(glm::distance(player.transform.position, en->transform.position) < 5.0f)
			en->transform.position = glm::vec3(Stulu::Random::getVector2(glm::vec2(player.transform.position.x - viewDistance/2, player.transform.position.y - viewDistance / 2),glm::vec2(player.transform.position.x + viewDistance / 2, player.transform.position.y + viewDistance / 2)), -.15f);
		enemys.push_back(en);
	}
	for (int i = 0; i < enemys.size(); i++) {
		GameObject* enemy = enemys[i];
		if (glm::distance(player.transform.position, enemys[i]->transform.position) > (float)viewDistance || !inMap(glm::vec2(enemys[i]->transform.position))) {
			enemys.erase(enemys.begin() + i);
			continue;
		}
		glm::vec3 direction = enemys[i]->transform.upDirection();
		enemys[i]->transform.rotation.z = Stulu::Math::lookAt2D(enemys[i]->transform.position, player.transform.position);
		if (canWalkOn(glm::vec2(enemys[i]->transform.position + direction))) {
			enemys[i]->transform.position += direction * .75f * timestep;
		}
		else {
			enemys[i]->transform.position += direction * .25f * timestep;
		}
		bool killed = false;
		for (int b = 0; b < bullets.size(); b++) {
			if (Stulu::Math::isPosOverQuad(enemys[i]->hitbox(), glm::vec2(bullets[b]->transform.position))) {
				enemys.erase(enemys.begin() + i);
				bullets.erase(bullets.begin() + b);
				ammo += 2;
				kills++;
				killed = true;
				break;
			}
		}
		if (killed)
			continue;

		if (Stulu::Math::isPosOverQuad(enemy->hitbox(), glm::vec2(player.transform.position))) {

			if (knife && Stulu::Input::isMouseDown(MOUSE_BUTTON_1)) {
				enemys.erase(enemys.begin() + i);
				ammo++;
				return;
			}
			onGameOver();
			return;
		}
		
		Stulu::Renderer2D::drawTexturedQuad(enemy->transform, enemy->texture);

	}
	difficulty += timestep/5.0f;
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
		if (glm::distance(bullets[i]->transform.position, player.transform.position) > viewDistance) {
			bullets.erase(bullets.begin() + i);
			continue;
		}
		bullets[i]->transform.position += bullets[i]->velocity * bullets[i]->speed * timestep;
		Stulu::Renderer2D::drawTexturedQuad(bullets[i]->transform, bullets[i]->texture);
	}
	if (knife)
		if(Stulu::Input::isMouseDown(MOUSE_BUTTON_1))
			player.texture = m_textures[16];
		else
			player.texture = m_textures[15];

	player.transform.scale /= 1.5f;
	Stulu::Renderer2D::drawTexturedQuad(player.transform, player.texture);
	alive += timestep;
}

void Layer2D::onGameOver() {
	state = GameState::Over;
	player.transform.reset();
	cameraController.getTransform().reset();
	seed = (uint32_t)Stulu::Random::getInt(0, 99999);
	delete(map);
	enemys.clear();
	bullets.clear();


	FILE* scores = fopen("scores.txt","a");
	if (scores != NULL)
	{
		std::string score = std::string("\nScore: ") + std::to_string((int)(alive * kills));
		fputs(score.c_str(), scores);
		fclose(scores);
	}
	canShoot = true;
	alive = .0f;
	kills = 0;
	ammo = 90;
	shootTimer = .0f;
	difficulty = 30.0f;
}
