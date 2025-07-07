#ifndef _STULU_MATERIAL_GLSL_
#define _STULU_MATERIAL_GLSL_

#include "Stulu/Buffer/DefaultMaterialBuffer.glsl"

float FilterAlpha(float alpha, uint mode, float cutOut) {
    if(mode == 1) {
		if(alpha > cutOut){
			return 1.0;//full opaque
		}else{
			return 0.0;//full transparent
		}
	}
	else if(mode == 2) {
		return alpha;//keep alpha
	}
    else{
		return 1.0;//ignore alpha
	}

	float value = -1.0;
    value += 1.0 * float(when_eq(mode, 1)) * when_gt(alpha, cutOut);
    value += alpha * float(when_eq(mode, 2));
    value += 1.0 * float(when_neq(value, -1.0));
    return value;
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