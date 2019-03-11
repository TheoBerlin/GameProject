#include "TrailShader.h"

#include "Engine/Components/Camera.h"
#include "Engine/Rendering/Display.h"

TrailShader::TrailShader(const std::string & vertex, const std::string & fragment, Camera ** camera)
	: Shader("./Engine/Rendering/Shaders/TrailEffect.vert", "./Engine/Rendering/Shaders/TrailEffect.frag")
{
	std::vector<glm::vec4> points;

	for(size_t i = 0; i < 6; i++) {
		glm::vec4 pos;

		pos.x = 0.0f;
		pos.y = 5.0f;
		pos.z = i * 2.0f;

		pos.w = 1.0;
		points.push_back(pos);
		pos.w = -1.0;
		points.push_back(pos);
	}

	AttributeLayout trailLayout;
	trailLayout.push(4, 0, 4); // vec4 last
	trailLayout.push(4, 8, 4); // vec4 current
	trailLayout.push(4, 16, 4); // vec4 next

	mesh = new Mesh((void*)(&points[0][0]), points.size() * sizeof(glm::vec4), NULL, 0, trailLayout, GL_DYNAMIC_DRAW);
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

	const float width = 0.13f;
	Shader::setUniform1f("width", width);

	glm::vec2 viewport;
	viewport.x = Display::get().getWidth();
	viewport.y = Display::get().getHeight();
	Shader::setUniform2fv("viewPort", 1, &viewport[0]);

	mesh->bindVertexArray();
}
