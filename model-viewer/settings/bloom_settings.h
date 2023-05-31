#pragma once 
#include <optional>
#include "../wrapper/texture.h"

struct BloomSettings
{
    int blurKernel = 11;
    float stdDev = 2.0f;
    std::optional<Texture> lensDirt;
    float factor = 0.1f;
    float lensDirtIntensity = 0.0f;
};