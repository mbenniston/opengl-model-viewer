#include "primitive.h"


Primitive::Primitive(VertexArray&& vertexArray, std::vector<VertexBuffer*>&& vertexBuffers, GLenum mode, GLsizei count, Material* material) :
    m_vertexArray(std::move(vertexArray)),
    m_vertexBuffers(std::move(vertexBuffers)),
    m_mode(mode),
    m_count(count),
    m_material(material)
{
}

Primitive::Primitive(IndexBuffer&& indexBuffer, VertexArray&& vertexArray, std::vector<VertexBuffer*>&& vertexBuffers, GLenum mode, GLsizei count, GLenum indexType, Material* material) :
    m_vertexArray(std::move(vertexArray)),
    m_vertexBuffers(std::move(vertexBuffers)),
    m_indexBuffer(std::move(indexBuffer)),
    m_mode(mode),
    m_count(count),
    m_indexType(indexType),
    m_material(material)
{
}

void Primitive::draw() const
{
    bind();
    drawUnbound();
    unbind();
}

void Primitive::drawUnbound() const
{
    if (m_indexBuffer.has_value())
    {
        glDrawElements(m_mode, m_count, m_indexType.value(), nullptr);
    }
    else
    {
        glDrawArrays(m_mode, 0, m_count);
    }
}

void Primitive::bind() const
{
    glBindVertexArray(m_vertexArray);
    if (m_indexBuffer.has_value())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.value());
    }
}

void Primitive::unbind() const
{
    if (m_indexBuffer.has_value())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
}