#include "gooch_renderer.h"
#include <array>
#include "../../constants.h"

GoochPass::GoochPass(int width, int height) :
    m_deferredShader(Shader::CreateFromPath(
        RESOURCES_PATH + "shaders/gooch/deferred_gooch.vert",
        RESOURCES_PATH + "shaders/gooch/deferred_gooch.frag")),
    m_lightingShader(Shader::CreateFromPath(
        RESOURCES_PATH + "shaders/gooch/shading_gooch.vert",
        RESOURCES_PATH + "shaders/gooch/shading_gooch.frag")),
    m_screenQuad(2.0f, 2.0f)
{
    m_shadingUniformLocations.projectionMatrix = m_deferredShader.getUniformLocation("u_projection");
    m_shadingUniformLocations.viewMatrix = m_deferredShader.getUniformLocation("u_view");
    m_shadingUniformLocations.modelMatrix = m_deferredShader.getUniformLocation("u_model");
    m_shadingUniformLocations.baseSampler = m_deferredShader.getUniformLocation("u_base");
    m_shadingUniformLocations.baseFactor = m_deferredShader.getUniformLocation("u_baseFactor");
    m_shadingUniformLocations.blue = m_deferredShader.getUniformLocation("u_blue");
    m_shadingUniformLocations.yellow = m_deferredShader.getUniformLocation("u_yellow");
    m_shadingUniformLocations.alpha = m_deferredShader.getUniformLocation("u_alpha");
    m_shadingUniformLocations.beta = m_deferredShader.getUniformLocation("u_beta");

    m_lightingUniformLocations.albedoSampler = m_lightingShader.getUniformLocation("u_albedo");
    m_lightingUniformLocations.geometrySampler = m_lightingShader.getUniformLocation("u_geometry");
    m_lightingUniformLocations.normalSampler = m_lightingShader.getUniformLocation("u_normal");
    m_lightingUniformLocations.temperatureSampler = m_lightingShader.getUniformLocation("u_temperature");
    m_lightingUniformLocations.lightDirection = m_lightingShader.getUniformLocation("u_lightDirection");
    m_lightingUniformLocations.scale = m_lightingShader.getUniformLocation("u_scale");
    m_lightingUniformLocations.depthThreshold = m_lightingShader.getUniformLocation("u_depthThreshold");
    m_lightingUniformLocations.normalThreshold = m_lightingShader.getUniformLocation("u_normalThreshold");

    m_lightingUniformLocations.depthNormalThreshold = m_lightingShader.getUniformLocation("u_depthNormalThreshold");
    m_lightingUniformLocations.depthNormalThresholdScale = m_lightingShader.getUniformLocation("u_depthNormalThresholdScale");
    glSamplerParameteri(m_bufferSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    createFramebuffer(width, height);
}

void GoochPass::render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Scene& scene, 
    glm::vec3 direction, float blue, float yellow, float alpha, float beta, int scale, float depthThreshold, float normalThreshold, float depthNormalThreshold,
    float depthNormalThresholdScale)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    m_deferredShader.bind();
    m_deferredShader.loadUniform(m_shadingUniformLocations.projectionMatrix, projectionMatrix);
    m_deferredShader.loadUniform(m_shadingUniformLocations.viewMatrix, viewMatrix);
    m_deferredShader.loadUniform(m_shadingUniformLocations.baseSampler, 0);

    m_deferredShader.loadUniform(m_shadingUniformLocations.blue, blue);
    m_deferredShader.loadUniform(m_shadingUniformLocations.yellow, yellow);
    m_deferredShader.loadUniform(m_shadingUniformLocations.alpha, alpha);
    m_deferredShader.loadUniform(m_shadingUniformLocations.beta, beta);

    for (const auto& node : scene.nodes())
    {
        renderNode(*node, glm::mat4(1.0f));
    }

    m_deferredShader.unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    m_lightingShader.bind();
    m_lightingShader.loadUniform(m_lightingUniformLocations.albedoSampler, 0);
    m_lightingShader.loadUniform(m_lightingUniformLocations.geometrySampler, 1);
    m_lightingShader.loadUniform(m_lightingUniformLocations.normalSampler, 2);
    m_lightingShader.loadUniform(m_lightingUniformLocations.temperatureSampler, 3);
    m_lightingShader.loadUniform(m_lightingUniformLocations.lightDirection, direction);
    m_lightingShader.loadUniform(m_lightingUniformLocations.scale, scale);
    m_lightingShader.loadUniform(m_lightingUniformLocations.depthThreshold, depthThreshold);
    m_lightingShader.loadUniform(m_lightingUniformLocations.normalThreshold, normalThreshold);

    m_lightingShader.loadUniform(m_lightingUniformLocations.depthNormalThreshold, depthNormalThreshold);
    m_lightingShader.loadUniform(m_lightingUniformLocations.depthNormalThresholdScale, depthNormalThresholdScale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_albedoBuffer);
    glBindSampler(0, m_bufferSampler);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_geometryBuffer);
    glBindSampler(1, m_bufferSampler);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_normalBuffer);
    glBindSampler(2, m_bufferSampler);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_temperatureBuffer);
    glBindSampler(3, m_bufferSampler);

    m_screenQuad.draw();
    m_lightingShader.unbind();

    glBindSampler(0, 0);
    glBindSampler(1, 0);
    glBindSampler(2, 0);
    glBindSampler(3, 0);
}

void GoochPass::renderNode(const Node& node, const glm::mat4& parent_transform)
{
    glm::mat4 transform = parent_transform * node.transform();

    for (auto child : node.children())
    {
        renderNode(*child, transform);
    }

    m_deferredShader.loadUniform(m_shadingUniformLocations.modelMatrix, transform);

    if (!node.mesh().has_value())
    {
        return;
    }

    for (const auto& prim : node.mesh().value()->primitives())
    {
        if (prim.material() != nullptr)
        {
            const Material& mat = *prim.material();
            if (mat.baseColorTexture().has_value())
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, *mat.baseColorTexture().value());
            }

            if (mat.baseColorTextureSampler().has_value())
            {
                glBindSampler(0, *mat.baseColorTextureSampler().value());
            }
            else
            {
                glBindSampler(0, 0);
            }

            m_deferredShader.loadUniform(m_shadingUniformLocations.baseFactor, mat.baseColorFactor());
        }

        prim.draw();
    }
}


void GoochPass::createFramebuffer(int width, int height)
{
    m_framebufferWidth = width;
    m_framebufferHeight = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glBindTexture(GL_TEXTURE_2D, m_albedoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, m_geometryBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, m_normalBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, m_temperatureBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_albedoBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_geometryBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_normalBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_temperatureBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

    std::array<GLenum, 4> drawBuffers = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
    };

    glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
