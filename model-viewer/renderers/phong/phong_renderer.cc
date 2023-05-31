#include "phong_renderer.h"
#include "../../constants.h"

PhongRenderer::PhongRenderer(SceneSettings& scene_settings) : 
    m_sceneSettings(scene_settings),
    m_phongPass(scene_settings.width, scene_settings.height),
    m_panel(m_settings)
{
    IblBaker iblBaker;
    m_settings.skyboxSource = TextureWrapper::LoadHDRFromPath(RESOURCES_PATH + "environments/kloppenheim_07_puresky_4k.hdr");
    m_settings.skyboxCubemap = iblBaker.convertEquirectangularToCubemap(m_settings.skyboxSource.value(), m_settings.skyboxResolution);
}

void PhongRenderer::onRender(const glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
{
    glm::vec3 lightDirectionViewSpace = glm::vec3(viewMatrix * glm::vec4(m_sceneSettings.lightDirection, 0.0f));

    if (m_sceneSettings.fileResource.has_value())
    {
        GltfFileResource& fileResource = m_sceneSettings.fileResource.value();

        m_phongPass.render(projectionMatrix, viewMatrix,
            *fileResource.defaultScene(),
            lightDirectionViewSpace, m_sceneSettings.lightColor,
            m_settings.ambient, m_settings.specularPower, m_settings.specularStrength);
    }

    if (m_settings.skyboxCubemap.has_value())
    {
        m_phongPass.drawSkybox(m_settings.skyboxCubemap.value(), projectionMatrix, viewMatrix);
    }
}

void PhongRenderer::onGuiDraw()
{
    m_panel.onDraw();
}

void PhongRenderer::onWindowResize(int newWidth, int newHeight)
{
    m_phongPass.createFramebuffer(newWidth, newHeight);
}
