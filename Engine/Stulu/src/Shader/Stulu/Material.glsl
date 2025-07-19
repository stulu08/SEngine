#ifndef _STULU_MATERIAL_GLSL_
#define _STULU_MATERIAL_GLSL_

#include "Stulu/Buffer/DefaultMaterialBuffer.glsl"

float FilterAlpha(float alpha, uint mode, float cutOut) {
    float isCutout  = float(when_eq(mode, 1));
    float isBlend   = float(when_eq(mode, 2));
    float isOpaque  = 1.0 - isCutout - isBlend;

    float alphaCut  = when_gt(alpha, cutOut);

    return isCutout * alphaCut +
           isBlend  * alpha +
           isOpaque * 1.0;
}
float FilterAlpha(float alpha, uint mode) {
    return FilterAlpha(alpha, mode, ST_AlphaCutOff);
}
float FilterAlpha(float alpha) {
    return FilterAlpha(alpha, ST_AlphaMode, ST_AlphaCutOff);
}

#define auto_align 0
#define Sampler2D(name, offset, binding, default)
#define Color(name, offset, isHDR)
#define Float(name, offset, minVal, maxVal)
#define Float2(name, offset)
#define Float4(name, offset)

#endif