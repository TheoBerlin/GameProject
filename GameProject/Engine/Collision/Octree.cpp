#include "Octree.h"

#include "../Rendering/GLAbstraction/RenderingResources.h"
#include <Utils/Utils.h>

Octree::Octree()
{
	this->root = new Node();
}

Octree::~Octree()
{
	deleteNode(this->root);

	for (Shape* shape : this->shapes)
		delete shape;
	this->shapes.clear();
}

void Octree::constructOctreeTree(const glm::vec3 & position, const glm::vec3 & size, unsigned int levels)
{
	this->root->aabb.size = size;
	this->root->aabb.center = position;
	makeNodes(this->root, 0, levels);
}

std::vector<Octree::Node*>& Octree::getNodes()
{
	return this->root->children;
}

Octree::Shape * Octree::addShape(Vertex * vertices, unsigned numVertices)
{
	Shape* shape = new Shape();
	shape->obb = makeOBB(vertices, numVertices);
	// TODO: Make shape a separate class. 
	this->shapes.push_back(shape);
	return shape;
}

Octree::AABB Octree::makeAABB(Vertex * vertices, unsigned int numVertices, glm::vec3 e1, glm::vec3 e2, glm::vec3 e3)
{
	AABB aabb;
	glm::vec3 min = glm::vec3(10000.0f);
	glm::vec3 max = glm::vec3(-10000.0f);
	for (unsigned int i = 0; i < numVertices; i++)
	{
		Vertex& vert = vertices[i];
		float e1dot = glm::dot(e1, vert.Position);
		float e2dot = glm::dot(e2, vert.Position);
		float e3dot = glm::dot(e3, vert.Position);
		min.x = glm::min(min.x, e1dot);
		min.y = glm::min(min.y, e2dot);
		min.z = glm::min(min.z, e3dot);

		max.x = glm::max(max.x, e1dot);
		max.y = glm::max(max.y, e2dot);
		max.z = glm::max(max.z, e3dot);
	}

	glm::vec3 pos = e1 * (min.x + (max.x - min.x)*.5f) + e2 * (min.y + (max.y - min.y)*.5f) + e3 * (min.z + (max.z - min.z)*.5f);

	aabb.size = glm::max(max - min, 0.01f);
	aabb.center = pos;
	return aabb;
}

Octree::OBB Octree::makeOBB(Vertex * vertices, unsigned int numVertices)
{
	std::vector<Vertex> verts(vertices, vertices + numVertices);

	// Calculate centroid.
	glm::vec3 centroid(0.0f);
	for (Vertex& v : verts)
		centroid += v.Position;
	centroid /= (float)numVertices;

	// Distance from centroid to vertex.
	std::vector<glm::vec3> changeInPos;
	for (Vertex& v : verts)
		changeInPos.push_back(v.Position - centroid);

	/* Matrix multiplication for a single element.
		c[i][j] = (a*b)[i][j]
	Arguments:
		m1: matrix a
		m2: matrix b
		i: row index
		j: col index
	*/
	auto getElem = [](std::vector<glm::vec3>& m1, std::vector<glm::vec3>& m2, int i, int j)->float {
		float a = 0.0f;
		for (unsigned k = 0; k < m1.size(); k++)
			a += m1[k][i] * m2[k][j];
		return a;
	};

	// Calculate the covariance matrix. By multiplying the Nx3 matrix with its transpose from the left we get a 3x3 matrix which will be the covariance matrix after normalization. 
	glm::mat3 cov;
	for (unsigned i = 0; i < 3; i++)
		for (unsigned j = 0; j < 3; j++)
			cov[j][i] = getElem(changeInPos, changeInPos, i, j) / 3.f; // Multiply by its transpose and divide by the dimension size to normalize it.

	// Calculate the eigenvectors from the covariance matrix. This will also give us the eigenvalues as a byproduct but we will not use them.
	std::pair<glm::vec3, glm::mat3> jacobiResult = Utils::jacobiMethod(cov);

	glm::vec3 e1 = glm::normalize(jacobiResult.second[0]);
	glm::vec3 e2 = glm::normalize(jacobiResult.second[1]);
	glm::vec3 e3 = glm::normalize(jacobiResult.second[2]);

	// Get the AABB in the base [e1 e2 e3].
	AABB aabb = makeAABB(vertices, numVertices, e1, e2, e3);

	// Rotate the x-axis to the first eigenvector.
	glm::vec3 vx = glm::vec3{ 1.f, 0.f, 0.f };
	glm::quat rotX = Utils::rotateTo(vx, e1);

	// If the first eigenvector is parallel to the x-axis, do not rotate.
	float d = glm::dot(vx, e1);
	if (abs(d) > 0.999f)
		rotX = glm::quat(1.f, 0.f, 0.f, 0.f);

	// Rotate the z-axis by the previous rotation and then rotate that vector to the second eigenvector.
	// This is equivalent to make a roll around the previously calculated axis to match the second and third eigenvector..
	glm::vec3 vz = glm::normalize(rotX * glm::vec3(0.f, 0.f, 1.f));
	glm::quat roll = Utils::rotateTo(vz, e3);

	// If the second eigenvector is parallel to the new z-axis, do not rotate.
	d = glm::dot(e3, vz);
	if (abs(d) > 0.999f)
		roll = glm::quat(1.f, 0.f, 0.f, 0.f);

	OBB obb;
	obb.center = aabb.center;
	obb.size = aabb.size;
	obb.rotation = roll * rotX;
	return obb;
}

void Octree::makeNodes(Node * node, unsigned int currLevel, unsigned int maxLevel)
{
	if (currLevel < maxLevel)
	{
		glm::vec3 nodeSize = node->aabb.size / 2.f;
		glm::vec3 nodePosition = node->aabb.center;
		for (int x = -1; x <= 1; x += 2) {
			for (int y = -1; y <= 1; y += 2) {
				for (int z = -1; z <= 1; z += 2)
				{
					Node* child = new Node();
					AABB aabb = { nodePosition + glm::vec3(nodeSize.x*x, nodeSize.y*y, nodeSize.z*z)*.5f, nodeSize };
					child->aabb = aabb;
					child->color = node->color - glm::vec3(1.f/(float)(maxLevel+1));
					node->children.push_back(child);
					makeNodes(child, currLevel + 1, maxLevel);
				}
			}
		}
	}
}

void Octree::deleteNode(Node* node)
{
	for (Node* node : node->children)
		deleteNode(node);
	delete node;
}
