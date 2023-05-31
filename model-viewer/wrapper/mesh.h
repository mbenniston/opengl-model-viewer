#pragma once

#include "primitive.h"

class Mesh
{
public:

	Mesh(std::vector<Primitive>&& primitives) : m_primitives(std::move(primitives))
	{
	}

	const std::vector<Primitive>& primitives() const
	{
		return m_primitives;
	}

private:
	std::vector<Primitive> m_primitives;
};