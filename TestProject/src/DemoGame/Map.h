#pragma once
#include <Stulu.h>

using namespace Stulu;

class Node {
public:
	Node(float playerPosX);
	void update(Timestep s, float playerPosX);
	void draw();
	Transform transform;
};

class Map {
public:
	Map() {
		inst = this;
	}
	virtual ~Map(){}
	void update(Timestep s, float playerPosX);
	void imgui();
	void draw();
	void spawnNode(float playerPosX);
	void deleteNode(Node* node);

	inline static Map* get() { return inst; };
private:
	std::vector<Node*> m_nodes;
	unsigned int m_insertIndex = 0;

	int nodeCount = 0;

	inline static Map* inst;
};