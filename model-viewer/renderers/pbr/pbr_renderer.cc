#include "pbr_renderer.h"
#include <imgui.h>
#include "../../constants.h"

PbrRenderer::PbrRenderer(SceneSettings& sceneSettings) : 
	m_sceneSettings(sceneSettings),
    m_bloomPanel(m_bloomSettings),
    m_ssaoPanel(m_ssaoSettings),
    m_iblPanel(m_iblSettings, m_iblBaker),
    m_deferredPass(sceneSettings.width, sceneSettings.height),
    m_pbrPass(sceneSettings.width, sceneSettings.height),
    m_ssaoPass(sceneSettings.width, sceneSettings.height),
    m_ssaoBlurPass(sceneSettings.width, sceneSettings.height, 4, true),
    m_bloomBlurPass(sceneSettings.width, sceneSettings.height, 8),
    m_black(TextureWrapper::CreateSinglePixel(glm::vec3(0.0f)))
{
    m_framebufferWidth = sceneSettings.width;
    m_framebufferHeight = sceneSettings.height;

    m_iblSettings.equirectangularEnvironment = TextureWrapper::LoadHDRFromPath(RESOURCES_PATH + "environments/kloppenheim_07_puresky_4k.hdr");
    m_iblSettings.diffuseCubemap = m_iblBaker.convertEquirectangularToCubemap(m_iblSettings.equirectangularEnvironment, m_iblSettings.sourceCubemapSize);
    m_iblSettings.irrandianceCubemap = m_iblBaker.convoluteCubemap(m_iblSettings.diffuseCubemap, m_iblSettings.irrandianceCubemapSize);
    m_iblSettings.filteredCubemap = m_iblBaker.prefilterCubemap(m_iblSettings.diffuseCubemap, m_iblSettings.filteredCubemapSize, 5);
    m_iblSettings.brdfLut = m_iblBaker.generateBrdfLut(m_iblSettings.brdfLutSize);
}

void PbrRenderer::onRender(const glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
{
    m_deferredPass.start(projectionMatrix, viewMatrix);
    if (m_sceneSettings.fileResource.has_value())
    {
        m_deferredPass.render(*m_sceneSettings.fileResource.value().defaultScene());
    }
    m_deferredPass.stop();

    m_deferredPass.drawSkybox(m_iblSettings.diffuseCubemap, projectionMatrix, viewMatrix);

    if (m_sceneSettings.debugNormals)
    {
        m_deferredDebugPass.start_debug(m_deferredPass, projectionMatrix, viewMatrix);
        if (m_sceneSettings.fileResource.has_value())
        {
            m_deferredDebugPass.render_debug(*m_sceneSettings.fileResource.value().defaultScene());
        }
        m_deferredDebugPass.stop_debug();
    }

    // shading pass
    m_ssaoPass.render(m_deferredPass.geometry(), m_deferredPass.normal(), projectionMatrix, m_ssaoSettings.samples, m_ssaoSettings.distance, m_ssaoSettings.bias);
    m_ssaoBlurPass.blur(m_ssaoPass.occlusion(), m_ssaoSettings.blurKernel, 0);
    
    m_pbrPass.render(m_deferredPass,
        m_ssaoSettings.enabled ? m_ssaoBlurPass.blurred() : m_black,
        m_iblSettings.irrandianceCubemap, m_iblSettings.filteredCubemap, m_iblSettings.brdfLut,
        glm::vec3(viewMatrix * glm::vec4(m_sceneSettings.lightDirection, 0.0f)), m_sceneSettings.lightColor, viewMatrix);

    m_bloomBlurPass.blur(m_pbrPass.color(), m_bloomSettings.blurKernel, m_bloomSettings.stdDev);

    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
    m_bloomPass.render(m_pbrPass.color(), m_bloomBlurPass.blurred(), m_bloomSettings.lensDirt.has_value() ? m_bloomSettings.lensDirt.value() : m_black, m_bloomSettings.factor, m_bloomSettings.lensDirtIntensity);
}

void PbrRenderer::onGuiDraw()
{
    m_iblPanel.onDraw();
    m_ssaoPanel.onDraw();
    m_bloomPanel.onDraw();
}

void PbrRenderer::onWindowResize(int new_width, int new_height)
{
    m_framebufferWidth = new_width;
    m_framebufferHeight = new_height;

    m_deferredPass.createFramebuffer(new_width, new_height);
    m_pbrPass.createFramebuffer(new_width, new_height);
    m_ssaoPass.createFramebuffer(new_width, new_height);
    m_ssaoBlurPass.createFramebuffer(new_width, new_height);
    m_bloomBlurPass.createFramebuffer(new_width, new_height);
}
