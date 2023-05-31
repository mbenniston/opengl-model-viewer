#include "quad.h"
#include <utility>

Quad::Quad(float width, float height, QuadOrigin origin)
{
    const bool isCenteredAtOrigin = origin == QuadOrigin::CenteredAtOrigin;
    const float halfWidth = width/2.0f;
    const float halfHeight = height/2.0f;

    float minWidth = isCenteredAtOrigin ? -halfWidth : 0;
    float minHeight = isCenteredAtOrigin ? -halfHeight : 0;
    float maxWidth = isCenteredAtOrigin ? halfWidth : width;
    float maxHeight = isCenteredAtOrigin ? halfHeight : height;

    const float vertices[] = {
        minWidth,
        minHeight,
        maxWidth,
        minHeight,
        maxWidth,
        maxHeight,
        maxWidth,
        maxHeight,
        minWidth,
        maxHeight,
        minWidth,
        minHeight,
    };

	glGenVertexArrays(1, &m_vertexArray);
	glBindVertexArray(m_vertexArray);
		
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

Quad::~Quad()
{
    release();
}

void Quad::draw()
{
    bind();
    drawUnbound();
    unbind();
}

void Quad::bind()
{
    glBindVertexArray(m_vertexArray);
}

void Quad::drawUnbound()
{
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Quad::unbind()
{
    glBindVertexArray(0);
}

Quad::Quad(Quad&& other) noexcept
{
    m_vertexArray = 0;
    m_vertexBuffer = 0;
    std::swap(m_vertexArray, other.m_vertexArray);
    std::swap(m_vertexBuffer, other.m_vertexBuffer);
}

Quad& Quad::operator=(Quad&& other) noexcept
{
    release();
    std::swap(m_vertexArray, other.m_vertexArray);
    std::swap(m_vertexBuffer, other.m_vertexBuffer);
    return *this;
}

void Quad::release()
{
    if (m_vertexArray != 0)
    {
        glDeleteVertexArrays(1, &m_vertexArray);
        m_vertexArray = 0;
    }

    if (m_vertexBuffer != 0)
    {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
    }
}
