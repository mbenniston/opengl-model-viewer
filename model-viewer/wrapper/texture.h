#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <stb_image.h>
#include <glad.h>
#include <glm_nowarn.h>
#include "handle.h"

class TextureLoadException : public std::exception {};

namespace TextureWrapper
{
	GLuint CreateTexture();
	void DeleteTexture(GLuint vb);
}

extern template class Handle<GLuint, TextureWrapper::CreateTexture, TextureWrapper::DeleteTexture>;
class Texture : public Handle<GLuint, TextureWrapper::CreateTexture, TextureWrapper::DeleteTexture> {};

namespace TextureWrapper
{
	Texture LoadFromData(const std::uint8_t* file_data, std::size_t length);
	Texture LoadFromData(const std::vector<std::uint8_t>& fileData);
	Texture LoadFromPath(const std::string& path);
	Texture LoadHDRFromPath(const std::string& path);
	Texture CreateSinglePixel(glm::vec3 color);
}