#ifndef _STULU_BRANCHLESS_GLSL_
#define _STULU_BRANCHLESS_GLSL_

#define st_impl_for_all_types(func) \
func(vec4)		\
func(vec3)		\
func(vec2)		\
func(float)		\
func(int)		\
func(uint)		\
func(double)

#define st_when_eq_impl(type) type when_eq(type x, type y) { return type(type(1.0) - abs(sign(x-y)));}
#define st_when_neq_impl(type) type when_neq(type x, type y) { return type(abs(sign(x-y)));}
#define st_when_gt_impl(type) type when_gt(type x, type y) { return type(max(sign(x-y), type(0.0)));}
#define st_when_lt_impl(type) type when_lt(type x, type y) { return type(max(sign(y-x), type(0.0)));}
#define st_when_ge_impl(type) type when_ge(type x, type y) { return type(type(1.0)-when_gt(x,y));}
#define st_when_le_impl(type) type when_le(type x, type y) { return type(type(1.0)-when_lt(x,y));}
#define st_and_impl(type) type and(type a, type b) { return type(a*b);}
#define st_or_impl(type) type or(type a, type b) { return type(min(a+b, type(1.0)));}
#define st_not_impl(type) type not(type a, type b) { return type(1.0)-a;}

st_impl_for_all_types(st_when_eq_impl)
st_impl_for_all_types(st_when_neq_impl)
st_impl_for_all_types(st_when_gt_impl)
st_impl_for_all_types(st_when_lt_impl)
st_impl_for_all_types(st_when_ge_impl)
st_impl_for_all_types(st_when_le_impl)
st_impl_for_all_types(st_and_impl)
st_impl_for_all_types(st_or_impl)
st_impl_for_all_types(st_not_impl)

float when_zero(float x) { return 1.0 - min(1.0, abs(x)); }

#include "Stulu/Flags.glsl"

float FisFlagEnabled(uint flags, uint flag) {
	return when_eq(flags & flag, flag);
}
float FisFlagDisabled(uint flags, uint flag) {
	return when_neq(flags & flag, flag);
}
#endif