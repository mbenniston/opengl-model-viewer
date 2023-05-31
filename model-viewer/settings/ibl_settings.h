#pragma once
#include "../wrapper/texture.h"

struct IblSettings
{
    int sourceCubemapSize = 512;
    int irrandianceCubemapSize = 32;
    int filteredCubemapSize = 128;
    int brdfLutSize = 512;

    Texture equirectangularEnvironment;
    Texture diffuseCubemap;
    Texture irrandianceCubemap;
    Texture filteredCubemap;
    Texture brdfLut;
};