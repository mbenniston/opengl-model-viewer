#pragma once
#include "../irenderer.h"

#include "deferred_pass.h"
#include "deferred_debug_pass.h"
#include "bloom_pass.h"
#include "blur_pass.h"
#include "pbr_pass.h"
#include "ssao_pass.h"
#include "ibl_baking.h"

#include "../../settings/scene_settings.h"
#include "../../settings/bloom_settings.h"
#include "../../settings/ssao_settings.h"
#include "../../settings/ibl_settings.h"
#include "../../panels/ssao_panel.h"
#include "../../panels/ibl_panel.h"
#include "../../panels/bloom_panel.h"

class PbrRenderer : public IRenderer
{
public:
    PbrRenderer(SceneSettings& sceneSettings);

    virtual void onRender(const glm::mat4& projectionMatrix, glm::mat4& viewMatrix);
    virtual void onGuiDraw();
    virtual void onWindowResize(int newWidth, int newHeight);

private:
    int m_framebufferWidth;
    int m_framebufferHeight;

    SceneSettings& m_sceneSettings;
    IblBaker m_iblBaker;

    BloomSettings m_bloomSettings;
    SsaoSettings m_ssaoSettings;
    IblSettings m_iblSettings;

    BloomPanel m_bloomPanel;
    SsaoPanel m_ssaoPanel;
    IblPanel m_iblPanel;

    DeferredPass m_deferredPass;
    DeferredDebugPass m_deferredDebugPass;
    PbrPass m_pbrPass;
    SsaoPass m_ssaoPass;
    BlurPass m_ssaoBlurPass;
    BlurPass m_bloomBlurPass;
    BloomPass m_bloomPass;

    Texture m_black;
};