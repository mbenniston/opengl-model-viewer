#pragma once
#include "../../wrapper/shader.h"
#include "../../wrapper/framebuffer.h"
#include "../../wrapper/cube.h"
#include "../../wrapper/quad.h"
#include "../../wrapper/texture.h"

class IblBaker
{
public:
	IblBaker();

	Texture generateBrdfLut(int resolution);
	Texture convertEquirectangularToCubemap(const Texture& equirectangular, int cubemapResolution);
	Texture convoluteCubemap(const Texture& inputCubemap, int cubemapResolution);
	Texture prefilterCubemap(const Texture& inputCubemap, int maxCubemapResolution, int levels);

private:
	Shader m_conversionShader;
	Shader m_convolutionShader;
	Shader m_prefilteringShader;
	Shader m_brdfBakingShader;

	struct {
		GLint projection;
		GLint view;
		GLint equirectangular;
	} m_conversionUniforms;

	struct {
		GLint projection;
		GLint view;
		GLint environmentMap;
	} m_convolutionUniforms;

	struct {
		GLint projection;
		GLint view;
		GLint environmentMap;
		GLint roughness;
	} m_prefilteringUniforms;

	Cube m_cube;
	Quad m_quad;
};
