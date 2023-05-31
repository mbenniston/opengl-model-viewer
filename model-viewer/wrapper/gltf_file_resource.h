#pragma once
#include <vector>
#include "scene.h"
#include <gltf/gltf.h>
#include "node.h"

struct IntermediateResources;

// contains all the data loaded into opengl for a single gltf file
class GltfFileResource
{
public:
	GltfFileResource(const glTF::File& source, const std::string& parentFilePath);

	Scene* defaultScene()
	{
		return m_defaultScene;
	}

private:
	void loadScenes(IntermediateResources& resources);
	void loadScene(int sceneIndex, IntermediateResources& resources);
	Node* loadNode(int nodeIndex, IntermediateResources& resources);
	Mesh* loadMesh(int meshIndex, IntermediateResources& resources);
	Primitive loadPrimitive(const glTF::Primitive& primitive, IntermediateResources& resources);
	VertexBuffer* loadBufferView(int bufferViewIndex, IntermediateResources& resources);
	Material* loadMaterial(int materialIndex, IntermediateResources& resources);
	std::pair<std::optional<Texture*>, std::optional<Sampler*>> loadTexture(int textureIndex, IntermediateResources& resources);
	Texture* loadImage(int imageIndex, IntermediateResources& resources);
	Sampler* loadSampler(int samplerIndex, IntermediateResources& resources);

	std::vector<std::optional<VertexBuffer>> m_buffers; // relates to buffer views
	std::vector<std::optional<Texture>> m_textures; // relates to images
	std::vector<std::optional<Sampler>> m_samplers; 
	std::vector<std::optional<Material>> m_materials; 
	std::vector<std::optional<Mesh>> m_meshes;
	std::vector<std::optional<Node>> m_nodes;
	std::vector<std::optional<Scene>> m_scenes;
	Scene* m_defaultScene = nullptr;
};