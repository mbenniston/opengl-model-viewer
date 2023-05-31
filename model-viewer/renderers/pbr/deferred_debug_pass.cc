#include "deferred_debug_pass.h"
#include <array>
#include "../../constants.h"

DeferredDebugPass::DeferredDebugPass() :
    m_debugShader(Shader::CreateFromPath(
        RESOURCES_PATH + "shaders/pbr/deferred_debug.vert",
        RESOURCES_PATH + "shaders/pbr/deferred_debug.geom",
        RESOURCES_PATH + "shaders/pbr/deferred_debug.frag"))
{
    m_debugUniformLocations.projectionMatrix = m_debugShader.getUniformLocation("u_projection");
    m_debugUniformLocations.viewMatrix = m_debugShader.getUniformLocation("u_view");
    m_debugUniformLocations.modelMatrix = m_debugShader.getUniformLocation("u_model");
}

void DeferredDebugPass::start_debug(DeferredPass& deferredRender, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
    m_debugShader.bind();
    m_debugShader.loadUniform(m_debugUniformLocations.projectionMatrix, projectionMatrix);
    m_debugShader.loadUniform(m_debugUniformLocations.viewMatrix, viewMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, deferredRender.framebuffer());
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void DeferredDebugPass::render_debug(const Scene& scene)
{
    for (const Node* node : scene.nodes())
    {
        render_node_debug(*node, glm::mat4(1.0f));
    }
}

void DeferredDebugPass::stop_debug()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_debugShader.unbind();
}

void DeferredDebugPass::render_node_debug(const Node& node, const glm::mat4& parentTransform)
{
    glm::mat4 transform = parentTransform * node.transform();

    for (auto child : node.children())
    {
        render_node_debug(*child, transform);
    }

    m_debugShader.loadUniform(m_debugUniformLocations.modelMatrix, transform);

    if (!node.mesh().has_value())
    {
        return;
    }

    for (const Primitive& prim : node.mesh().value()->primitives())
    {
        prim.draw();
    }
}

