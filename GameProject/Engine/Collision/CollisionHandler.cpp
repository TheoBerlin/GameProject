#include "CollisionHandler.h"
#include "Engine/Entity/Entity.h"
#include "Utils/Logger.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "GLFW/glfw3.h"

#include "../Rendering/GLAbstraction/RenderingResources.h"

CollisionHandler::CollisionHandler()
{
	// Set default values
	this->takenBodies = 0;
	this->player = nullptr;

	// Give pointer to entites to collision
	this->collision.setEntitesPointer(&this->entities);

	// Set settings for world
	rp3d::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 20;

	// Create the world with the settings
	this->world = new rp3d::CollisionWorld(settings);

	// Create the shapes used by bodies
	createShapes();
#ifdef ENABLE_COLLISION_BOXES
	//Register callback for drawing GUI
	EventBus::get().subscribe(this, &CollisionHandler::toggleDrawing);

	this->drawCollisionShapes = false;
#endif
}


CollisionHandler::~CollisionHandler()
{
	// Delete world, deletes bodies and their proxyshapes aswell
	delete this->world;

	// Clear the bodies list of pointers
	this->bodies.clear();

	for (auto shapeVector : this->shapes)
		for (auto data : shapeVector) {
			delete data->shape;
			delete data;
		}
}

void CollisionHandler::checkCollision()
{
	if (this->player)
		this->world->testCollision(this->player, &this->collision);
	else
		this->world->testCollision(&this->collision);
}

void CollisionHandler::createCollisionBodies(int num)
{
	size_t previousSize = this->bodies.size();
	this->bodies.resize(this->bodies.size() + num);

	// Standard values for bodies which is not in use
	rp3d::Vector3 initPosition(0.0, 0.0, 0.0);
	rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
	rp3d::Transform transform(initPosition, initOrientation);

	// Create the bodies in the world and add them to the map
	for (size_t i = previousSize; i < num; i++)
	{
		this->bodies[i] = this->world->createCollisionBody(transform);
	}
}

rp3d::CollisionBody * CollisionHandler::getUnusedBody()
{
	if (this->takenBodies == this->bodies.size())
		return nullptr;
	else
		return this->bodies[this->takenBodies++];
}

void CollisionHandler::addCollisionToEntity(Entity * entity, SHAPE shape)
{
	if (entity == nullptr)
	{
		LOG_WARNING("Nullptr as entity!");
		return;
	}

	rp3d::CollisionBody* entityBody = getUnusedBody();
	if (!entityBody)
	{
		LOG_WARNING("No empty collisionBodies!");
		return;
	}

	rp3d::Vector3 pos = toReactVec(entity->getTransform()->getPosition());
	rp3d::Quaternion rotation;
	glm::quat q = entity->getTransform()->getRotationQuat();
	rotation.setAllValues(q.x, q.y, q.z, q.w);
	rp3d::Transform transform(pos, rotation);

	/*
	SET SHAPE TO COLLISIONBODY. IF SHAPE IS ARROW/PLAYER SAVE THE POINTER FOR COLLISION CHECK
	*/

	if (shape == SHAPE::ARROW)
		this->player = entityBody;


	rp3d::Quaternion shapeRot = rp3d::Quaternion::identity();
#ifdef ENABLE_COLLISION_BOXES

	for (auto data : this->shapes[(size_t)shape]) {
		rp3d::ProxyShape* proxyShape = entityBody->addCollisionShape(data->shape, rp3d::Transform(this->toReactVec(data->pos), shapeRot));
		proxyShape->setUserData((void*)data);
		this->proxyShapes.push_back(proxyShape);
	}

#else

	for (auto data : this->shapes[(size_t)shape]) {
		entityBody->addCollisionShape(data->shape, rp3d::Transform(this->toReactVec(data->pos), shapeRot));
	}

#endif


	entityBody->setTransform(transform);

	// Set the body for the entity
	entity->setCollisionBody(entityBody);
	// Save the pair of entity and body for collision detection later
	this->entities[entityBody] = entity;
}

void CollisionHandler::removeCollisionBody(rp3d::CollisionBody * body)
{
	for (size_t i = 0; i < this->bodies.size(); i++)
	{
		if (this->bodies[i] == body)
		{
			this->world->destroyCollisionBody(body);
			this->bodies[i] = this->bodies[this->bodies.size() - 1];
			this->bodies.pop_back();
			this->takenBodies--;
		}
	}
}

void CollisionHandler::removeCollisionBody(Entity * entity)
{
	rp3d::CollisionBody* body = entity->getCollisionBody();
	
	rp3d::ProxyShape* current = body->getProxyShapesList();

	// Look for the proxy shape that contains the collision shape in parameter
	while (current != nullptr) {

		// Remove the proxy collision shape
		rp3d::ProxyShape* nextElement = current->getNext();
		
		// TEMPORARY
		rp3d::ProxyShape* proxyPtr;
		size_t size = this->proxyShapes.size();
		for (size_t i = 0; i < size; i++) {
			proxyPtr = this->proxyShapes[i];
			if (proxyPtr == current) {
				this->proxyShapes[i] = this->proxyShapes[size - 1];
				this->proxyShapes.pop_back();
			}
		}

		body->removeCollisionShape(current);

		// Get the next element in the list
		current = nextElement;
	}

	for (size_t i = 0; i < this->bodies.size(); i++)
	{
		if (this->bodies[i] == body)
		{
			rp3d::CollisionBody* temp = this->bodies[this->takenBodies - 1];
			this->bodies[this->takenBodies - 1] = body;
			this->bodies[i] = temp;
			this->takenBodies--;
			break;
		}
	}
}

void CollisionHandler::addShape(const std::string & name, Vertex* vertices, unsigned int numVertices)
{
	std::vector<Vertex> verts(vertices, vertices+numVertices);
	
	// Calculate centroid.
	glm::vec3 centroid;
	for (Vertex& v : verts)
		centroid += v.Position;
	centroid /= numVertices;

	// Distance from centroid to vertex.
	std::vector<glm::vec3> vertsDist;
	for (Vertex& v : verts)
		vertsDist.push_back(v.Position-centroid);
	
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

	// Calculate the covariance matrix
	float cov[3][3];
	for (unsigned i = 0; i < 3; i++)
		for (unsigned j = 0; j < 3; j++)
			cov[j][i] = getElem(vertsDist, vertsDist, i, j);

	/*
	TODO: 
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);
	Vector3 axis = eig.eigenvectors().col(2).normalized();
	*/
}

rp3d::Vector3 CollisionHandler::toReactVec(const glm::vec3 & vec)
{
	return { vec.x, vec.y, vec.z };
}

glm::vec3 CollisionHandler::toGlmVec(const rp3d::Vector3 & vec)
{
	return { vec.x, vec.y, vec.z };
}

glm::quat CollisionHandler::toGlmQuat(const rp3d::Quaternion & vec)
{
	glm::quat q;
	q.x = vec.x;
	q.y = vec.y;
	q.z = vec.z;
	q.w = vec.w;

	return q;
}

void CollisionHandler::createShapes()
{
	this->shapes.resize((size_t)SHAPE::SIZE);

	// Create and add shapes to the shapes vector. Order is important!

	
#ifdef ENABLE_COLLISION_BOXES
	//// DRONE = 0
	this->addShape(SHAPE::DRONE, { 0.25f, 0.25f, 0.25f }, { 0.15f, 0.5f, 0.15f });
	this->addShape(SHAPE::DRONE, { 0.05f, 0.05f, 0.05f }, { 0.75f, 0.15f, 0.15f }, { 0.0f, 0.05f, -0.25f });

	//// BOX = 1
	this->addShape(SHAPE::BOX, { 0.5f, 0.5f, 0.5f });

	//// ARROW = 2
	this->addShape(SHAPE::ARROW, { 0.05f, 0.05f, 0.5f }, { 0.0f, 0.0f, 1.0f });
#else
	//// DRONE = 0 ---- CHANGE TO A MESH WHEN DONE
	this->addShape(SHAPE::DRONE, { 0.25f, 0.25f, 0.25f }); 

	//// BOX = 1
	this->addShape(SHAPE::BOX, { 0.5f, 0.5f, 0.5f });

	//// ARROW = 2 ---- CHANGE TO A MESH WHEN DOWN
	this->addShape(SHAPE::ARROW, { 0.05f, 0.05f, 0.5f });
#endif
}



#ifdef ENABLE_COLLISION_BOXES

void CollisionHandler::toggleDrawing(KeyEvent * ev)
{
	if (ev->key == GLFW_KEY_F3 && ev->action == GLFW_PRESS) {
		this->drawCollisionShapes = !this->drawCollisionShapes;
	}
}


void CollisionHandler::addShape(SHAPE shape, const glm::vec3& scale, const glm::vec3& color, const glm::vec3& pos)
{
	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	data->color = color;
	data->scale = scale;
	data->pos = pos;

	rp3d::BoxShape * boxShape = new rp3d::BoxShape(toReactVec(data->scale));
	data->shape = boxShape;

	this->shapes[(size_t)shape].push_back(data);
}

void CollisionHandler::updateDrawingData()
{
	if (this->drawCollisionShapes) {
		this->matrices.clear();
		this->colors.clear();

		for (auto proxyShape : this->proxyShapes) {
			CollisionShapeDrawingData* data = (CollisionShapeDrawingData*)(proxyShape->getUserData());
			rp3d::Transform trans = proxyShape->getLocalToWorldTransform();
			glm::vec3 eulerAngles = glm::eulerAngles(this->toGlmQuat(trans.getOrientation()));

			glm::mat4 mat(1.0f);
			mat = glm::translate(mat, this->toGlmVec(trans.getPosition()));
			mat = glm::rotate(mat, eulerAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));
			mat = glm::rotate(mat, eulerAngles.y, glm::vec3(0.0f, 1.0f, 0.0f));
			mat = glm::rotate(mat, eulerAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));
			mat = glm::scale(mat, data->scale);
			this->matrices.push_back(mat);
			this->colors.push_back(data->color);
		}

		this->cRenderer.updateColors(colors);
		this->cRenderer.updateMatrices(matrices);
	}

}

void CollisionHandler::drawCollisionBoxes()
{
	if(this->drawCollisionShapes)
		this->cRenderer.render();
}
#else


void CollisionHandler::addShape(SHAPE shape, const glm::vec3& scale, const glm::vec3 & pos)
{
	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	data->pos = pos;
	data->shape = new rp3d::BoxShape(this->toReactVec(scale));

	this->shapes[(size_t)shape].push_back(data);
}

#endif