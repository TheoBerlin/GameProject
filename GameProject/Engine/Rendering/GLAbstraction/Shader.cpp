#include "Shader.h"

#include "../../../Utils/Logger.h"
#include "Texture.h"

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

void Shader::setTexture2D(const std::string & name, unsigned int unit, const Texture & texture)
{
	setTexture2D(name, unit, texture.getID());
}

void Shader::setTexture2D(const std::string & name, unsigned int unit, GLuint textureID)
{
	setUniform1i(name, unit);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Shader::setUniform1b(const std::string & name, bool value)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform1i(location, (int)value);
}

void Shader::setUniform1i(const std::string & name, int value)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform1i(location, value);
}

void Shader::setUniform2i(const std::string & name, int v1, int v2)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform2i(location, v1, v2);
}

void Shader::setUniform2iv(const std::string & name, unsigned int count, const int * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform2iv(location, count, values);
}

void Shader::setUniform3i(const std::string & name, int v1, int v2, int v3)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform3i(location, v1, v2, v3);
}

void Shader::setUniform3iv(const std::string & name, unsigned int count, const int * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform3iv(location, count, values);
}

void Shader::setUniform4i(const std::string & name, int v1, int v2, int v3, int v4)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform4i(location, v1, v2, v3, v4);
}

void Shader::setUniform4iv(const std::string & name, unsigned int count, const int * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform4iv(location, count, values);
}

void Shader::setUniform1ui(const std::string & name, unsigned int value)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform1ui(location, value);
}

void Shader::setUniform2ui(const std::string & name, unsigned int v1, unsigned int v2)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform2ui(location, v1, v2);
}

void Shader::setUniform2uiv(const std::string & name, unsigned int count, const unsigned int * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform2uiv(location, count, values);
}

void Shader::setUniform3ui(const std::string & name, int v1, int v2, int v3)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform3ui(location, v1, v2, v3);
}

void Shader::setUniform3uiv(const std::string & name, unsigned int count, const unsigned int * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform3uiv(location, count, values);
}

void Shader::setUniform4ui(const std::string & name, int v1, int v2, int v3, int v4)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform4ui(location, v1, v2, v3, v4);
}

void Shader::setUniform4uiv(const std::string & name, unsigned int count, const unsigned int * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform4uiv(location, count, values);
}

void Shader::setUniform1f(const std::string & name, float f)
{
	GLint location = addUniform(name);
	if(location != -1) glUniform1f(location, f);
}

void Shader::setUniform2f(const std::string & name, float v1, float v2)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform2f(location, v1, v2);
}

void Shader::setUniform2fv(const std::string & name, unsigned int count, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform2fv(location, count, values);
}

void Shader::setUniform3f(const std::string & name, float v1, float v2, float v3)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform3f(location, v1, v2, v3);
}

void Shader::setUniform3fv(const std::string & name, unsigned int count, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform3fv(location, count, values);
}

void Shader::setUniform4f(const std::string & name, float v1, float v2, float v3, float v4)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform4f(location, v1, v2, v3, v4);
}

void Shader::setUniform4fv(const std::string & name, unsigned int count, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniform4fv(location, count, values);
}

void Shader::setUniformMatrix2fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix2fv(location, count, transpose, values);
}

void Shader::setUniformMatrix3fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix3fv(location, count, transpose, values);
}

void Shader::setUniformMatrix4fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix4fv(location, count, transpose, values);
}

void Shader::setUniformMatrix2x3fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix2x3fv(location, count, transpose, values);
}

void Shader::setUniformMatrix3x2fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix3x2fv(location, count, transpose, values);
}

void Shader::setUniformMatrix2x4fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix2x4fv(location, count, transpose, values);
}

void Shader::setUniformMatrix4x2fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix4x2fv(location, count, transpose, values);
}

void Shader::setUniformMatrix3x4fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix3x4fv(location, count, transpose, values);
}

void Shader::setUniformMatrix4x3fv(const std::string & name, unsigned int count, bool transpose, const float * values)
{
	GLint location = addUniform(name);
	if (location != -1) glUniformMatrix4x3fv(location, count, transpose, values);
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
