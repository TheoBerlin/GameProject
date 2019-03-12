#include "TrailEmitter.h"

#include "Engine/Entity/Entity.h"

#include "Engine/Rendering/Display.h"
#include "Engine/Rendering/Renderer.h"

#include "Game/Components/ArrowGuider.h"


TrailEmitter::TrailEmitter(Entity * host) : Component(host, "TrailEmitter")
{
	glm::vec3 forward = this->host->getTransform()->getForward();
	this->startPos = this->host->getTransform()->getPosition() - forward;
	this->currentPos = this->startPos - forward;
	this->forwardPos = this->startPos + forward;

	this->startUpVector = this->host->getTransform()->getUp();

	this->timer = 0.0f;
	this->trailDuration = 4.0f;
}

TrailEmitter::~TrailEmitter()
{
}

void TrailEmitter::update(const float & dt)
{
	ArrowGuider* arrowGuider = dynamic_cast<ArrowGuider*>(this->host->getComponent("ArrowGuider"));

	if (arrowGuider) {
		this->path = arrowGuider->getPath();
	}

	this->timer += dt;
	if (this->path.size() > 0) {
		Pipeline* pipeline = Display::get().getRenderer().getPipeline();

		std::vector<glm::vec3> pointData;
		std::vector<glm::vec3> upVectorData;

		if (timer < trailDuration) {
			pointData.push_back(this->startPos);
			upVectorData.push_back(this->startUpVector);
		}

		for (KeyPoint& p : this->path) {
			if ((timer - p.t) < trailDuration) {
				pointData.push_back(p.Position);
				upVectorData.push_back(p.UpVector);
			}

		}

		//Add current position of entity after the last point
		pointData.push_back(this->host->getTransform()->getPosition() - this->host->getTransform()->getForward() * 0.2f);
		glm::vec3 currentUp = this->host->getTransform()->getUp();
		upVectorData.push_back(currentUp);

		pointData.push_back(this->forwardPos);
		upVectorData.push_back(currentUp);

		this->forwardPos = this->host->getTransform()->getPosition() + this->host->getTransform()->getForward();

		pipeline->updateTrail(pointData, upVectorData);
	}
}

void TrailEmitter::resetTrailTimer()
{
	this->timer = 0.0f;
}
