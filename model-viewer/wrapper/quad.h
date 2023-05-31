#pragma once 
#include <glad.h>

enum class QuadOrigin
{
	CenteredAtOrigin,
	CenteredAtTopLeft
};

class Quad
{
public:
	Quad(float width, float height, QuadOrigin origin = QuadOrigin::CenteredAtOrigin);
	~Quad();

	void draw();
	void bind();
	void drawUnbound();
	void unbind();

	Quad(const Quad&) = delete;
	Quad& operator=(const Quad&) = delete;

	Quad(Quad&&) noexcept;
	Quad& operator=(Quad&&) noexcept;


private:

	void release();
	
	GLuint m_vertexArray;
	GLuint m_vertexBuffer;
};