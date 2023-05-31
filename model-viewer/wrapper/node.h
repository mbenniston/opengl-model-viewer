#pragma once
#include <memory>
#include <glm_nowarn.h>
#include "mesh.h"

class Node
{
public:
	Node(glm::mat4 transform, std::optional<const Mesh*> mesh, std::vector<const Node*> children) :
		m_transform(transform),
		m_mesh(mesh),
		m_children(children)
	{
	}

	const glm::mat4& transform() const { return m_transform; }
	std::optional<const Mesh*> mesh() const { return m_mesh; }
	std::vector<const Node*> children() const { return m_children;}

private:
	glm::mat4 m_transform;
	std::optional<const Mesh*> m_mesh;
	std::vector<const Node*> m_children;
};