#pragma once

#include "GL/glew.h"
#include <vector>
#include <unordered_map>
#include <string>

class Shader
{
public:
	Shader();
	Shader(const std::string& vertex, const std::string& fragment);
	virtual ~Shader();

	void recreate(const std::string& vertex, const std::string& fragment);

	void bind();
	void unbind();

	const GLuint getID() const;
	void setUniform1f(const std::string& name, float f);

private:
	void init(const std::string& vertex, const std::string& fragment);
	GLint addUniform(const std::string& name);

	bool addShaderPart(const std::string& name, GLuint type);
	GLuint loadShaderPart(const std::string& name, GLuint type);
	bool compileShaderPart(GLuint id, GLuint type);
	bool link();

	void identifyName(const std::string& path);

	std::string name;
	GLuint id;
	std::vector<GLuint> ids;
	std::unordered_map<std::string, GLint> uniforms;

};