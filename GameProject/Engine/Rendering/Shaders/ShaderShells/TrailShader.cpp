#include "TrailShader.h"

#include "Engine/Components/Camera.h"
#include "Engine/Rendering/Display.h"
#include "Game/Components/TrailEmitter.h"

TrailShader::TrailShader(Camera ** camera)
	: Shader("./Engine/Rendering/Shaders/TrailEffect.vert", "./Engine/Rendering/Shaders/TrailEffect.frag"),
	trailColor(1.0f, 0.0f, 0.0f)
{
	this->camera = camera;
	this->pointCount = pointCount;

	AttributeLayout trailLayout;
	trailLayout.push(4, 0, 4); // vec4 previous
	trailLayout.push(4, 8, 4); // vec4 current

	mesh = new Mesh(NULL, 0, NULL, 0, trailLayout, GL_DYNAMIC_DRAW);

	AttributeLayout alphaWidthLayout;
	alphaWidthLayout.push(1); // float alpha
	alphaWidthLayout.push(1); // vec4 width
	alphaWidthLayout.push(3); // upVector alpha

	mesh->addBuffer(NULL, 0, alphaWidthLayout, GL_DYNAMIC_DRAW);

}

TrailShader::~TrailShader()
{
	delete mesh;
}

void TrailShader::bind()
{
	Shader::bind();
	
	Shader::setUniformMatrix4fv("proj", 1, false, &((*this->camera)->getProj()[0][0]));
	Shader::setUniformMatrix4fv("view", 1, false, &((*this->camera)->getView()[0][0]));
	Shader::setUniform3fv("cameraPos", 1, &((*this->camera)->getPosition()[0]));
	Shader::setUniform3fv("trailColor", 1, &(this->trailColor[0]));

	this->setHorizontal(true);

	mesh->bindVertexArray();

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TrailShader::unbind()
{
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void TrailShader::setHorizontal(bool horiz)
{
	this->horizontal = horiz;
	Shader::setUniform1b("horizontal", this->horizontal);
}

size_t TrailShader::getDrawCount() const
{
	if (pointCount >= 4)
		return (this->pointCount - 1) * 2;
	else
		return 0;
	
}

void TrailShader::updateTrail(const std::vector<TrailPointData>& pointData, const glm::vec3& color)
{
	this->pointCount = pointData.size();

	if (pointData.size() < 4)
		return;

	this->trailColor = color;

	

	std::vector<glm::vec4> positionData;
	std::vector<float> metaData;

	for (size_t i = 0; i < pointData.size(); i++) {
		glm::vec4 pos;

		pos.x = pointData[i].pos.x;
		pos.y = pointData[i].pos.y;
		pos.z = pointData[i].pos.z;

		pos.w = 1.0;
		positionData.push_back(pos);

		metaData.push_back(pointData[i].alpha);
		metaData.push_back(pointData[i].width);

		glm::vec3 upVector = pointData[i].upVector;
		metaData.push_back(upVector.x);
		metaData.push_back(upVector.y);
		metaData.push_back(upVector.z);

		pos.w = -1.0;
		positionData.push_back(pos);

		metaData.push_back(pointData[i].alpha);
		metaData.push_back(pointData[i].width);
		metaData.push_back(upVector.x);
		metaData.push_back(upVector.y);
		metaData.push_back(upVector.z);
	}

	mesh->updateInstancingData((void*)(&positionData[0][0]), positionData.size() * sizeof(glm::vec4), 0, 0);
	mesh->updateInstancingData((void*)(&metaData[0]), metaData.size() * sizeof(float), 0, 1);
}
