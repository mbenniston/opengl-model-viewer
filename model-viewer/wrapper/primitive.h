#pragma once
#include <optional>
#include <vector>

#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "material.h"

class Primitive
{
public:
    Primitive(VertexArray&& vertexArray, std::vector<VertexBuffer*> && vertexBuffers, GLenum mode, GLsizei count, Material* material);
    Primitive(IndexBuffer&& indexBuffer, VertexArray&& vertexArray, std::vector<VertexBuffer*>&& vertexBuffers, GLenum mode, GLsizei count, GLenum indexType, Material* material);

    void draw() const;

    void bind() const;
    void drawUnbound() const;
    void unbind() const;

    const Material* material() const { return m_material; }

private:
    VertexArray m_vertexArray;
    std::vector<VertexBuffer*> m_vertexBuffers;
    std::optional<IndexBuffer> m_indexBuffer;
    std::optional<GLenum> m_indexType;

    GLenum m_mode;
    GLsizei m_count;

    const Material* m_material;
};