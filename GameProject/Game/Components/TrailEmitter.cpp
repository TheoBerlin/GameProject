#include "TrailEmitter.h"

#include "Engine/Entity/Entity.h"

#include "Engine/Rendering/Display.h"
#include "Engine/Rendering/Renderer.h"

#include "Game/Components/ArrowGuider.h"


TrailEmitter::TrailEmitter(Entity * host) : Component(host, "TrailEmitter")
{
	glm::vec3 forward = this->host->getTransform()->getForward();
	this->startPos = this->host->getTransform()->getPosition() - forward;
	this->forwardPos = this->startPos + forward;
	this->startUpVector = this->host->getTransform()->getUp();

	this->startingWidth = 0.025f;
	this->startingAlpha = 0.5f;

	this->timer = 0.0f;
	this->trailDuration = 20.0f;
}

TrailEmitter::~TrailEmitter()
{
	Pipeline* pipeline = Display::get().getRenderer().getPipeline();
	std::vector<TrailPointData> trailPointData;
	// Send in a empty vector to cleanse vbo of trail
	pipeline->updateTrail(trailPointData, glm::vec3(1.0f, 0.0f, 0.0f));
}

void TrailEmitter::update(const float & dt)
{
	if (this->host) {
		ArrowGuider* arrowGuider = dynamic_cast<ArrowGuider*>(this->host->getComponent("ArrowGuider"));

		if (arrowGuider) {
			this->path = arrowGuider->getPath();
		}
	}

	
	//Update trailtimer
	this->timer += dt;

	if (this->path.size() > 0) {
		Pipeline* pipeline = Display::get().getRenderer().getPipeline();

		std::vector<glm::vec3> pointData;
		std::vector<glm::vec3> upVectorData;
		std::vector<TrailPointData> trailPointData;
		TrailPointData tp;

		//Create a starting point to create emission before any keypoints have been placed
		if (timer < trailDuration) {
			tp.pos = this->startPos;
			tp.upVector = this->startUpVector;

			float timeFactor = 1.0f - (timer / trailDuration);

			tp.alpha = timeFactor;
			tp.width = this->startingWidth * timeFactor;

			trailPointData.push_back(tp);
		}

		//Loop through keypoints and update emission lines based on time
		glm::vec3 prevPos;
		size_t loopIndex = 0;
		for (KeyPoint& p : this->path) {
			if ((timer - p.t) < trailDuration && timer > p.t) {
				//If there is more than two arrows create a forward between previous point and current point to get offset of emission point
				if (loopIndex >= 1) {
					glm::vec3 currentForward = glm::normalize(p.Position - prevPos);
					tp.pos = p.Position - currentForward * 0.2f;
				}
				else 
					tp.pos = p.Position;

				tp.upVector = p.UpVector;

				float timeFactor = 1.0f - ((timer - p.t) / trailDuration);

				tp.alpha = timeFactor;
				tp.width = this->startingWidth * timeFactor;

				trailPointData.push_back(tp);
			}
			prevPos = p.Position;
			loopIndex++;
		}

		//Add two points on the arrow so the emissions seems to come from the entity
		glm::vec3 emissionOffset = this->host->getTransform()->getForward() * 0.2f;
		glm::vec3 emissionPoint = this->host->getTransform()->getPosition() - emissionOffset;
		glm::vec3 currentUp = this->host->getTransform()->getUp();

		tp.pos = emissionPoint;
		tp.upVector = currentUp;
		tp.width = this->startingWidth;
		trailPointData.push_back(tp);

		tp.pos = emissionPoint;
		trailPointData.push_back(tp);

		pipeline->updateTrail(trailPointData, glm::vec3(1.0f, 0.0f, 0.0f));
	}
}

void TrailEmitter::setTrailTimer(float elapsedTime)
{
	this->timer = elapsedTime;
}

void TrailEmitter::resetTrailTimer()
{
	this->timer = 0.0f;
}
