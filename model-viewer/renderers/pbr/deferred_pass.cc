#include "deferred_pass.h"
#include <array>
#include "../../constants.h"

DeferredPass::DeferredPass(int width, int height) :
    m_deferredShader(Shader::CreateFromPath(
        RESOURCES_PATH + "shaders/pbr/deferred.vert",
        RESOURCES_PATH + "shaders/pbr/deferred.frag")),
    m_skyboxShader(Shader::CreateFromPath(
        RESOURCES_PATH + "shaders/pbr/skybox.vert",
        RESOURCES_PATH + "shaders/pbr/skybox.frag"))
{
    m_uniformLocations.projectionMatrix = m_deferredShader.getUniformLocation("u_projection");
    m_uniformLocations.viewMatrix = m_deferredShader.getUniformLocation("u_view");
    m_uniformLocations.modelMatrix = m_deferredShader.getUniformLocation("u_model");
    m_uniformLocations.baseSampler = m_deferredShader.getUniformLocation("u_base");
    m_uniformLocations.roughMetalSampler = m_deferredShader.getUniformLocation("u_roughnessMetallic");
    m_uniformLocations.emissiveSampler = m_deferredShader.getUniformLocation("u_emissive");
    m_uniformLocations.baseFactor = m_deferredShader.getUniformLocation("u_baseFactor");
    m_uniformLocations.metalFactor = m_deferredShader.getUniformLocation("u_metalFactor");
    m_uniformLocations.roughFactor = m_deferredShader.getUniformLocation("u_roughFactor");
    m_uniformLocations.emissiveFactor = m_deferredShader.getUniformLocation("u_emissiveFactor");
    m_uniformLocations.occlusionSampler = m_deferredShader.getUniformLocation("u_occlusion");

    m_skyboxUniformLocations.mvpMatrix = m_skyboxShader.getUniformLocation("u_mvp");
    m_skyboxUniformLocations.diffuseSampler = m_skyboxShader.getUniformLocation("u_skyboxSampler");

    m_whiteTexture = TextureWrapper::CreateSinglePixel(glm::vec3(1.0f, 1.0f, 1.0f));
    m_blackTexture = TextureWrapper::CreateSinglePixel(glm::vec3(0.0f, 0.0f, 0.0f));

    createFramebuffer(width, height);
}

void DeferredPass::start(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
	m_deferredShader.bind();
    m_deferredShader.loadUniform(m_uniformLocations.projectionMatrix, projectionMatrix);
    m_deferredShader.loadUniform(m_uniformLocations.viewMatrix, viewMatrix);
    m_deferredShader.loadUniform(m_uniformLocations.baseSampler, 0);
    m_deferredShader.loadUniform(m_uniformLocations.roughMetalSampler, 1);
    m_deferredShader.loadUniform(m_uniformLocations.emissiveSampler, 2);
    m_deferredShader.loadUniform(m_uniformLocations.occlusionSampler, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void DeferredPass::render(const Scene& scene)
{
    for (const auto& node : scene.nodes())
    {
        renderNode(*node, glm::mat4(1.0f), m_deferredShader);
    }
}

void DeferredPass::drawSkybox(const Texture& diffuseCubemap, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuseCubemap);
    glBindSampler(0, 0);

    m_skyboxShader.bind();
    m_skyboxShader.loadUniform(m_skyboxUniformLocations.diffuseSampler, 0);
    m_skyboxShader.loadUniform(m_skyboxUniformLocations.mvpMatrix, projectionMatrix * glm::mat4(glm::mat3(viewMatrix)));
    m_cube.draw();
    m_skyboxShader.unbind();

    glDepthFunc(GL_LESS);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredPass::stop()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_deferredShader.unbind();
}

void DeferredPass::renderNode(const Node& node, const glm::mat4& parentTransform, Shader& shader)
{
    glm::mat4 transform = parentTransform * node.transform();

    for (auto child : node.children())
    {
        renderNode(*child, transform, shader);
    }

    shader.loadUniform(m_uniformLocations.modelMatrix, transform);

    if (!node.mesh().has_value()) {
        return;
    }

    for (const auto& prim : node.mesh().value()->primitives())
    {
        if (prim.material() != nullptr) {
            const Material& mat = *prim.material();
            
            glActiveTexture(GL_TEXTURE0);
            if (mat.baseColorTexture().has_value())
            {
                glBindTexture(GL_TEXTURE_2D, *mat.baseColorTexture().value());
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
            }

            glActiveTexture(GL_TEXTURE1);
            if (mat.metallicRoughnessTexture().has_value())
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, *mat.metallicRoughnessTexture().value());
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, m_blackTexture);
            }

            glActiveTexture(GL_TEXTURE2);
            if (mat.emissiveTexture().has_value())
            {
                glBindTexture(GL_TEXTURE_2D, *mat.emissiveTexture().value());
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
            }

            glActiveTexture(GL_TEXTURE3);
            if (mat.occulusionTexture().has_value())
            {
                glBindTexture(GL_TEXTURE_2D, *mat.occulusionTexture().value());
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, m_blackTexture);
            }

            if (mat.baseColorTextureSampler().has_value())
            {
                glBindSampler(0, *mat.baseColorTextureSampler().value());
            }
            else
            {
                glBindSampler(0, 0);
            }

            if (mat.metallicRoughnessTextureSampler().has_value())
            {
                glBindSampler(1, *mat.metallicRoughnessTextureSampler().value());
            }
            else
            {
                glBindSampler(1, 0);
            }

            if (mat.emissiveTextureSampler().has_value())
            {
                glBindSampler(2, *mat.emissiveTextureSampler().value());
            }
            else
            {
                glBindSampler(2, 0);
            }

            if (mat.occulusionTextureSampler().has_value())
            {
                glBindSampler(3, *mat.occulusionTextureSampler().value());
            }
            else
            {
                glBindSampler(3, 0);
            }

            shader.loadUniform(m_uniformLocations.baseFactor, mat.baseColorFactor());
            shader.loadUniform(m_uniformLocations.metalFactor, mat.metallicFactor());
            shader.loadUniform(m_uniformLocations.roughFactor, mat.roughnessFactor());
            shader.loadUniform(m_uniformLocations.emissiveFactor, mat.emissiveFactor());
        }

        prim.draw();
    }
}

void DeferredPass::createFramebuffer(int width, int height)
{
    m_framebufferWidth = width;
    m_framebufferHeight = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    
    glBindTexture(GL_TEXTURE_2D, m_albedoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, m_geometryBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, m_normalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, m_roughMetalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, m_emissiveBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_albedoBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_geometryBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_normalBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_roughMetalBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_emissiveBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

    std::array<GLenum,5> drawBuffers = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
    };

    glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
