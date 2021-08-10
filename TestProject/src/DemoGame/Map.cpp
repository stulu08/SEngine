#include "Map.h"

Node::Node(float playerPosX) {
	transform.getPos().x = playerPosX + 4.0f;
	transform.getPos().z = 0.5f;
	transform.setRotation(glm::vec3(1.0f));
	float scaleMax = 1.5f;
	transform.getScale().x = Random::getFloat(.5f,scaleMax);
	transform.getScale().y = Random::getFloat(.5f, scaleMax - transform.getScale().x/2);
}

void Node::update(Timestep s, float playerPosX) {
	float distance = transform.getPos().x - playerPosX;
	if (distance > 5 || distance < -5) {
		Map::get()->deleteNode(this);
	}
}

void Node::draw() {
	Transform temp = transform;
	temp.getPos().y -= .75;
	temp.setScale(glm::vec3(transform.getScale().x));
	Renderer2D::drawTriangle(temp);
	temp.getRotation().z += 180;
	temp.setScale(glm::vec3(transform.getScale().y));
	temp.getPos().y += 1.5;
	Renderer2D::drawTriangle(temp);
}
float deltaForNodeSpawn;
void Map::update(Timestep s, float playerPosX) {
	for (Node* node : m_nodes)
		node->update(s, playerPosX);

	if (m_nodes.size() < 10 && deltaForNodeSpawn >= .75f) {
		spawnNode(playerPosX);
	}
	if (deltaForNodeSpawn > .75f)
		deltaForNodeSpawn = 0.0f;
	deltaForNodeSpawn += s;
}

void Map::imgui() {

}

void Map::draw() {
	for (Node* node : m_nodes)
		node->draw();
}

void Map::spawnNode(float playerPosX) {
	nodeCount++;
	m_nodes.emplace(m_nodes.begin() + m_insertIndex, new Node(playerPosX));
	m_insertIndex++;
}

void Map::deleteNode(Node* node) {
	auto it = std::find(m_nodes.begin(), m_nodes.begin() + m_insertIndex, node);
	if (it != m_nodes.begin() + m_insertIndex) {
		m_nodes.erase(it);
		m_insertIndex--;

		delete(node);
		return;
	}
}
