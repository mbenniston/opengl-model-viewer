#include "gooch_renderer.h"

GoochRenderer::GoochRenderer(SceneSettings& sceneSettings) : 
	m_sceneSettings(sceneSettings),
    m_goochPass(sceneSettings.width, sceneSettings.height),
    m_goochPanel(m_goochSettings)
{
}

void GoochRenderer::onRender(const glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
{
    if (m_sceneSettings.fileResource.has_value())
    {
        m_goochPass.render(projectionMatrix, viewMatrix,
            *m_sceneSettings.fileResource.value().defaultScene(),
            glm::vec3(viewMatrix * glm::vec4(m_sceneSettings.lightDirection, 0.0f)),
            m_goochSettings.blue, m_goochSettings.yellow,
            m_goochSettings.alpha, m_goochSettings.beta, m_goochSettings.scale,
            m_goochSettings.depthThreshold, m_goochSettings.normalThreshold,
            m_goochSettings.depthNormalThreshold, m_goochSettings.depthNormalThresholdScale);
    }
}

void GoochRenderer::onGuiDraw()
{
    m_goochPanel.onDraw();
}

void GoochRenderer::onWindowResize(int newWidth, int newHeight)
{
    m_goochPass.createFramebuffer(newWidth, newHeight);
}
