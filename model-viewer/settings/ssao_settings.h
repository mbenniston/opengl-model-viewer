#pragma once

struct SsaoSettings
{
    bool enabled = true;
    int samples = 16;
    float distance = 0.1f;
    float bias = 0.005f;
    int blurKernel = 11;
};