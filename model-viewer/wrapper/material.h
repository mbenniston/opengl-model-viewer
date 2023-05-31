#pragma once
#include <memory>
#include <glm_nowarn.h>
#include "texture.h"
#include "sampler.h"

class Material
{
public:
	std::optional<const Texture*> baseColorTexture() const { return m_baseColorTexture; }
	std::optional<const Sampler*> baseColorTextureSampler() const { return m_baseColorTextureSampler; }
	std::optional<const Texture*> metallicRoughnessTexture() const { return m_metallicRoughnessTexture; }
	std::optional<const Sampler*> metallicRoughnessTextureSampler() const { return m_metallicRoughnessTextureSampler; }
	std::optional<const Texture*> emissiveTexture() const { return m_emissiveTexture; }
	std::optional<const Sampler*> emissiveTextureSampler() const { return m_emissiveTextureSampler; }
	std::optional<const Texture*> occulusionTexture() const { return m_occulusionTexture; }
	std::optional<const Sampler*> occulusionTextureSampler() const { return m_occulusionTextureSampler; }

	glm::vec4 baseColorFactor() const { return m_baseColorFactor; }
	float metallicFactor() const { return m_metallicFactor; }
	float roughnessFactor() const { return m_roughnessFactor; }
	glm::vec3 emissiveFactor() const { return m_emssiveFactor; }

	Material()
	{
		m_baseColorFactor = glm::vec4(1.0f);
		m_metallicFactor = 1.0f;
		m_roughnessFactor = 1.0f;
		m_emssiveFactor = glm::vec3(0.0f);
	}

	Material(
		glm::vec4 baseColorFactor,
		std::optional<const Texture*> baseColorTexture,
		std::optional<const Sampler*> baseColorTextureSampler,
		float metallicFactor,
		float roughnessFactor,
		glm::vec3 emissiveFactor,
		std::optional<const Texture*> metallicRoughnessTexture,
		std::optional<const Sampler*> metallicRoughnessTextureSampler,
		std::optional<const Texture*> emissiveTexture,
		std::optional<const Sampler*> emissiveTextureSampler,
		std::optional<const Texture*> occulusionTexture,
		std::optional<const Sampler*> occulusionTextureSampler

	) :
		m_baseColorFactor(baseColorFactor),
		m_baseColorTexture(baseColorTexture),
		m_baseColorTextureSampler(baseColorTextureSampler),
		m_metallicFactor(metallicFactor),
		m_roughnessFactor(roughnessFactor),
		m_metallicRoughnessTexture(metallicRoughnessTexture),
		m_metallicRoughnessTextureSampler(metallicRoughnessTextureSampler),
		m_emissiveTexture(emissiveTexture),
		m_emssiveFactor(emissiveFactor),
		m_emissiveTextureSampler(emissiveTextureSampler),
		m_occulusionTexture(occulusionTexture),
		m_occulusionTextureSampler(occulusionTextureSampler)
	{
	}

private:
	glm::vec4 m_baseColorFactor;
	std::optional<const Texture*> m_baseColorTexture;
	std::optional<const Sampler*> m_baseColorTextureSampler;
	float m_metallicFactor;
	float m_roughnessFactor;
	glm::vec3 m_emssiveFactor;
	std::optional<const Texture*> m_metallicRoughnessTexture;
	std::optional<const Sampler*> m_metallicRoughnessTextureSampler;
	std::optional<const Texture*> m_emissiveTexture;
	std::optional<const Sampler*> m_emissiveTextureSampler;
	std::optional<const Texture*> m_occulusionTexture;
	std::optional<const Sampler*> m_occulusionTextureSampler;
};