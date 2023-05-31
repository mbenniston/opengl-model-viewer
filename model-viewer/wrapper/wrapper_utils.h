#pragma once 
#include <glad.h>
#include <gltf/gltf.h>

GLenum ToplogyTypeToInt(glTF::TopologyType type);
GLenum ComponentTypeToInt(glTF::AccessorComponentType componentType);
int SizeOfComponent(glTF::AccessorComponentType componentType);
GLenum SamplerWrapToEnum(glTF::Wrap wrap);
GLenum MinFilterToEnum(glTF::MinFilter minFilter);
GLenum MagFilterToEnum(glTF::MagFilter magFilter);
