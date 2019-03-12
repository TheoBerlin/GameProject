#include "TrailShader.h"

#include "Engine/Components/Camera.h"
#include "Engine/Rendering/Display.h"

TrailShader::TrailShader(Camera ** camera)
	: Shader("./Engine/Rendering/Shaders/TrailEffect.vert", "./Engine/Rendering/Shaders/TrailEffect.frag")
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
		return (this->pointCount - 2) * 2;
	else
		return 0;
	
}

void TrailShader::updateTrail(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& upVectors)
{
	if (points.size() < 4)
		return;

	this->pointCount = points.size();

	std::vector<glm::vec4> pointData;
	std::vector<float> metaData;

	for (size_t i = 0; i < points.size(); i++) {
		glm::vec4 pos;

		pos.x = points[i].x;
		pos.y = points[i].y;
		pos.z = points[i].z;

		pos.w = 1.0;
		pointData.push_back(pos);

		float width = 0.07;
		float alpha = 0.5;

		metaData.push_back(alpha);
		metaData.push_back(width);
		metaData.push_back(upVectors[i].x);
		metaData.push_back(upVectors[i].y);
		metaData.push_back(upVectors[i].z);

		pos.w = -1.0;
		pointData.push_back(pos);

		metaData.push_back(alpha);
		metaData.push_back(width);
		metaData.push_back(upVectors[i].x);
		metaData.push_back(upVectors[i].y);
		metaData.push_back(upVectors[i].z);
	}

	mesh->updateInstancingData((void*)(&pointData[0][0]), pointData.size() * sizeof(glm::vec4), 0, 0);
	mesh->updateInstancingData((void*)(&metaData[0]), metaData.size() * sizeof(float), 0, 1);
}
