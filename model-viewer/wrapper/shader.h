#pragma once
#include <string>
#include <glad.h>
#include <glm_nowarn.h>

class Shader
{
public:
	static Shader CreateFromPath(const std::string& vertexPath, const std::string& fragmentPath);
	static Shader CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource);

	static Shader CreateFromPath(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath);
	static Shader CreateFromSource(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource);
	~Shader();

	GLint getUniformLocation(const std::string& uniform_name) const;

	void loadUniform(GLint location, int value);
	void loadUniform(GLint location, float value);
	void loadUniform(GLint location, glm::vec3 vector);
	void loadUniform(GLint location, glm::vec4 vector);
	void loadUniform(GLint location, const glm::mat3& matrix);
	void loadUniform(GLint location, const glm::mat4& matrix);

	void bind();
	void unbind();
	
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

private:
	Shader(GLuint vertexShader, GLuint fragmentShader);
	Shader(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader);
	
	void release();

	GLuint m_program;
};