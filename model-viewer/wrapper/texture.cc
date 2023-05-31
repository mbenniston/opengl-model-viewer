#include "texture.h"

template class Handle<GLuint, TextureWrapper::CreateTexture, TextureWrapper::DeleteTexture>;

namespace TextureWrapper
{
	GLuint CreateTexture()
	{
		GLuint v;
		glGenTextures(1, &v);
		return v;
	}

	void DeleteTexture(GLuint vb)
	{
		glDeleteTextures(1, &vb);
	}

	Texture LoadFromData(const std::uint8_t* file_data, std::size_t length)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		stbi_set_flip_vertically_on_load(false);

		if (length > std::numeric_limits<int>().max())
		{
			throw std::runtime_error("File too large to load");
		}

		int w, h, c;
		unsigned char* data = stbi_load_from_memory(file_data, static_cast<int>(length), &w, &h, &c, 0);
		if (!data) {
			std::cout << stbi_failure_reason() << std::endl;
			throw TextureLoadException();
		}

		Texture texture;
		glBindTexture(GL_TEXTURE_2D, texture);

		switch (c)
		{
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		default:
			throw TextureLoadException();
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);

		return texture;
	}

	Texture LoadFromData(const std::vector<std::uint8_t>& fileData)
	{
		return LoadFromData(fileData.data(), fileData.size());
	}

	Texture LoadFromPath(const std::string& path)
	{
		stbi_set_flip_vertically_on_load(false);

		int w, h, c;
		unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);
		if (!data) {
			throw TextureLoadException();
		}

		Texture texture;
		glBindTexture(GL_TEXTURE_2D, texture);

		switch (c)
		{
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		default:
			throw TextureLoadException();
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);

		return texture;
	}

	Texture LoadHDRFromPath(const std::string& path)
	{
		stbi_set_flip_vertically_on_load(true);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		int w, h, c;
		float* data = stbi_loadf(path.c_str(), &w, &h, &c, 0);
		if (!data) {
			throw TextureLoadException();
		}

		Texture texture;
		glBindTexture(GL_TEXTURE_2D, texture);

		switch (c)
		{
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RED, GL_FLOAT, data);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, data);
			break;
		default:
			throw TextureLoadException();
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);

		return texture;
	}

	Texture CreateSinglePixel(glm::vec3 color)
	{
		Texture texture;

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_FLOAT, glm::value_ptr(color));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return texture;
	}
}