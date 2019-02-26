#pragma once

#include "GL/glew.h"
#include <vector>
#include <unordered_map>
#include <string>

class Texture;
class Shader
{
public:
	Shader();
	Shader(const std::string& vertex, const std::string& fragment);
	virtual ~Shader();

	void recreate(const std::string& vertex, const std::string& fragment);

	virtual void bind();

	void unbind();

	const GLuint getID() const;

	/*
	Add a uniform for Texture.
	Arguments:
		name: Name of texture in shader.
		unit: The texture unit which will be used.
		textureID: The texture which will be used.
	*/
	void setTexture2D(const std::string& name, unsigned int unit, const Texture& texture);

	/*
	Add a uniform for texture2D.
	Arguments:
		name: Name of texture in shader.
		unit: The texture unit which will be used.
		textureID: The texture which will be used.
	*/
	void setTexture2D(const std::string& name, unsigned int unit, GLuint textureID);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform1b(const std::string& name, bool value);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform1i(const std::string& name, int value);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform2i(const std::string& name, int v1, int v2);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform2iv(const std::string& name, unsigned int count, const int* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform3i(const std::string& name, int v1, int v2, int v3);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform3iv(const std::string& name, unsigned int count, const int* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform4i(const std::string& name, int v1, int v2, int v3, int v4);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform4iv(const std::string& name, unsigned int count, const int* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform1ui(const std::string& name, unsigned int value);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform2ui(const std::string& name, unsigned int v1, unsigned int v2);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform2uiv(const std::string& name, unsigned int count, const unsigned int* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform3ui(const std::string& name, int v1, int v2, int v3);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform3uiv(const std::string& name, unsigned int count, const unsigned int* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform4ui(const std::string& name, int v1, int v2, int v3, int v4);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform4uiv(const std::string& name, unsigned int count, const unsigned int* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform1f(const std::string& name, float f);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform2f(const std::string& name, float v1, float v2);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform2fv(const std::string& name, unsigned int count, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform3f(const std::string& name, float v1, float v2, float v3);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform3fv(const std::string& name, unsigned int count, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform4f(const std::string& name, float v1, float v2, float v3, float v4);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniform4fv(const std::string& name, unsigned int count, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix2fv(const std::string& name, unsigned int count, bool transpose, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix3fv(const std::string& name, unsigned int count, bool transpose, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix4fv(const std::string& name, unsigned int count, bool transpose, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix2x3fv(const std::string& name, unsigned int count, bool transpose, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix3x2fv(const std::string& name, unsigned int count, bool transpose, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix2x4fv(const std::string& name, unsigned int count, bool transpose, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix4x2fv(const std::string& name, unsigned int count, bool transpose, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix3x4fv(const std::string& name, unsigned int count, bool transpose, const float* values);

	/*
	Add uniform if not found, then set it to the new value.
	Arguments:
		name: Name of the uniform in the shader.
	*/
	void setUniformMatrix4x3fv(const std::string& name, unsigned int count, bool transpose, const float* values);

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