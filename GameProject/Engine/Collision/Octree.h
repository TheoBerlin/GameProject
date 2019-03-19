#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <unordered_map>

class Entity;
struct Vertex;
class Octree
{
public:
	struct Triangle
	{
		glm::vec3 v1;
		glm::vec3 v2;
		glm::vec3 v3;
	};

	struct AABB
	{
		glm::vec3 center;
		glm::vec3 size;
	};

	struct OBB
	{
		glm::vec3 center;
		glm::vec3 size;
		glm::quat rotation;
	};

	struct Node;
	struct Shape
	{
		std::vector<Triangle> triangles;
		OBB obb;
		std::vector<Node*> parent;
	};

	struct Node
	{
		glm::mat4 transform;
		glm::vec3 color = {1.f, 1.f, 1.f};
		AABB aabb;
		std::vector<Shape*> shapes;
		std::vector<Node*> children;
	};

	Octree();
	virtual ~Octree();

	/*
	Construct the octree.
	Arguments:
		position: The center position of the octree.
		size: The size of the octree.
		levels: How deep the octree is.
	*/
	void constructOctreeTree(const glm::vec3& position, const glm::vec3& size, unsigned int levels = 2);

	/*
	Get the list of the first level nodes.
	*/
	std::vector<Node*>& getNodes();

	Shape* createShape(Vertex* vertices, unsigned numVertices, unsigned char* indices, unsigned numIndices);

	void addShapeToEntity(Entity* entity, Shape* shape);

	/*
	---------------------- UTILS ------------------------
	*/
	AABB makeAABB(Vertex* vertices, unsigned int numVertices, glm::vec3 e1 = { 1.f, 0.f, 0.f }, glm::vec3 e2 = { 0.f, 1.f, 0.f }, glm::vec3 e3 = { 0.f, 0.f, 1.f });
	OBB makeOBB(Vertex* vertices, unsigned int numVertices);

private:
	void makeNodes(Node* node, unsigned int currLevel, unsigned int maxLevel);
	void deleteNode(Node* node);

	Node* root;											// Structure of the octree.
	// Key: pointer to the shape
	std::unordered_map<unsigned int, Shape*> shapes;	// Each node can have a referance to a shape. Each shape has vertices.
	std::unordered_map <Entity*, Node*> entitesMap;
};