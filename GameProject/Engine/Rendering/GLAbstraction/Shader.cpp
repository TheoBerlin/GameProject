#include "Shader.h"

#include "../../../Utils/Logger.h"

Shader::Shader()
{
	init("", "");
}

Shader::Shader(const std::string& vertex, const std::string& fragment)
{
	init(vertex, fragment);
}

Shader::~Shader()
{
	glDeleteProgram(this->id);
}

void Shader::recreate(const std::string & vertex, const std::string & fragment)
{
	glDeleteProgram(this->id);
	init(vertex, fragment);
}

void Shader::bind()
{
	glUseProgram(this->id);
}

void Shader::unbind()
{
	glUseProgram(0);
}

const GLuint Shader::getID() const
{
	return this->id;
}

void Shader::setUniform1f(const std::string & name, float f)
{
	GLint location = addUniform(name);
	if(location != -1)
		glUniform1f(location, f);
}

void Shader::init(const std::string& vertex, const std::string& fragment)
{
	this->id = glCreateProgram();
	
	if (vertex.empty() || fragment.empty())
		return;

	if (addShaderPart(vertex, GL_VERTEX_SHADER) == false)
		return;
	if (addShaderPart(fragment, GL_FRAGMENT_SHADER) == false)
		return;

	identifyName(vertex);

	link();
}

GLint Shader::addUniform(const std::string & name)
{
	if (this->uniforms.find(name) == this->uniforms.end())
	{
		GLint location = glGetUniformLocation(this->id, name.c_str());
		if (location == -1) {
			LOG_ERROR("[Shader: %s] Can't find uniform '%s'!", this->name.c_str(), name.c_str());
			return -1;
		}
		this->uniforms.insert({name, location});
	}
	return this->uniforms[name];
}

bool Shader::addShaderPart(const std::string & name, GLuint type)
{
	GLuint shaderPartID = loadShaderPart(name, type);
	if (shaderPartID != 0) {
		if (compileShaderPart(shaderPartID, type)) {
			glAttachShader(this->id, shaderPartID);
			glDeleteShader(shaderPartID);
			return true;
		}
	}
	return false;
}

GLuint Shader::loadShaderPart(const std::string & name, GLuint type)
{
	GLuint id = glCreateShader(type);
	if (id == 0) {
		LOG_ERROR("[Shader: %s] Could not create shader: %s", this->name.c_str(), name.c_str());
		return 0;
	}

	std::ifstream file(name);
	std::string shaderText;
	if (file.is_open())
	{
		shaderText = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
	}
	else
	{
		LOG_ERROR("[Shader: %s] Could not open shader: %s", this->name.c_str(), name.c_str());
		return 0;
	}
	const char* shaderTextPtr = shaderText.c_str();
	glShaderSource(id, 1, &shaderTextPtr, 0);
	this->ids.push_back(id);
	return id;
}

bool Shader::compileShaderPart(GLuint id, GLuint type)
{
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = GL_FALSE;
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		glGetShaderInfoLog(id, 1024, nullptr, buff);
		std::string shaderType = type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader";
		LOG_ERROR("[Shader: %s] Could not compile %s: %s", this->name.c_str(), shaderType.c_str(), buff);
		return false;
	}
	return true;
}

bool Shader::link()
{
	glLinkProgram(this->id);

	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = GL_FALSE;
	glGetProgramiv(this->id, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		memset(buff, 0, 1024);
		glGetProgramInfoLog(this->id, 1024, nullptr, buff);
		buff[1023] = '\n';
		LOG_ERROR("[Shader: %s] Failed to link shader program: %s", this->name.c_str(), buff);
		return false;
	}

	for (GLuint partID : this->ids)
		glDetachShader(this->id, partID);
	return true;
}

void Shader::identifyName(const std::string & path)
{
	std::size_t found = path.rfind("/");
	if (found == std::string::npos)
		found = path.rfind("\\");
	if (found == std::string::npos)
		found = 0;
	if (found != std::string::npos) {
		this->name = path.substr(found + (found != 0 ? 1 : 0));
		found = this->name.rfind(".");
		this->name = this->name.substr(0, found);
	}
	else {
		LOG_WARNING("Could not identify name of shader: %s", path.c_str());
	}
}
