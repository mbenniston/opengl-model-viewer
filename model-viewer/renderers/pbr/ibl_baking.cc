#include "ibl_baking.h"
#include <array>

#include "../../wrapper/render_buffer.h"
#include "../../constants.h"

static const glm::mat4 PROJECTION_MATRIX = glm::perspectiveFov<float>(glm::radians(90.0f), 1.0f, 1.0f, 0.1f, 10.0f);

static const std::array<glm::mat4, 6> VIEW_MATRICES {
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

IblBaker::IblBaker() : 
	m_conversionShader(Shader::CreateFromPath(
		RESOURCES_PATH + "shaders/pbr/ibl/cubemap.vert",
		RESOURCES_PATH + "shaders/pbr/ibl/equirectangular.frag")),
	m_convolutionShader(Shader::CreateFromPath(
		RESOURCES_PATH + "shaders/pbr/ibl/cubemap.vert",
		RESOURCES_PATH + "shaders/pbr/ibl/convolution.frag")),
	m_prefilteringShader(Shader::CreateFromPath(
		RESOURCES_PATH + "shaders/pbr/ibl/cubemap.vert",
		RESOURCES_PATH + "shaders/pbr/ibl/prefiltering.frag")),
	m_brdfBakingShader(Shader::CreateFromPath(
		RESOURCES_PATH + "shaders/pbr/ibl/bake_brdf.vert",
		RESOURCES_PATH + "shaders/pbr/ibl/bake_brdf.frag")),
	m_cube(),
	m_quad(2.0f, 2.0f)
{
	m_conversionUniforms.projection = m_conversionShader.getUniformLocation("u_projection");
	m_conversionUniforms.view = m_conversionShader.getUniformLocation("u_view");
	m_conversionUniforms.equirectangular = m_conversionShader.getUniformLocation("u_equirectangularMap");

	m_convolutionUniforms.projection = m_convolutionShader.getUniformLocation("u_projection");
	m_convolutionUniforms.view = m_convolutionShader.getUniformLocation("u_view");
	m_convolutionUniforms.environmentMap = m_convolutionShader.getUniformLocation("u_environmentMap");

	m_prefilteringUniforms.projection = m_prefilteringShader.getUniformLocation("u_projection");
	m_prefilteringUniforms.view = m_prefilteringShader.getUniformLocation("u_view");
	m_prefilteringUniforms.environmentMap = m_prefilteringShader.getUniformLocation("u_environmentMap");
	m_prefilteringUniforms.roughness = m_prefilteringShader.getUniformLocation("u_roughness");
}

Texture IblBaker::generateBrdfLut(int resolution)
{
	// create empty lut
	Texture lut;
	glBindTexture(GL_TEXTURE_2D, lut);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	// create framebuffer
	Framebuffer framebuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lut, 0);

	RenderBuffer depthbuffer;
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution, resolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	m_brdfBakingShader.bind();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, resolution, resolution);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_quad.draw();
	m_brdfBakingShader.unbind();

	return lut;
}

Texture IblBaker::convertEquirectangularToCubemap(const Texture& equirectangular, int cubemapResolution)
{
	// create empty cubemap
	Texture cubemap;
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			cubemapResolution, cubemapResolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// create framebuffer
	Framebuffer framebuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	RenderBuffer depthbuffer;
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cubemapResolution, cubemapResolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	// render each cubemap side with a different view matrix 
	m_conversionShader.bind();
	m_conversionShader.loadUniform(m_conversionUniforms.projection, PROJECTION_MATRIX);
	m_conversionShader.loadUniform(m_conversionUniforms.equirectangular, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equirectangular);

	m_cube.bind();
	glViewport(0, 0, cubemapResolution, cubemapResolution);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	for (int side = 0; side < 6; side++)
	{
		m_conversionShader.loadUniform(m_conversionUniforms.view, VIEW_MATRICES[side]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, cubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cube.drawUnbound();
	}

	m_cube.unbind();
	m_conversionShader.unbind();

	glBindTexture(GL_TEXTURE_2D, 0);

	return cubemap;
}

Texture IblBaker::convoluteCubemap(const Texture& inputCubemap, int cubemapResolution)
{
	// create empty cubemap
	Texture cubemap;
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			cubemapResolution, cubemapResolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// create framebuffer
	Framebuffer framebuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	RenderBuffer depthbuffer;
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cubemapResolution, cubemapResolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	// convolove each side of input cubemap 
	m_convolutionShader.bind();
	m_convolutionShader.loadUniform(m_convolutionUniforms.projection, PROJECTION_MATRIX);
	m_convolutionShader.loadUniform(m_convolutionUniforms.environmentMap, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, inputCubemap);

	m_cube.bind();
	glViewport(0, 0, cubemapResolution, cubemapResolution);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < 6; i++)
	{
		m_convolutionShader.loadUniform(m_convolutionUniforms.view, VIEW_MATRICES[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cube.drawUnbound();
	}
	m_cube.unbind();
	m_convolutionShader.unbind();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return cubemap;
}

Texture IblBaker::prefilterCubemap(const Texture& inputCubemap, int maxCubemapResolution, int levels)
{
	// create empty cube map
	Texture cubemap;
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			maxCubemapResolution, maxCubemapResolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// create framebuffer
	Framebuffer framebuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	RenderBuffer depthbuffer;
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, maxCubemapResolution, maxCubemapResolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	// filter each cubemap side for each mip level
	m_prefilteringShader.bind();
	m_prefilteringShader.loadUniform(m_prefilteringUniforms.projection, PROJECTION_MATRIX);
	m_prefilteringShader.loadUniform(m_prefilteringUniforms.environmentMap, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, inputCubemap);

	m_cube.bind();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_CULL_FACE);

	for (int mipLevel = 0; mipLevel < levels; mipLevel++)
	{
		int mip_width = static_cast<int>(maxCubemapResolution * std::pow(0.5f, mipLevel));
		int mip_height = static_cast<int>(maxCubemapResolution * std::pow(0.5f, mipLevel));

		glViewport(0, 0, mip_width, mip_height);
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mip_width, mip_height);

		float roughness = mipLevel / static_cast<float>(levels - 1);
		m_prefilteringShader.loadUniform(m_prefilteringUniforms.roughness, roughness);

		for (int side = 0; side < 6; side++)
		{
			m_prefilteringShader.loadUniform(m_prefilteringUniforms.view, VIEW_MATRICES[side]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, cubemap, mipLevel);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_cube.drawUnbound();
		}
	}

	m_cube.unbind();
	m_prefilteringShader.unbind();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return cubemap;
}