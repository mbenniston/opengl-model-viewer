#include "glTF_file_resource.h"
#include "wrapper_utils.h"

#include <cppcodec/base64_rfc4648.hpp>
#include <iostream>
#include <fstream>

static std::string GetParentDirectoryPath(const std::string& path)
{
    std::size_t lastSlashPosition = path.find_last_of("\\/");
    return std::string::npos == lastSlashPosition ? "" : path.substr(0, lastSlashPosition);
}

static bool StartsWith(const std::string& text, const std::string& start)
{
    if (text.length() < start.length())
    {
        return false;
    }

    for (std::size_t i = 0; i < start.length(); i++)
    {
        if (text[i] != start[i])
        {
            return false;
        }
    }

    return true;
}

static glTF::BinaryBuffer LoadBinaryBuffer(const glTF::Buffer& gltfBuffer, const std::string& parentFilePath, const glTF::File& file)
{
    if (!gltfBuffer.uri.has_value())
    {
        // assume data is stored in binary buffers
        return file.binaryBuffers.at(0);
    }

    const std::string& uri = gltfBuffer.uri.value();
    const std::string& embeddedPrefix = "data:application/octet-stream;base64,";

    glTF::BinaryBuffer buffer;

    if (StartsWith(uri, embeddedPrefix))
    {
        using base64 = cppcodec::base64_rfc4648;

        buffer.bytes = base64::decode(uri.substr(embeddedPrefix.size()));
    }
    else
    {
        std::string path = parentFilePath + "/" + uri;
        std::fstream fileStream(path, std::ios::binary | std::ios::in);

        if (!fileStream)
        {
            throw std::exception("could not open file");
        }

        fileStream.seekg(0, std::ios::end);
        std::size_t length = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);

        buffer.bytes.resize(length);

        fileStream.read(reinterpret_cast<char*>(buffer.bytes.data()), length);
    }
    
    return buffer;
}

struct IntermediateResources
{
    const glTF::File& file;
	mutable std::vector<std::optional<glTF::BinaryBuffer>> buffers;

    std::string parent_file_path;

    IntermediateResources(const glTF::File& file) : file(file)
    {
    }

    const glTF::BinaryBuffer& getBuffer(int bufferIndex) const
    {
        std::optional<glTF::BinaryBuffer>& bufferData = buffers.at(bufferIndex);

        if (!bufferData.has_value())
        {
            bufferData = LoadBinaryBuffer(file.buffers->at(bufferIndex), parent_file_path, file);
        }

        return bufferData.value();
    }
};

GltfFileResource::GltfFileResource(const glTF::File& source, const std::string& parentFilePath)
{
    m_scenes.resize(source.scenes.value_or(std::vector<glTF::Scene>{}).size());
	m_nodes.resize(source.nodes.value_or(std::vector<glTF::Node>{}).size());
    m_meshes.resize(source.meshes.value_or(std::vector<glTF::Mesh>{}).size());
    m_buffers.resize(source.bufferViews.value_or(std::vector<glTF::BufferView>{}).size());
    m_materials.resize(source.materials.value_or(std::vector<glTF::Material>{}).size());
    m_textures.resize(source.images.value_or(std::vector<glTF::Image>{}).size());
    m_samplers.resize(source.samplers.value_or(std::vector<glTF::Sampler>{}).size());

    IntermediateResources resources(source);
    resources.buffers.resize(source.bufferViews.value_or(std::vector<glTF::BufferView>{}).size());
    resources.parent_file_path = GetParentDirectoryPath(parentFilePath);

    for (int i = 0; i < m_nodes.size(); i++)
    {
        loadNode(i, resources);
    }

	loadScenes(resources);

    m_defaultScene = &m_scenes.at(source.scene.value()).value();
}

void GltfFileResource::loadScenes(IntermediateResources& resources)
{
	for (std::size_t sceneIndex = 0; sceneIndex < resources.file.scenes.value_or(std::vector<glTF::Scene>{}).size(); sceneIndex++)
	{
		loadScene(static_cast<int>(sceneIndex), resources);
	}
}

void GltfFileResource::loadScene(int sceneIndex, IntermediateResources& resources)
{
	const glTF::Scene& scene = resources.file.scenes->at(sceneIndex);

	std::vector<const Node*> nodes;
	nodes.reserve(scene.nodes.size());

	for (int nodeIndex : scene.nodes)
	{
		Node* node = loadNode(nodeIndex, resources);
		nodes.push_back(node);
	}

	m_scenes[sceneIndex] = Scene(nodes);
}

Node* GltfFileResource::loadNode(int nodeIndex, IntermediateResources& resources)
{
	if (m_nodes.at(nodeIndex).has_value())
	{
		return &m_nodes[nodeIndex].value();
	}

	const glTF::Node& nodeData = resources.file.nodes->at(nodeIndex);

	std::vector<const Node*> children;

    if (nodeData.children.has_value())
    {
	    children.reserve(nodeData.children->size());

	    for (int childIndex : nodeData.children.value())
	    {
		    Node* childNode = loadNode(childIndex, resources);
		    children.push_back(childNode);
	    }
    }

	glm::mat4 transform = glm::make_mat4(nodeData.matrix.data());

	glm::quat rotation = glm::quat(
        nodeData.rotation[3], 
        nodeData.rotation[0], 
        nodeData.rotation[1], 
        nodeData.rotation[2]);

	transform *= glm::scale(glm::mat4(1.0f), glm::make_vec3(nodeData.scale.data()));
	transform *= glm::mat4_cast(rotation);
	transform *= glm::translate(glm::mat4(1.0f), glm::make_vec3(nodeData.translation.data()));

    std::optional<const Mesh*> mesh;

	if (nodeData.mesh.has_value())
	{
        mesh = loadMesh(nodeData.mesh.value(), resources);
	}

    m_nodes[nodeIndex].emplace(Node(transform, mesh, children));

	return &m_nodes[nodeIndex].value();
}

Mesh* GltfFileResource::loadMesh(int meshIndex, IntermediateResources& resources)
{
    if (m_meshes.at(meshIndex).has_value())
    {
        return &m_meshes[meshIndex].value();
    }

    const glTF::Mesh& meshData = resources.file.meshes->at(meshIndex);

    std::vector<Primitive> primitives;
    primitives.reserve(meshData.primitives.size());

    for (const glTF::Primitive& primData : meshData.primitives)
    {
        primitives.emplace_back(loadPrimitive(primData, resources));
    }

    m_meshes[meshIndex].emplace(Mesh(std::move(primitives)));
    return &m_meshes[meshIndex].value();
}

Primitive GltfFileResource::loadPrimitive(const glTF::Primitive& primitiveData, IntermediateResources& resources)
{
    Material* mat = nullptr;
    if (primitiveData.material.has_value())
    {
        mat = loadMaterial(primitiveData.material.value(), resources);
    }

    VertexArray vertexArray;
    glBindVertexArray(vertexArray);

    std::vector<VertexBuffer*> vertexBuffers;

    for (glTF::Attribute attributeData : primitiveData.attributes)
    {
        int attribIndex = 0;
        if (attributeData.name == "POSITION")
        {
            attribIndex = 0;
        } 
        else if (attributeData.name == "NORMAL")
        {
            attribIndex = 1;
        }
        else if (attributeData.name == "TEXCOORD_0")
        {
            attribIndex = 2;
        }
        else
        {
            continue;
        }

        const glTF::Accessor& accessorData = resources.file.accessors->at(attributeData.accessorIndex);
        const glTF::BufferView& bufferViewData = resources.file.bufferViews->at(accessorData.bufferView);

        VertexBuffer* vertexBuffer = loadBufferView(accessorData.bufferView, resources);
        glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);

        GLsizei stride = bufferViewData.byteStride.value_or(0);

        glVertexAttribPointer(attribIndex,
            glTF::GetAccessorElementsTypeComponentCount(accessorData.type),
            ComponentTypeToInt(accessorData.componentType),
            accessorData.normalized,
            stride,
            static_cast<void*>((char*)nullptr + accessorData.byteOffset));
        glEnableVertexAttribArray(attribIndex);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        vertexBuffers.push_back(vertexBuffer);
    }
    glBindVertexArray(0);

    if (primitiveData.indices.has_value())
    {
        const glTF::Accessor& accessorData = resources.file.accessors->at(primitiveData.indices.value());
        const glTF::BufferView& bufferViewData = resources.file.bufferViews->at(accessorData.bufferView);
        
        const glTF::BinaryBuffer& bufferData = resources.getBuffer(bufferViewData.buffer);

        IndexBuffer indexBuffer;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferViewData.byteLength - accessorData.byteOffset, bufferData.bytes.data() + bufferViewData.byteOffset + accessorData.byteOffset, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return Primitive(
            std::move(indexBuffer),
            std::move(vertexArray),
            std::move(vertexBuffers),
            ToplogyTypeToInt(primitiveData.mode),
            accessorData.count,
            ComponentTypeToInt(accessorData.componentType),
            mat);
    }
    
    throw std::runtime_error("Non indexed primitive not supported");
}

VertexBuffer* GltfFileResource::loadBufferView(int bufferViewIndex, IntermediateResources& resources)
{
    if (m_buffers.at(bufferViewIndex).has_value())
    {
        return &m_buffers[bufferViewIndex].value();
    }

    const glTF::BufferView& bufferViewData = resources.file.bufferViews->at(bufferViewIndex);

    VertexBuffer vertexBuffer;
    const glTF::BinaryBuffer& bufferData = resources.getBuffer(bufferViewData.buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferViewData.byteLength, bufferData.bytes.data() + bufferViewData.byteOffset, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_buffers[bufferViewIndex].emplace(std::move(vertexBuffer));
    
    return &m_buffers[bufferViewIndex].value();
}

Material* GltfFileResource::loadMaterial(int materialIndex, IntermediateResources& resources)
{
    if (m_materials.at(materialIndex).has_value())
    {
        return &m_materials[materialIndex].value();
    }

    const glTF::Material& matinfo = resources.file.materials->at(materialIndex);
    
    std::optional<const Texture*> base;
    std::optional<const Sampler*> baseSampler;
    std::optional<const Texture*> metal;
    std::optional<const Sampler*> metal_sampler;
    std::optional<const Texture*> emissive;
    std::optional<const Sampler*> emissiveSampler;
    std::optional<const Texture*> occulusion;
    std::optional<const Sampler*> occulusion_sampler;
    
    float metalFactor = 1.0f;
    float roughFactor = 1.0f;
    glm::vec3 emissiveFactor = glm::vec3(0.0f);
    glm::vec4 baseFactor = glm::vec4(1.0f);
    
    if (matinfo.pbrMetallicRoughness.has_value()) {
        if (matinfo.pbrMetallicRoughness->baseColorTexture.has_value())
        {
            auto basePair = loadTexture(matinfo.pbrMetallicRoughness->baseColorTexture->index, resources);
            base = basePair.first;
            baseSampler = basePair.second;
        }
    
        if (matinfo.pbrMetallicRoughness->metallicRoughnessTexture.has_value())
        {
            auto metalPair = loadTexture(matinfo.pbrMetallicRoughness->metallicRoughnessTexture->index, resources);
            metal = metalPair.first;
            metal_sampler = metalPair.second;
        }
    
        metalFactor = matinfo.pbrMetallicRoughness->metallicFactor;
        roughFactor = matinfo.pbrMetallicRoughness->roughnessFactor;
        baseFactor = glm::make_vec4(matinfo.pbrMetallicRoughness->baseColorFactor.data());
    }
    
    if (matinfo.emissiveTexture.has_value())
    {
        auto basePair = loadTexture(matinfo.emissiveTexture->index, resources);
        emissive = basePair.first;
        emissiveSampler = basePair.second;
    }
        
    if (matinfo.occulusionTexture.has_value())
    {
        auto basePair = loadTexture(matinfo.occulusionTexture->index, resources);
        occulusion = basePair.first;
        occulusion_sampler = basePair.second;
    }
    
    emissiveFactor = glm::make_vec3(matinfo.emissiveFactor.data());
    
    m_materials[materialIndex].emplace(Material(baseFactor, base, baseSampler,metalFactor, roughFactor, emissiveFactor, metal, metal_sampler, emissive, emissiveSampler, occulusion, occulusion_sampler));
    return &m_materials[materialIndex].value();
}

std::pair<std::optional<Texture*>, std::optional<Sampler*>> GltfFileResource::loadTexture(int textureIndex, IntermediateResources& resources)
{
    const glTF::Texture& texture = resources.file.textures->at(textureIndex);

    std::optional<Texture*> image;
    std::optional<Sampler*> sampler;

    if (texture.source.has_value())
    {
        image = loadImage(texture.source.value(), resources);
    }

    if (texture.sampler.has_value())
    {
        sampler = loadSampler(texture.sampler.value(), resources);
    }

    return std::make_pair(image, sampler);
}

Texture* GltfFileResource::loadImage(int imageIndex, IntermediateResources& resources)
{
    if (m_textures.at(imageIndex).has_value())
    {
        return &m_textures[imageIndex].value();
    }

    const glTF::Image& imageData = resources.file.images->at(imageIndex);

    if (imageData.uri.has_value()) {
        const std::string& jpeg_prefix = "data:image/jpeg;base64,";
        const std::string& png_prefix = "data:image/png;base64,";

        if (StartsWith(imageData.uri.value(), jpeg_prefix))
        {
            using base64 = cppcodec::base64_rfc4648;

            std::vector<std::uint8_t> bytes = base64::decode(imageData.uri.value().substr(jpeg_prefix.size()));
            m_textures[imageIndex] = TextureWrapper::LoadFromData(bytes);
        }
        if (StartsWith(imageData.uri.value(), png_prefix))
        {
            using base64 = cppcodec::base64_rfc4648;

            std::vector<std::uint8_t> bytes = base64::decode(imageData.uri.value().substr(png_prefix.size()));
            m_textures[imageIndex] = TextureWrapper::LoadFromData(bytes);
        }
        else
        {
            std::string full_path = resources.parent_file_path + "/" + imageData.uri.value();

            m_textures[imageIndex] = TextureWrapper::LoadFromPath(full_path);
        }
        return &m_textures[imageIndex].value();
    }

    if (imageData.bufferView.has_value())
    {
        const glTF::BufferView& textureView = resources.file.bufferViews->at(imageData.bufferView.value());
        const glTF::BinaryBuffer& buffer = resources.getBuffer(textureView.buffer);

        m_textures[imageIndex] = TextureWrapper::LoadFromData(
            buffer.bytes.data() + textureView.byteOffset, 
            textureView.byteLength);

        return &m_textures[imageIndex].value();
    }

    throw std::runtime_error("could not load texture");
}

Sampler* GltfFileResource::loadSampler(int samplerIndex, IntermediateResources& resources)
{
    if (m_samplers.at(samplerIndex).has_value())
    {
        return &m_samplers[samplerIndex].value();
    }

    const glTF::Sampler& samplerData = resources.file.samplers->at(samplerIndex);

    Sampler sampler;

    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, SamplerWrapToEnum(samplerData.wrapS));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, SamplerWrapToEnum(samplerData.wrapT));

    glTF::MinFilter minFilter = samplerData.minFilter.value_or(glTF::MinFilter::Linear);
    glTF::MagFilter magFilter = samplerData.magFilter.value_or(glTF::MagFilter::Linear);

    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, MinFilterToEnum(minFilter));
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, MagFilterToEnum(magFilter));

    m_samplers[samplerIndex].emplace(std::move(sampler));
    return &m_samplers[samplerIndex].value();
}
