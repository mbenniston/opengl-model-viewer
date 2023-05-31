#pragma once
#include "node.h"

class Scene
{
public:
	const std::vector<const Node*>& nodes() const
	{
		return m_nodes;
	}

	Scene(const std::vector<const Node*>& nodes) : m_nodes(nodes)
	{
	}

private:
	std::vector<const Node*> m_nodes;
};