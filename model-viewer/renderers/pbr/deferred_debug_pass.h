#pragma once
#include "../../wrapper/framebuffer.h"
#include "../../wrapper/render_buffer.h"
#include "../../wrapper/shader.h"
#include "../../wrapper/scene.h"
#include "deferred_pass.h"

class DeferredDebugPass
{
public:
	DeferredDebugPass();

	void start_debug(DeferredPass& deferredRender, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
	void render_debug(const Scene& scene);
	void stop_debug();

private:
	void render_node_debug(const Node& node, const glm::mat4& parentTransform);

	Shader m_debugShader;

	struct {
		GLint projectionMatrix;
		GLint viewMatrix;
		GLint modelMatrix;
	} m_debugUniformLocations;
};