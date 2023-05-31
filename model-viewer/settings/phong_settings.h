#pragma once
#include <glm_nowarn.h>
#include "../wrapper/texture.h"

struct PhongSettings
{
	glm::vec3 ambient = glm::vec3(0.1f);
	float specularPower = 64.0f;
	float specularStrength = 1.0f;
	std::optional<Texture> skyboxSource;
	std::optional<Texture> skyboxCubemap;
	int skyboxResolution = 512;
};