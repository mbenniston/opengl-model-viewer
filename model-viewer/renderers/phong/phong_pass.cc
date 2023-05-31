#include "phong_pass.h"
#include <array>
#include "../../constants.h"

PhongPass::PhongPass(int width, int height) :
    m_phongShader(Shader::CreateFromPath(
        RESOURCES_PATH + "shaders/phong/phong.vert",
        RESOURCES_PATH + "shaders/phong/phong.frag")),
    m_skyboxShader(Shader::CreateFromPath(
        RESOURCES_PATH + "shaders/phong/phong_skybox.vert",
        RESOURCES_PATH + "shaders/phong/phong_skybox.frag"))
{
    m_uniformLocations.projectionMatrix = m_phongShader.getUniformLocation("u_projection");
    m_uniformLocations.viewMatrix = m_phongShader.getUniformLocation("u_view");
    m_uniformLocations.modelMatrix = m_phongShader.getUniformLocation("u_model");
    m_uniformLocations.baseSampler = m_phongShader.getUniformLocation("u_base");
    m_uniformLocations.emissiveSampler = m_phongShader.getUniformLocation("u_emissive");
    m_uniformLocations.baseFactor = m_phongShader.getUniformLocation("u_baseFactor");
    m_uniformLocations.emissiveFactor = m_phongShader.getUniformLocation("u_emissiveFactor");
    m_uniformLocations.lightDirection = m_phongShader.getUniformLocation("u_lightDirection");
    m_uniformLocations.lightColor = m_phongShader.getUniformLocation("u_lightColor");
    m_uniformLocations.ambient = m_phongShader.getUniformLocation("u_ambient");
    m_uniformLocations.specularPower = m_phongShader.getUniformLocation("u_specularPower");
    m_uniformLocations.specularStrength = m_phongShader.getUniformLocation("u_specularStrength");

    m_skyboxUniformLocations.mvpMatrix = m_skyboxShader.getUniformLocation("u_mvp");
    m_skyboxUniformLocations.diffuseSampler = m_skyboxShader.getUniformLocation("u_skyboxSampler");

    m_whiteTexture = TextureWrapper::CreateSinglePixel(glm::vec3(1.0f, 1.0f, 1.0f));
    m_blackTexture = TextureWrapper::CreateSinglePixel(glm::vec3(0.0f, 0.0f, 0.0f));

    createFramebuffer(width, height);
}

void PhongPass::render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Scene& scene,
    glm::vec3 direction, glm::vec3 lightColor,
    glm::vec3 ambient, float specularPower, float specularStrength)
{
    m_phongShader.bind();
    m_phongShader.loadUniform(m_uniformLocations.projectionMatrix, projectionMatrix);
    m_phongShader.loadUniform(m_uniformLocations.viewMatrix, viewMatrix);
    m_phongShader.loadUniform(m_uniformLocations.baseSampler, 0);
    m_phongShader.loadUniform(m_uniformLocations.emissiveSampler, 1);
    m_phongShader.loadUniform(m_uniformLocations.lightDirection, direction);
    m_phongShader.loadUniform(m_uniformLocations.lightColor, lightColor);
    m_phongShader.loadUniform(m_uniformLocations.ambient, ambient);
    m_phongShader.loadUniform(m_uniformLocations.specularPower, specularPower);
    m_phongShader.loadUniform(m_uniformLocations.specularStrength, specularStrength);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    for (const Node* node : scene.nodes())
    {
        renderNode(*node, glm::mat4(1.0f));
    }

    glBindSampler(0, 0);
    glBindSampler(1, 0);

    m_phongShader.unbind();
}

void PhongPass::drawSkybox(const Texture& diffuseCubemap, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
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
}

void PhongPass::renderNode(const Node& node, const glm::mat4& parentTransform)
{
    glm::mat4 transform = parentTransform * node.transform();

    for (auto child : node.children())
    {
        renderNode(*child, transform);
    }

    m_phongShader.loadUniform(m_uniformLocations.modelMatrix, transform);

    if (!node.mesh().has_value())
    {
        return;
    }

    for (const auto& prim : node.mesh().value()->primitives())
    {
        if (prim.material() != nullptr)
        {
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
            if (mat.emissiveTexture().has_value())
            {
                glBindTexture(GL_TEXTURE_2D, *mat.emissiveTexture().value());
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
            }

            if (mat.baseColorTextureSampler().has_value())
            {
                glBindSampler(0, *mat.baseColorTextureSampler().value());
            }
            else
            {
                glBindSampler(0, 0);
            }

            if (mat.emissiveTextureSampler().has_value())
            {
                glBindSampler(1, *mat.emissiveTextureSampler().value());
            }
            else
            {
                glBindSampler(1, 0);
            }

            m_phongShader.loadUniform(m_uniformLocations.baseFactor, mat.baseColorFactor());
            m_phongShader.loadUniform(m_uniformLocations.emissiveFactor, mat.emissiveFactor());
        }

        prim.draw();
    }
}


void PhongPass::createFramebuffer(int width, int height)
{
    m_framebufferWidth = width;
    m_framebufferHeight = height;
}
