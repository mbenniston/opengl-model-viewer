#include "wrapper_utils.h"


GLenum ToplogyTypeToInt(glTF::TopologyType type)
{
    switch (type)
    {
    case glTF::TopologyType::Points:
        return GL_POINTS;
    case glTF::TopologyType::Lines:
        return GL_LINES;
    case glTF::TopologyType::LineLoop:
        return GL_LINE_LOOP;
    case glTF::TopologyType::LineStrip:
        return GL_LINE_STRIP;
    case glTF::TopologyType::Triangles:
        return GL_TRIANGLES;
    case glTF::TopologyType::TrianglesStrip:
        return GL_TRIANGLE_STRIP;
    case glTF::TopologyType::TrianglesFan:
        return GL_TRIANGLE_FAN;
    }

    throw std::invalid_argument("Invalid toplogy type");
}

GLenum ComponentTypeToInt(glTF::AccessorComponentType componentType)
{
    switch (componentType)
    {
    case glTF::AccessorComponentType::Byte:
        return GL_BYTE;
    case glTF::AccessorComponentType::UnsignedByte:
        return GL_UNSIGNED_BYTE;
    case glTF::AccessorComponentType::Short:
        return GL_SHORT;
    case glTF::AccessorComponentType::UnsignedShort:
        return GL_UNSIGNED_SHORT;
    case glTF::AccessorComponentType::UnsignedInt:
        return GL_UNSIGNED_INT;
    case glTF::AccessorComponentType::Float:
        return GL_FLOAT;
    }

    throw std::invalid_argument("Invalid component type");
}

int SizeOfComponent(glTF::AccessorComponentType componentType)
{
    switch (componentType)
    {
    case glTF::AccessorComponentType::Byte:
        return 1;
    case glTF::AccessorComponentType::UnsignedByte:
        return 1;
    case glTF::AccessorComponentType::Short:
        return 2;
    case glTF::AccessorComponentType::UnsignedShort:
        return 2;
    case glTF::AccessorComponentType::UnsignedInt:
        return 4;
    case glTF::AccessorComponentType::Float:
        return 4;
    }

    throw std::invalid_argument("Invalid component type");
}

GLenum SamplerWrapToEnum(glTF::Wrap wrap)
{
    switch (wrap)
    {
    case glTF::Wrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case glTF::Wrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    case glTF::Wrap::Repeat:
        return GL_REPEAT;
    }

    throw std::invalid_argument("Invalid wrap type");
}

GLenum MinFilterToEnum(glTF::MinFilter minFilter)
{
    switch (minFilter)
    {
    case glTF::MinFilter::Nearest:
        return GL_NEAREST;
    case glTF::MinFilter::Linear:
        return GL_LINEAR;
    case glTF::MinFilter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case glTF::MinFilter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    case glTF::MinFilter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case glTF::MinFilter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    }

    throw std::invalid_argument("Invalid min filter type");
}

GLenum MagFilterToEnum(glTF::MagFilter magFilter)
{
    switch (magFilter)
    {
    case glTF::MagFilter::Nearest:
        return GL_NEAREST;
    case glTF::MagFilter::Linear:
        return GL_LINEAR;
    }

    throw std::invalid_argument("Invalid mags filter type");
}