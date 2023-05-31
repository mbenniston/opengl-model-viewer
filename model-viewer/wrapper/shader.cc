#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

static std::string read_all_from_file(const std::string& path)
{
	std::fstream stream(path);

	if (!stream)
	{
		throw std::exception("File not found");
	}

	std::stringstream ss;
	ss << stream.rdbuf();

	return ss.str();
}

static GLuint create_shader_from_source(const char* source, GLenum type)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint result = GL_FALSE;
	GLint logLength;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> shaderError(static_cast<std::size_t>(logLength));
		glGetShaderInfoLog(shader, logLength, NULL, &shaderError[0]);

		glDeleteShader(shader);

		std::cout << std::string(shaderError.begin(), shaderError.end()) << std::endl;
		throw std::exception();
	}

	return shader;
}

Shader Shader::CreateFromPath(const std::string& vertex_path, const std::string& fragment_path)
{
	std::string vertex_source = read_all_from_file(vertex_path);
	std::string fragment_source = read_all_from_file(fragment_path);

	return Shader::CreateFromSource(vertex_source, fragment_source);
}

Shader Shader::CreateFromSource(const std::string& vertex_source, const std::string& fragment_source)
{
	GLuint vertex_shader = create_shader_from_source(vertex_source.c_str(), GL_VERTEX_SHADER);
	GLuint fragment_shader = create_shader_from_source(fragment_source.c_str(), GL_FRAGMENT_SHADER);

	Shader shader = Shader(vertex_shader, fragment_shader);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader;
}

Shader Shader::CreateFromPath(const std::string& vertex_path, const std::string& geometry_path, const std::string& fragment_path)
{
	std::string vertex_source = read_all_from_file(vertex_path);
	std::string geometry_source = read_all_from_file(geometry_path);
	std::string fragment_source = read_all_from_file(fragment_path);

	return Shader::CreateFromSource(vertex_source, geometry_source, fragment_source);
}

Shader Shader::CreateFromSource(const std::string& vertex_source, const std::string& geometry_source, const std::string& fragment_source)
{
	GLuint vertex_shader = create_shader_from_source(vertex_source.c_str(), GL_VERTEX_SHADER);
	GLuint geometry_shader = create_shader_from_source(geometry_source.c_str(), GL_GEOMETRY_SHADER);
	GLuint fragment_shader = create_shader_from_source(fragment_source.c_str(), GL_FRAGMENT_SHADER);

	Shader shader = Shader(vertex_shader, geometry_shader, fragment_shader);

	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);

	return shader;
}

Shader::~Shader()
{
	release();
}

GLint Shader::getUniformLocation(const std::string& uniform_name) const
{
	return glGetUniformLocation(m_program, uniform_name.c_str());
}

void Shader::loadUniform(GLint location, int value)
{
	glUniform1i(location, value);
}

void Shader::loadUniform(GLint location, float value)
{
	glUniform1f(location, value);
}

void Shader::loadUniform(GLint location, glm::vec3 vector)
{
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::loadUniform(GLint location, glm::vec4 vector)
{
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::loadUniform(GLint location, const glm::mat3& matrix)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::loadUniform(GLint location, const glm::mat4& matrix)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::bind()
{
	glUseProgram(m_program);
}

void Shader::unbind()
{
	glUseProgram(0);
}

Shader::Shader(Shader&& other) noexcept
{
	m_program = 0;
	std::swap(m_program, other.m_program);
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	release();
	std::swap(m_program, other.m_program);

	return *this;
}

Shader::Shader(GLuint vertex_shader, GLuint fragment_shader)
{
	m_program = glCreateProgram();
	glAttachShader(m_program, vertex_shader);
	glAttachShader(m_program, fragment_shader);
	
	glLinkProgram(m_program);
	GLint link_status;
	glGetProgramiv(m_program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE)
	{
		GLint logLength; 
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> shaderError(static_cast<std::size_t>(logLength));
		glGetProgramInfoLog(m_program, logLength, NULL, &shaderError[0]);

		std::cout << std::string(shaderError.begin(), shaderError.end()) << std::endl;
		throw std::exception();
	}

	glValidateProgram(m_program);
	GLint validate_status;
	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &validate_status);
	if (validate_status == GL_FALSE)
	{
		GLint logLength;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> shaderError(static_cast<std::size_t>(logLength));
		glGetProgramInfoLog(m_program, logLength, NULL, &shaderError[0]);

		std::cout << std::string(shaderError.begin(), shaderError.end()) << std::endl;
		throw std::exception();
	}	
}

Shader::Shader(GLuint vertex_shader, GLuint geometry_shader, GLuint fragment_shader)
{
	m_program = glCreateProgram();
	glAttachShader(m_program, vertex_shader);
	glAttachShader(m_program, geometry_shader);
	glAttachShader(m_program, fragment_shader);

	glLinkProgram(m_program);
	GLint link_status;
	glGetProgramiv(m_program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE)
	{
		GLint logLength;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> shaderError(static_cast<std::size_t>(logLength));
		glGetProgramInfoLog(m_program, logLength, NULL, &shaderError[0]);

		std::cout << std::string(shaderError.begin(), shaderError.end()) << std::endl;
		throw std::exception();
	}

	glValidateProgram(m_program);
	GLint validate_status;
	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &validate_status);
	if (validate_status == GL_FALSE)
	{
		GLint logLength;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> shaderError(static_cast<std::size_t>(logLength));
		glGetProgramInfoLog(m_program, logLength, NULL, &shaderError[0]);

		std::cout << std::string(shaderError.begin(), shaderError.end()) << std::endl;
		throw std::exception();
	}
}

void Shader::release()
{
	if (m_program != 0)
	{
		glDeleteProgram(m_program);
		m_program = 0;
	}
}
