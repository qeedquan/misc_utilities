#include <stdio.h>
#include <string.h>
#include <math.h>
#include "linalg.h"

float2
xyplr(float2 a)
{
	return (float2){
	    .r = hypot(a.x, a.y),
	    .t = atan2(a.y, a.x),
	};
}

float2
plrxy(float2 a)
{
	return (float2){
	    .x = a.r * cos(a.t),
	    .y = a.r * sin(a.t),
	};
}

float3
xyz2(float2 a)
{
	return (float3){
	    .x = a.x,
	    .y = a.y,
	    .z = 1,
	};
}

float2
reciprocal2(float2 a)
{
	return (float2){.x = 1.0 / a.x, .y = 1.0 / a.y};
}

float2
add2(float2 a, float2 b)
{
	return (float2){
	    .x = a.x + b.x,
	    .y = a.y + b.y,
	};
}

float2
sub2(float2 a, float2 b)
{
	return (float2){
	    .x = a.x - b.x,
	    .y = a.y - b.y,
	};
}

float2
scale2(float2 a, float s)
{
	return (float2){
	    .x = a.x * s,
	    .y = a.y * s,
	};
}

float2
min2(float2 a, float2 b)
{
	return (float2){
	    .x = min(a.x, b.x),
	    .y = min(a.y, b.y),
	};
}

float2
max2(float2 a, float2 b)
{
	return (float2){
	    .x = max(a.x, b.x),
	    .y = max(a.y, b.y),
	};
}

float2
abs2(float2 a)
{
	return (float2){
	    .x = fabs(a.x),
	    .y = fabs(a.y),
	};
}

float
mincomp2(float2 a)
{
	return min(a.x, a.y);
}

float
maxcomp2(float2 a)
{
	return max(a.x, a.y);
}

float
dot2(float2 a, float2 b)
{
	return a.x * b.x + a.y * b.y;
}

float
ndot2(float2 a, float2 b)
{
	return a.x * b.x - a.y * b.y;
}

float
len2(float2 a)
{
	return sqrt(dot2(a, a));
}

float2
normalize2(float2 a)
{
	float l;

	l = len2(a);
	if (l == 0)
		return a;
	return scale2(a, 1 / l);
}

float2
perp2(float2 a)
{
	return (float2){
	    .x = -a.y,
	    .y = a.x,
	};
}

float2
proj2(float2 a, float2 b)
{
	b = normalize2(b);
	return scale2(b, dot2(a, b));
}

float2
rej2(float2 a, float2 b)
{
	return sub2(a, proj2(a, b));
}

float2
hadamard2(float2 a, float2 b)
{
	return (float2){
	    .x = a.x * b.x,
	    .y = a.y * b.y,
	};
}

float2
lerp2(float t, float2 a, float2 b)
{
	return (float2){
	    .x = lerp(t, a.x, b.x),
	    .y = lerp(t, a.y, b.y),
	};
}

float2
unlerp2(float t, float2 a, float2 b)
{
	return (float2){
	    .x = unlerp(t, a.x, b.x),
	    .y = unlerp(t, a.y, b.y),
	};
}

float2
saturate2(float2 a)
{
	return (float2){
	    .x = saturatef(a.x),
	    .y = saturatef(a.y),
	};
}

float
wedge2(float2 a, float2 b)
{
	return a.x * b.y - a.y * b.x;
}

float2
reflect2(float2 a, float2 v)
{
	float s = 2 * dot2(v, a) / dot2(a, a);
	return sub2(v, scale2(a, s));
}

void
outer2(float2x2 m, float2 a, float2 b)
{
	size_t i, j;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++)
			m[i][j] = a.e[i] * b.e[j];
	}
}

float2
sin2(float2 a)
{
	return (float2){
	    .x = sin(a.x),
	    .y = sin(a.y),
	};
}

float2
cos2(float2 a)
{
	return (float2){
	    .x = cos(a.x),
	    .y = cos(a.y),
	};
}

float2
tan2(float2 a)
{
	return (float2){
	    .x = tan(a.x),
	    .y = tan(a.y),
	};
}

float2
sqrt2(float2 a)
{
	return (float2){
	    .x = sqrt(a.x),
	    .y = sqrt(a.y),
	};
}

float2
cbrt2(float2 a)
{
	return (float2){
	    .x = cbrt(a.x),
	    .y = cbrt(a.y),
	};
}

float2
pow2(float2 a, float2 b)
{
	return (float2){
	    .x = pow(a.x, b.x),
	    .y = pow(a.y, b.y),
	};
}

float2
fma2(float2 a, float2 b, float2 c)
{
	return (float2){
	    .x = fma(a.x, b.x, c.x),
	    .y = fma(a.y, b.y, c.y),
	};
}

float2
sign2(float2 a)
{
	return (float2){
	    .x = signf(a.x),
	    .y = signf(a.y),
	};
}

float2
clamp2(float2 a, float2 b, float2 c)
{
	return (float2){
	    .x = clampf(a.x, b.x, c.x),
	    .y = clampf(a.y, b.y, c.y),
	};
}

float3
xyzsph(float3 a)
{
	return (float3){
	    .rd = sqrt(a.x * a.x + a.y * a.y + a.z * a.z),
	    .t = atan2(sqrt(a.x * a.x + a.y * a.y), a.z),
	    .p = atan2(a.y, a.x),
	};
}

float3
sphxyz(float3 a)
{
	float st = sin(a.t);
	float ct = cos(a.t);
	float sp = sin(a.p);
	float cp = cos(a.p);
	return (float3){
	    .x = a.rd * st * cp,
	    .y = a.rd * st * sp,
	    .z = a.rd * ct,
	};
}

float3
xyzcyl(float3 a)
{
	return (float3){
	    .x = sqrt(a.x * a.x + a.y * a.y),
	    .y = atan2(a.y, a.x),
	    .z = a.z,
	};
}

float3
cylxyz(float3 a)
{
	float cp = cos(a.y);
	float sp = sin(a.y);
	return (float3){
	    .x = a.x * cp,
	    .y = a.x * sp,
	    .z = a.z,
	};
}

float2
xy3(float3 a)
{
	return (float2){.x = a.x, .y = a.y};
}

float2
xz3(float3 a)
{
	return (float2){.x = a.x, .y = a.z};
}

float3
yzx3(float3 a)
{
	return (float3){.x = a.y, .y = a.z, .z = a.x};
}

float3
zxy3(float3 a)
{
	return (float3){.x = a.z, .y = a.x, .z = a.y};
}

float4
xyzw3(float3 a, float w)
{
	return (float4){.x = a.x, .y = a.y, .z = a.z, .w = w};
}

float3
reciprocal3(float3 a)
{
	return (float3){.x = 1.0 / a.x, .y = 1.0 / a.y, .z = 1.0 / a.z};
}

float3
add3(float3 a, float3 b)
{
	return (float3){
	    .x = a.x + b.x,
	    .y = a.y + b.y,
	    .z = a.z + b.z,
	};
}

float3
sub3(float3 a, float3 b)
{
	return (float3){
	    .x = a.x - b.x,
	    .y = a.y - b.y,
	    .z = a.z - b.z,
	};
}

float3
scale3(float3 a, float s)
{
	return (float3){
	    .x = a.x * s,
	    .y = a.y * s,
	    .z = a.z * s,
	};
}

float3
min3(float3 a, float3 b)
{
	return (float3){
	    .x = min(a.x, b.x),
	    .y = min(a.y, b.y),
	    .z = min(a.z, b.z),
	};
}

float3
max3(float3 a, float3 b)
{
	return (float3){
	    .x = max(a.x, b.x),
	    .y = max(a.y, b.y),
	    .z = max(a.z, b.z),
	};
}

float3
abs3(float3 a)
{
	return (float3){
	    .x = fabs(a.x),
	    .y = fabs(a.y),
	    .z = fabs(a.z),
	};
}

float
mincomp3(float3 a)
{
	return min(a.x, min(a.y, a.z));
}

float
maxcomp3(float3 a)
{
	return max(a.x, max(a.y, a.z));
}

float
dot3(float3 a, float3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float
ndot3(float3 a, float3 b)
{
	return a.x * b.x - a.y * b.y - a.z * b.z;
}

float3
cross3(float3 a, float3 b)
{
	return (float3){
	    .x = a.y * b.z - a.z * b.y,
	    .y = a.z * b.x - a.x * b.z,
	    .z = a.x * b.y - a.y * b.x,
	};
}

float
len3(float3 a)
{
	return sqrt(dot3(a, a));
}

float3
normalize3(float3 a)
{
	float l;

	l = len3(a);
	if (l == 0)
		return a;
	return scale3(a, 1 / l);
}

float3
proj3(float3 a, float3 b)
{
	b = normalize3(b);
	return scale3(b, dot3(a, b));
}

float3
rej3(float3 a, float3 b)
{
	return sub3(a, proj3(a, b));
}

float3
hadamard3(float3 a, float3 b)
{
	return (float3){
	    .x = a.x * b.x,
	    .y = a.y * b.y,
	    .z = a.z * b.z,
	};
}

float3
lerp3(float t, float3 a, float3 b)
{
	return (float3){
	    .x = lerp(t, a.x, b.x),
	    .y = lerp(t, a.y, b.y),
	    .z = lerp(t, a.z, b.z),
	};
}

float3
unlerp3(float t, float3 a, float3 b)
{
	return (float3){
	    .x = unlerp(t, a.x, b.x),
	    .y = unlerp(t, a.y, b.y),
	    .z = unlerp(t, a.z, b.z),
	};
}

float3
saturate3(float3 a)
{
	return (float3){
	    .x = saturatef(a.x),
	    .y = saturatef(a.y),
	    .z = saturatef(a.z),
	};
}

float3
wedge3(float3 a, float3 b)
{
	return (float3){
	    .x = a.x * b.y - a.y * b.x,
	    .y = a.x * b.z - a.z * b.x,
	    .z = a.y * b.z - a.z * b.y,
	};
}

float3
reflect3(float3 a, float3 v)
{
	float s = 2 * dot3(v, a) / dot3(a, a);
	return sub3(v, scale3(a, s));
}

void
outer3(float3x3 m, float3 a, float3 b)
{
	size_t i, j;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++)
			m[i][j] = a.e[i] * b.e[j];
	}
}

float3
sin3(float3 a)
{
	return (float3){
	    .x = sin(a.x),
	    .y = sin(a.y),
	    .z = sin(a.z),
	};
}

float3
cos3(float3 a)
{
	return (float3){
	    .x = cos(a.x),
	    .y = cos(a.y),
	    .z = cos(a.z),
	};
}

float3
tan3(float3 a)
{
	return (float3){
	    .x = tan(a.x),
	    .y = tan(a.y),
	    .z = tan(a.z),
	};
}

float3
sqrt3(float3 a)
{
	return (float3){
	    .x = sqrt(a.x),
	    .y = sqrt(a.y),
	    .z = sqrt(a.z),
	};
}

float3
cbrt3(float3 a)
{
	return (float3){
	    .x = cbrt(a.x),
	    .y = cbrt(a.y),
	    .z = cbrt(a.z),
	};
}

float3
pow3(float3 a, float3 b)
{
	return (float3){
	    .x = pow(a.x, b.x),
	    .y = pow(a.y, b.y),
	    .z = pow(a.z, b.z),
	};
}

float3
fma3(float3 a, float3 b, float3 c)
{
	return (float3){
	    .x = fma(a.x, b.x, c.x),
	    .y = fma(a.y, b.y, c.y),
	    .z = fma(a.z, b.z, c.z),
	};
}

float3
sign3(float3 a)
{
	return (float3){
	    .x = signf(a.x),
	    .y = signf(a.y),
	    .z = signf(a.z),
	};
}

float3
clamp3(float3 a, float3 b, float3 c)
{
	return (float3){
	    .x = clampf(a.x, b.x, c.x),
	    .y = clampf(a.y, b.y, c.y),
	    .z = clampf(a.z, b.z, c.z),
	};
}

float2
xy4(float4 a)
{
	return (float2){
	    .x = a.x,
	    .y = a.y,
	};
}

float3
xyz4(float4 a)
{
	return (float3){
	    .x = a.x,
	    .y = a.y,
	    .z = a.z,
	};
}

float4
reciprocal4(float4 a)
{
	return (float4){.x = 1.0 / a.x, .y = 1.0 / a.y, .z = 1.0 / a.z, .w = 1.0 / a.w};
}

float4
add4(float4 a, float4 b)
{
	return (float4){
	    .x = a.x + b.x,
	    .y = a.y + b.y,
	    .z = a.z + b.z,
	    .w = a.w + b.w,
	};
}

float4
sub4(float4 a, float4 b)
{
	return (float4){
	    .x = a.x - b.x,
	    .y = a.y - b.y,
	    .z = a.z - b.z,
	    .w = a.w - b.w,
	};
}

float4
scale4(float4 a, float s)
{
	return (float4){
	    .x = a.x * s,
	    .y = a.y * s,
	    .z = a.z * s,
	    .w = a.w * s,
	};
}

float4
min4(float4 a, float4 b)
{
	return (float4){
	    .x = min(a.x, b.x),
	    .y = min(a.y, b.y),
	    .z = min(a.z, b.z),
	    .w = min(a.w, b.w),
	};
}

float4
max4(float4 a, float4 b)
{
	return (float4){
	    .x = max(a.x, b.x),
	    .y = max(a.y, b.y),
	    .z = max(a.z, b.z),
	    .w = max(a.w, b.w),
	};
}

float4
abs4(float4 a)
{
	return (float4){
	    .x = fabs(a.x),
	    .y = fabs(a.y),
	    .z = fabs(a.z),
	    .w = fabs(a.w),
	};
}

float
mincomp4(float4 a)
{
	return min(a.x, min(a.y, min(a.z, a.w)));
}

float
maxcomp4(float4 a)
{
	return max(a.x, max(a.y, max(a.z, a.w)));
}

float
dot4(float4 a, float4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float
ndot4(float4 a, float4 b)
{
	return a.x * b.x - a.y * b.y - a.z * b.z - a.w * b.w;
}

float
len4(float4 a)
{
	return sqrt(dot4(a, a));
}

float4
normalize4(float4 a)
{
	float l;

	l = len4(a);
	if (l == 0)
		return a;
	return scale4(a, 1 / l);
}

float4
hadamard4(float4 a, float4 b)
{
	return (float4){
	    .x = a.x * b.x,
	    .y = a.y * b.y,
	    .z = a.z * b.z,
	    .w = a.w * b.w,
	};
}

float4
lerp4(float t, float4 a, float4 b)
{
	return (float4){
	    .x = lerp(t, a.x, b.x),
	    .y = lerp(t, a.y, b.y),
	    .z = lerp(t, a.z, b.z),
	    .w = lerp(t, a.w, b.w),
	};
}

float4
unlerp4(float t, float4 a, float4 b)
{
	return (float4){
	    .x = unlerp(t, a.x, b.x),
	    .y = unlerp(t, a.y, b.y),
	    .z = unlerp(t, a.z, b.z),
	    .w = unlerp(t, a.w, b.w),
	};
}

float4
saturate4(float4 a)
{
	return (float4){
	    .x = saturatef(a.x),
	    .y = saturatef(a.y),
	    .z = saturatef(a.z),
	    .w = saturatef(a.w),
	};
}

void
outer4(float4x4 m, float4 a, float4 b)
{
	size_t i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			m[i][j] = a.e[i] * b.e[j];
	}
}

float4
sin4(float4 a)
{
	return (float4){
	    .x = sin(a.x),
	    .y = sin(a.y),
	    .z = sin(a.z),
	    .w = sin(a.w),
	};
}

float4
cos4(float4 a)
{
	return (float4){
	    .x = cos(a.x),
	    .y = cos(a.y),
	    .z = cos(a.z),
	    .w = cos(a.w),
	};
}

float4
tan4(float4 a)
{
	return (float4){
	    .x = tan(a.x),
	    .y = tan(a.y),
	    .z = tan(a.z),
	    .w = tan(a.w),
	};
}

float4
sqrt4(float4 a)
{
	return (float4){
	    .x = sqrt(a.x),
	    .y = sqrt(a.y),
	    .z = sqrt(a.z),
	    .w = sqrt(a.w),
	};
}

float4
cbrt4(float4 a)
{
	return (float4){
	    .x = cbrt(a.x),
	    .y = cbrt(a.y),
	    .z = cbrt(a.z),
	    .w = cbrt(a.w),
	};
}

float4
pow4(float4 a, float4 b)
{
	return (float4){
	    .x = pow(a.x, b.x),
	    .y = pow(a.y, b.y),
	    .z = pow(a.z, b.z),
	    .w = pow(a.w, b.w),
	};
}

float4
fma4(float4 a, float4 b, float4 c)
{
	return (float4){
	    .x = fma(a.x, b.x, c.x),
	    .y = fma(a.y, b.y, c.y),
	    .z = fma(a.z, b.z, c.z),
	    .w = fma(a.w, b.w, c.w),
	};
}

float4
sign4(float4 a)
{
	return (float4){
	    .x = signf(a.x),
	    .y = signf(a.y),
	    .z = signf(a.z),
	    .w = signf(a.w),
	};
}

float4
clamp4(float4 a, float4 b, float4 c)
{
	return (float4){
	    .x = clampf(a.x, b.x, c.x),
	    .y = clampf(a.y, b.y, c.y),
	    .z = clampf(a.z, b.z, c.z),
	    .w = clampf(a.w, b.w, c.w),
	};
}

float4
qeye(void)
{
	return (float4){.x = 0, .y = 0, .z = 0, .w = 1};
}

float4
qmul(float4 a, float4 b)
{
	return (float4){
	    .x = a.x * b.w + a.w * b.x + a.y * b.z - a.z * b.y,
	    .y = a.y * b.w + a.w * b.y + a.z * b.x - a.x * b.z,
	    .z = a.z * b.w + a.w * b.z + a.x * b.y - a.y * b.x,
	    .w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
	};
}

float4
qconj(float4 a)
{
	return (float4){
	    .x = -a.x,
	    .y = -a.y,
	    .z = -a.z,
	    .w = a.w,
	};
}

float4
qaxis(float3 a, float r)
{
	float3 w;
	float s, c;

	w = normalize3(a);
	s = sin(r / 2);
	c = cos(r / 2);
	return (float4){
	    .x = w.x * s,
	    .y = w.y * s,
	    .z = w.z * s,
	    .w = c,
	};
}

float4
qeuler(float3 a)
{
	float y = a.x;
	float p = a.y;
	float r = a.z;
	float cy = cos(y * 0.5);
	float sy = sin(y * 0.5);
	float cp = cos(p * 0.5);
	float sp = sin(p * 0.5);
	float cr = cos(r * 0.5);
	float sr = sin(r * 0.5);
	return (float4){
	    .x = sr * cp * cy - cr * sp * sy,
	    .y = cr * sp * cy + sr * cp * sy,
	    .z = cr * cp * sy - sr * sp * cy,
	    .w = cr * cp * cy + sr * sp * sy,
	};
}

float4
qrotate4x4(float4x4 m)
{
	float x = 1 + m[0][0] - m[1][1] - m[2][2];
	float y = 1 - m[0][0] + m[1][1] - m[2][2];
	float z = 1 - m[0][0] - m[1][1] + m[2][2];
	float w = 1 + m[0][0] + m[1][1] + m[2][2];
	x = (x < 0) ? 0 : sqrt(x) / 2;
	y = (y < 0) ? 0 : sqrt(y) / 2;
	z = (z < 0) ? 0 : sqrt(z) / 2;
	w = (w < 0) ? 0 : sqrt(w) / 2;
	if (m[2][1] - m[1][2] < 0)
		x = -x;
	if (m[0][2] - m[2][0] < 0)
		y = -y;
	if (m[1][0] - m[0][1] < 0)
		z = -z;
	return (float4){
	    .x = x,
	    .y = y,
	    .z = z,
	    .w = w,
	};
}

float3
qtrf3(float4 a, float3 v)
{
	float3 q = xyz4(a);
	float3 t = cross3(scale3(q, 2), v);
	return add3(add3(v, scale3(t, a.w)), cross3(q, t));
}

float4
qtrf4(float4 a, float4 v)
{
	float3 u = qtrf3(a, xyz4(v));
	return (float4){.x = u.x, .y = u.y, .z = u.z, .w = v.w};
}

float3
qang(float4 a)
{
	float3 r;

	float sy = 2 * (a.w * a.z + a.x * a.y);
	float cy = 1 - 2 * (a.y * a.y + a.z * a.z);
	r.x = atan2(sy, cy);

	float sp = 2 * (a.w * a.y - a.z * a.x);
	if (fabs(sp) >= 1)
		r.y = copysignf(M_PI / 2, sp);
	else
		r.y = asin(sp);

	float sr = 2 * (a.w * a.x + a.y * a.z);
	float cr = 1 - 2 * (a.x * a.x + a.y * a.y);
	r.z = atan2(sr, cr);

	return r;
}

float
qdist(float4 a, float4 b)
{
	float d;

	d = dot4(a, b);
	return acos(2 * d * d - 1);
}

float4
qsqrt(float4 a)
{
	float r, t;
	float l, z;
	float s, c;
	float4 u;

	u = (float4){.x = a.x, .y = a.y, .z = a.z};
	l = len4(u);
	if (l == 0)
		return (float4){0};

	r = len4(a);
	t = acos(a.w / r);
	z = sqrt(r);

	s = sin(t / 2);
	c = cos(t / 2);

	return (float4){
	    .x = z * s * u.x / l,
	    .y = z * s * u.y / l,
	    .z = z * s * u.z / l,
	    .w = z * c,
	};
}

float4
qpowu(float4 a, float p)
{
	return qexp(scale4(qlog(a), p));
}

float4
qpow(float4 a, float p)
{
	float l;

	l = pow(len4(a), p);
	return scale4(qpowu(a, p), l);
}

float4
qexp(float4 a)
{
	float3 v;
	float l, s, c;

	l = len4(a);
	if (l == 0)
		return (float4){.x = 0, .y = 0, .z = 0, .w = 1};
	v = scale3(xyz4(a), 1 / l);

	s = sin(l);
	c = cos(l);

	return (float4){
	    .x = v.x * s,
	    .y = v.y * s,
	    .z = v.z * s,
	    .w = expf(a.w) * c,
	};
}

float4
qlog(float4 a)
{
	float q, v, c;

	q = len4(a);
	v = len3(xyz4(a));
	if (q == 0 || v == 0)
		return (float4){.x = 0, .y = 0, .z = 0, .w = 0};
	c = acos(a.w / q) / v;

	return (float4){
	    .x = c * a.x,
	    .y = c * a.y,
	    .z = c * a.z,
	    .w = logf(q),
	};
}

float4
qlerp(float t, float4 a, float4 b)
{
	return add4(a, scale4(sub4(b, a), t));
}

float4
qslerp(float t, float4 a, float4 b)
{
	static const float eps = 0.9995;
	float d, r, r0;
	float4 c;

	d = dot4(a, b);
	if (d > eps)
		return normalize4(qlerp(t, a, b));

	d = clampf(d, -1, 1);
	r0 = acos(d);
	r = r0 * t;

	c = sub4(b, scale4(a, d));
	c = normalize4(c);
	return add4(scale4(a, cos(r)), scale4(c, sin(r)));
}

void
qmat4x4(float4x4 m, float4 a)
{
	float x2 = a.x * a.x;
	float y2 = a.y * a.y;
	float z2 = a.z * a.z;
	float xy = a.x * a.y;
	float xz = a.x * a.z;
	float yz = a.y * a.z;
	float wx = a.w * a.x;
	float wy = a.w * a.y;
	float wz = a.w * a.z;
	memset(m, 0, sizeof(float4x4));
	m[0][0] = 1.0 - 2.0 * (y2 + z2);
	m[0][1] = 2.0 * (xy - wz);
	m[0][2] = 2.0 * (xz + wy);
	m[1][0] = 2.0 * (xy + wz);
	m[1][1] = 1.0 - 2.0 * (x2 + z2);
	m[1][2] = 2.0 * (yz - wx);
	m[2][0] = 2.0 * (xz - wy);
	m[2][1] = 2.0 * (yz + wx);
	m[2][2] = 1.0 - 2.0 * (x2 + y2);
	m[3][3] = 1;
}

void
setrow2x2(float2x2 m, int r, float2 a)
{
	m[r][0] = a.x;
	m[r][1] = a.y;
}

void
setcol2x2(float2x2 m, int c, float2 a)
{
	m[0][c] = a.x;
	m[1][c] = a.y;
}

float2
row2x2(float2x2 m, int r)
{
	return (float2){
	    .x = m[r][0],
	    .y = m[r][1],
	};
}

float2
col2x2(float2x2 m, int c)
{
	return (float2){
	    .x = m[0][c],
	    .y = m[1][c],
	};
}

void
add2x2(float2x2 m, float2x2 a, float2x2 b)
{
	size_t i, j;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++)
			m[i][j] = a[i][j] + b[i][j];
	}
}

void
sub2x2(float2x2 m, float2x2 a, float2x2 b)
{
	size_t i, j;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++)
			m[i][j] = a[i][j] - b[i][j];
	}
}

void
hadamard2x2(float2x2 m, float2x2 a, float2x2 b)
{
	size_t i, j;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++)
			m[i][j] = a[i][j] * b[i][j];
	}
}

void
mul2x1(float2x2 m, float2x2 a, float s)
{
	size_t i, j;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++)
			m[i][j] = a[i][j] * s;
	}
}

void
mul2x2(float2x2 m, float2x2 a, float2x2 b)
{
	float2x2 c;
	size_t i, j, k;

	memset(c, 0, sizeof(c));
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			for (k = 0; k < 2; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	}
	memcpy(m, c, sizeof(c));
}

void
eye2x2(float2x2 m)
{
	memset(m, 0, sizeof(float2x2));
	m[0][0] = 1;
	m[1][1] = 1;
}

void
scale2x2(float2x2 m, float2 a)
{
	memset(m, 0, sizeof(float2x2));
	m[0][0] = a.x;
	m[1][1] = a.y;
}

void
rotate2x2(float2x2 m, float r)
{
	float s, c;

	s = sin(r);
	c = cos(r);
	m[0][0] = c;
	m[0][1] = -s;
	m[1][0] = s;
	m[1][1] = c;
}

float
det2x2(float2x2 m)
{
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

void
adj2x2(float2x2 a, float2x2 m)
{
	float m00 = m[0][0];
	float m01 = m[0][1];
	float m10 = m[1][0];
	float m11 = m[1][1];
	a[0][0] = m11;
	a[0][1] = -m01;
	a[1][0] = -m10;
	a[1][1] = m00;
}

float
inv2x2(float2x2 a, float2x2 m)
{
	size_t i, j;
	float d;

	adj2x2(a, m);
	d = det2x2(m);
	if (d == 0)
		return d;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++)
			a[i][j] /= d;
	}
	return d;
}

void
ipow2x2(float2x2 a, float2x2 m, int p)
{
	float2x2 x, y;

	if (p == 0) {
		eye2x2(a);
		return;
	}

	memcpy(x, m, sizeof(x));
	if (p < 0) {
		p = -p;
		inv2x2(x, x);
	}
	while ((p & 1) == 0) {
		mul2x2(x, x, x);
		p >>= 1;
	}
	memcpy(y, x, sizeof(x));
	for (p >>= 1; p > 0; p >>= 1) {
		mul2x2(x, x, x);
		if ((p & 1) != 0)
			mul2x2(y, y, x);
	}
	memcpy(a, y, sizeof(y));
}

void
transpose2x2(float2x2 a, float2x2 m)
{
	size_t i, j;
	float t;

	memmove(a, m, sizeof(float2x2));
	for (i = 0; i < 2; i++) {
		for (j = i + 1; j < 2; j++) {
			t = a[j][i];
			a[j][i] = a[i][j];
			a[i][j] = t;
		}
	}
}

float
trace2x2(float2x2 a)
{
	return a[0][0] + a[1][1];
}

float2
trf2x2(float2x2 m, float2 a)
{
	return (float2){
	    .x = m[0][0] * a.x + m[0][1] * a.y,
	    .y = m[1][0] * a.x + m[1][1] * a.y,
	};
}

float2
trb2x2(float2 a, float2x2 m)
{
	return (float2){
	    .x = a.x * m[0][0] + a.y * m[1][0],
	    .y = a.x * m[0][1] + a.y * m[1][1],
	};
}

void
decompose2x2(float2x2 m, float2x2 r, float2x2 s)
{
	float si, co;
	float t;

	t = atan2(m[1][0] - m[0][1], m[0][0] + m[1][1]);
	si = sin(t);
	co = cos(t);

	r[0][0] = co;
	r[0][1] = -si;
	r[1][0] = si;
	r[1][1] = co;

	s[0][0] = m[0][0] * r[0][0] + m[1][0] * r[1][0];
	s[1][0] = m[0][1] * r[0][0] + m[1][1] * r[1][0];
	s[0][1] = m[0][0] * r[0][1] + m[1][0] * r[1][1];
	s[1][1] = m[0][1] * r[0][1] + m[1][1] * r[1][1];
}

void
setrow3x3(float3x3 m, int r, float3 a)
{
	m[r][0] = a.x;
	m[r][1] = a.y;
	m[r][2] = a.z;
}

void
setcol3x3(float3x3 m, int c, float3 a)
{
	m[0][c] = a.x;
	m[1][c] = a.y;
	m[2][c] = a.z;
}

float3
row3x3(float3x3 m, int r)
{
	return (float3){
	    .x = m[r][0],
	    .y = m[r][1],
	    .z = m[r][2],
	};
}

float3
col3x3(float3x3 m, int c)
{
	return (float3){
	    .x = m[0][c],
	    .y = m[1][c],
	    .z = m[2][c],
	};
}

void
add3x3(float3x3 m, float3x3 a, float3x3 b)
{
	size_t i, j;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++)
			m[i][j] = a[i][j] + b[i][j];
	}
}

void
sub3x3(float3x3 m, float3x3 a, float3x3 b)
{
	size_t i, j;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++)
			m[i][j] = a[i][j] - b[i][j];
	}
}

void
hadamard3x3(float3x3 m, float3x3 a, float3x3 b)
{
	size_t i, j;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++)
			m[i][j] = a[i][j] * b[i][j];
	}
}

void
mul3x1(float3x3 m, float3x3 a, float s)
{
	size_t i, j;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++)
			m[i][j] = a[i][j] * s;
	}
}

void
mul3x3(float3x3 m, float3x3 a, float3x3 b)
{
	float3x3 c;
	size_t i, j, k;

	memset(c, 0, sizeof(c));
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			for (k = 0; k < 3; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	}
	memcpy(m, c, sizeof(c));
}

void
eye3x3(float3x3 m)
{
	memset(m, 0, sizeof(float3x3));
	m[0][0] = 1;
	m[1][1] = 1;
	m[2][2] = 1;
}

void
scale3x2(float3x3 m, float2 a)
{
	memset(m, 0, sizeof(float3x3));
	m[0][0] = a.x;
	m[1][1] = a.y;
	m[2][2] = 1;
}

void
rotate3x2(float3x3 m, float r)
{
	float s, c;

	s = sin(r);
	c = cos(r);
	memset(m, 0, sizeof(float3x3));
	m[0][0] = c;
	m[0][1] = -s;
	m[1][0] = s;
	m[1][1] = c;
	m[2][2] = 1;
}

void
translate3x2(float3x3 m, float2 a)
{
	eye3x3(m);
	m[0][2] = a.x;
	m[1][2] = a.y;
}

void
adj3x3(float3x3 madj, float3x3 m)
{
	float m00, m01, m02, m10, m11, m12, m20, m21, m22;

	m00 = m[0][0];
	m01 = m[0][1];
	m02 = m[0][2];
	m10 = m[1][0];
	m11 = m[1][1];
	m12 = m[1][2];
	m20 = m[2][0];
	m21 = m[2][1];
	m22 = m[2][2];
	madj[0][0] = m11 * m22 - m12 * m21;
	madj[0][1] = m02 * m21 - m01 * m22;
	madj[0][2] = m01 * m12 - m02 * m11;
	madj[1][0] = m12 * m20 - m10 * m22;
	madj[1][1] = m00 * m22 - m02 * m20;
	madj[1][2] = m02 * m10 - m00 * m12;
	madj[2][0] = m10 * m21 - m11 * m20;
	madj[2][1] = m01 * m20 - m00 * m21;
	madj[2][2] = m00 * m11 - m01 * m10;
}

float
det3x3(float3x3 m)
{
	float d;             /* determinant */
	float c00, c01, c02; /* cofactors of first row */
	float m00, m01, m02, m10, m11, m12, m20, m21, m22;

	m00 = m[0][0];
	m01 = m[0][1];
	m02 = m[0][2];
	m10 = m[1][0];
	m11 = m[1][1];
	m12 = m[1][2];
	m20 = m[2][0];
	m21 = m[2][1];
	m22 = m[2][2];
	c00 = m11 * m22 - m12 * m21;
	c01 = m12 * m20 - m10 * m22;
	c02 = m10 * m21 - m11 * m20;
	d = m00 * c00 + m01 * c01 + m02 * c02;
	return d;
}

float
inv3x3(float3x3 minv, float3x3 m)
{
	float d;             /* determinant */
	float c00, c01, c02; /* cofactors of first row */
	float m00, m01, m02, m10, m11, m12, m20, m21, m22;

	m00 = m[0][0];
	m01 = m[0][1];
	m02 = m[0][2];
	m10 = m[1][0];
	m11 = m[1][1];
	m12 = m[1][2];
	m20 = m[2][0];
	m21 = m[2][1];
	m22 = m[2][2];
	c00 = m11 * m22 - m12 * m21;
	c01 = m12 * m20 - m10 * m22;
	c02 = m10 * m21 - m11 * m20;
	d = m00 * c00 + m01 * c01 + m02 * c02;
	if (d == 0.)
		return 0.;
	minv[0][0] = c00 / d;
	minv[1][0] = c01 / d;
	minv[2][0] = c02 / d;
	minv[0][1] = (m02 * m21 - m01 * m22) / d;
	minv[1][1] = (m00 * m22 - m02 * m20) / d;
	minv[2][1] = (m01 * m20 - m00 * m21) / d;
	minv[0][2] = (m01 * m12 - m02 * m11) / d;
	minv[1][2] = (m02 * m10 - m00 * m12) / d;
	minv[2][2] = (m00 * m11 - m01 * m10) / d;
	return d;
}

void
ipow3x3(float3x3 a, float3x3 m, int p)
{
	float3x3 x, y;

	if (p == 0) {
		eye3x3(a);
		return;
	}

	memcpy(x, m, sizeof(x));
	if (p < 0) {
		p = -p;
		inv3x3(x, x);
	}
	while ((p & 1) == 0) {
		mul3x3(x, x, x);
		p >>= 1;
	}
	memcpy(y, x, sizeof(x));
	for (p >>= 1; p > 0; p >>= 1) {
		mul3x3(x, x, x);
		if ((p & 1) != 0)
			mul3x3(y, y, x);
	}
	memcpy(a, y, sizeof(y));
}

void
transpose3x3(float3x3 a, float3x3 m)
{
	size_t i, j;
	float t;

	memmove(a, m, sizeof(float3x3));
	for (i = 0; i < 3; i++) {
		for (j = i + 1; j < 3; j++) {
			t = a[j][i];
			a[j][i] = a[i][j];
			a[i][j] = t;
		}
	}
}

float
trace3x3(float3x3 a)
{
	return a[0][0] + a[1][1] + a[2][2];
}

float2
trf3x2(float3x3 m, float2 a)
{
	return (float2){
	    .x = m[0][0] * a.x + m[0][1] * a.y + m[0][2],
	    .y = m[1][0] * a.x + m[1][1] * a.y + m[1][2],
	};
}

float3
trf3x3(float3x3 m, float3 a)
{
	return (float3){
	    .x = m[0][0] * a.x + m[0][1] * a.y + m[0][2] * a.z,
	    .y = m[1][0] * a.x + m[1][1] * a.y + m[1][2] * a.z,
	    .z = m[2][0] * a.x + m[2][1] * a.y + m[2][2] * a.z,
	};
}

float2
trb3x2(float2 a, float3x3 m)
{
	return (float2){
	    .x = a.x * m[0][0] + a.y * m[1][0] + m[2][0],
	    .y = a.x * m[0][1] + a.y * m[1][1] + m[2][1],
	};
}

float3
trb3x3(float3 a, float3x3 m)
{
	return (float3){
	    .x = a.x * m[0][0] + a.y * m[1][0] + a.z * m[2][0],
	    .y = a.x * m[0][1] + a.y * m[1][1] + a.z * m[2][1],
	    .z = a.x * m[0][2] + a.y * m[1][2] + a.z * m[2][2],
	};
}

void
setrow4x4(float4x4 m, int r, float4 a)
{
	m[r][0] = a.x;
	m[r][1] = a.y;
	m[r][2] = a.z;
	m[r][3] = a.w;
}

void
setcol4x4(float4x4 m, int c, float4 a)
{
	m[0][c] = a.x;
	m[1][c] = a.y;
	m[2][c] = a.z;
	m[3][c] = a.w;
}

float4
row4x4(float4x4 m, int r)
{
	return (float4){
	    .x = m[r][0],
	    .y = m[r][1],
	    .z = m[r][2],
	    .w = m[r][3],
	};
}

float4
col4x4(float4x4 m, int c)
{
	return (float4){
	    .x = m[0][c],
	    .y = m[1][c],
	    .z = m[2][c],
	    .w = m[3][c],
	};
}

void
add4x4(float4x4 m, float4x4 a, float4x4 b)
{
	size_t i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			m[i][j] = a[i][j] + b[i][j];
	}
}

void
sub4x4(float4x4 m, float4x4 a, float4x4 b)
{
	size_t i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			m[i][j] = a[i][j] - b[i][j];
	}
}

void
hadamard4x4(float4x4 m, float4x4 a, float4x4 b)
{
	size_t i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			m[i][j] = a[i][j] * b[i][j];
	}
}

void
mul4x1(float4x4 m, float4x4 a, float s)
{
	size_t i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			m[i][j] = a[i][j] * s;
	}
}

void
mul4x4(float4x4 m, float4x4 a, float4x4 b)
{
	float4x4 c;
	size_t i, j, k;

	memset(c, 0, sizeof(c));
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 4; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	}
	memcpy(m, c, sizeof(c));
}

void
eye4x4(float4x4 m)
{
	size_t i;

	memset(m, 0, sizeof(float4x4));
	for (i = 0; i < 4; i++)
		m[i][i] = 1;
}

void
translate4x3(float4x4 m, float3 a)
{
	eye4x4(m);
	m[0][3] = a.x;
	m[1][3] = a.y;
	m[2][3] = a.z;
}

void
scale4x3(float4x4 m, float3 a)
{
	memset(m, 0, sizeof(float4x4));
	m[0][0] = a.x;
	m[1][1] = a.y;
	m[2][2] = a.z;
	m[3][3] = 1;
}

void
rotate4x3(float4x4 m, float3 a, float r)
{
	float3 w;
	float c, s;

	c = cos(r);
	s = sin(r);
	w = normalize3(a);
	memset(m, 0, sizeof(float4x4));
	m[0][0] = c + w.x * w.x * (1 - c);
	m[0][1] = w.x * w.y * (1 - c) - w.z * s;
	m[0][2] = w.y * s + w.x * w.z * (1 - c);
	m[1][0] = w.z * s + w.x * w.y * (1 - c);
	m[1][1] = c + w.y * w.y * (1 - c);
	m[1][2] = -w.x * s + w.y * w.z * (1 - c);
	m[2][0] = -w.y * s + w.x * w.z * (1 - c);
	m[2][1] = w.x * s + w.y * w.z * (1 - c);
	m[2][2] = c + w.z * w.z * (1 - c);
	m[3][3] = 1;
}

void
shear4x3(float4x4 m, int t, float3 a)
{
	eye4x4(m);
	switch (t) {
	case 0:
	case 'x':
	case 'X':
		m[1][0] = a.y;
		m[2][0] = a.z;
		break;

	case 1:
	case 'y':
	case 'Y':
		m[0][1] = a.x;
		m[2][1] = a.z;
		break;

	case 2:
	case 'z':
	case 'Z':
		m[0][2] = a.x;
		m[1][2] = a.y;
		break;
	}
}

void
lookat4x4(float4x4 m, float3 e, float3 c, float3 up)
{
	float3 f, u, s;

	f = normalize3(sub3(c, e));
	up = normalize3(up);
	s = normalize3(cross3(f, up));
	u = cross3(s, f);

	memset(m, 0, sizeof(float4x4));
	m[0][0] = s.x;
	m[0][1] = s.y;
	m[0][2] = s.z;
	m[1][0] = u.x;
	m[1][1] = u.y;
	m[1][2] = u.z;
	m[2][0] = -f.x;
	m[2][1] = -f.y;
	m[2][2] = -f.z;
	m[3][3] = 1;

	m[0][3] = -e.x * s.x - e.y * s.y - e.z * s.z;
	m[1][3] = -e.x * u.x - e.y * u.y - e.z * u.z;
	m[2][3] = e.x * f.x + e.y * f.y + e.z * f.z;
}

void
ortho4x4(float4x4 m, float l, float r, float b, float t, float n, float f)
{
	memset(m, 0, sizeof(float4x4));
	m[0][0] = 2 / (r - l);
	m[1][1] = 2 / (t - b);
	m[2][2] = -2 / (f - n);
	m[0][3] = -(r + l) / (r - l);
	m[1][3] = -(t + b) / (t - b);
	m[2][3] = -(f + n) / (f - n);
	m[3][3] = 1;
}

void
persp4x4(float4x4 m, float fv, float a, float zn, float zf)
{
	float f;

	f = 1 / tan(fv);
	memset(m, 0, sizeof(float4x4));
	m[0][0] = f / a;
	m[1][1] = a;
	m[2][2] = (zf + zn) / (zn - zf);
	m[2][3] = (2 * zf * zn) / (zn - zf);
	m[3][2] = -1;
}

void
frustum4x4(float4x4 m, float l, float r, float b, float t, float n, float f)
{
	memset(m, 0, sizeof(float4x4));
	m[0][0] = (2 * n) / (r - l);
	m[1][1] = (2 * n) / (t - b);
	m[0][2] = (r + l) / (r - l);
	m[1][2] = (t + b) / (t - b);
	m[2][2] = (f + n) / (f - n);
	m[2][3] = (2 * f * n) / (f - n);
	m[3][2] = -1;
}

void
transpose4x4(float4x4 a, float4x4 m)
{
	size_t i, j;
	float t;

	memmove(a, m, sizeof(float4x4));
	for (i = 0; i < 4; i++) {
		for (j = i + 1; j < 4; j++) {
			t = a[j][i];
			a[j][i] = a[i][j];
			a[i][j] = t;
		}
	}
}

float
trace4x4(float4x4 a)
{
	return a[0][0] + a[1][1] + a[2][2] + a[3][3];
}

float
det4x4(float4x4 m)
{
	return m[0][3] * m[1][2] * m[2][1] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0] -
	       m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][1] * m[1][3] * m[2][2] * m[3][0] +
	       m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][1] * m[1][2] * m[2][3] * m[3][0] -
	       m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][2] * m[1][3] * m[2][0] * m[3][1] +
	       m[0][3] * m[1][0] * m[2][2] * m[3][1] - m[0][0] * m[1][3] * m[2][2] * m[3][1] -
	       m[0][2] * m[1][0] * m[2][3] * m[3][1] + m[0][0] * m[1][2] * m[2][3] * m[3][1] +
	       m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][1] * m[1][3] * m[2][0] * m[3][2] -
	       m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][0] * m[1][3] * m[2][1] * m[3][2] +
	       m[0][1] * m[1][0] * m[2][3] * m[3][2] - m[0][0] * m[1][1] * m[2][3] * m[3][2] -
	       m[0][2] * m[1][1] * m[2][0] * m[3][3] + m[0][1] * m[1][2] * m[2][0] * m[3][3] +
	       m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][0] * m[1][2] * m[2][1] * m[3][3] -
	       m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][0] * m[1][1] * m[2][2] * m[3][3];
}

void
adj4x4(float4x4 a, float4x4 m)
{
	float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
	float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
	float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
	float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];
	a[0][0] = m11 * (m22 * m33 - m23 * m32) + m21 * (m13 * m32 - m12 * m33) + m31 * (m12 * m23 - m13 * m22);
	a[0][1] = m01 * (m23 * m32 - m22 * m33) + m21 * (m02 * m33 - m03 * m32) + m31 * (m03 * m22 - m02 * m23);
	a[0][2] = m01 * (m12 * m33 - m13 * m32) + m11 * (m03 * m32 - m02 * m33) + m31 * (m02 * m13 - m03 * m12);
	a[0][3] = m01 * (m13 * m22 - m12 * m23) + m11 * (m02 * m23 - m03 * m22) + m21 * (m03 * m12 - m02 * m13);
	a[1][0] = m10 * (m23 * m32 - m22 * m33) + m20 * (m12 * m33 - m13 * m32) + m30 * (m13 * m22 - m12 * m23);
	a[1][1] = m00 * (m22 * m33 - m23 * m32) + m20 * (m03 * m32 - m02 * m33) + m30 * (m02 * m23 - m03 * m22);
	a[1][2] = m00 * (m13 * m32 - m12 * m33) + m10 * (m02 * m33 - m03 * m32) + m30 * (m03 * m12 - m02 * m13);
	a[1][3] = m00 * (m12 * m23 - m13 * m22) + m10 * (m03 * m22 - m02 * m23) + m20 * (m02 * m13 - m03 * m12);
	a[2][0] = m10 * (m21 * m33 - m23 * m31) + m20 * (m13 * m31 - m11 * m33) + m30 * (m11 * m23 - m13 * m21);
	a[2][1] = m00 * (m23 * m31 - m21 * m33) + m20 * (m01 * m33 - m03 * m31) + m30 * (m03 * m21 - m01 * m23);
	a[2][2] = m00 * (m11 * m33 - m13 * m31) + m10 * (m03 * m31 - m01 * m33) + m30 * (m01 * m13 - m03 * m11);
	a[2][3] = m00 * (m13 * m21 - m11 * m23) + m10 * (m01 * m23 - m03 * m21) + m20 * (m03 * m11 - m01 * m13);
	a[3][0] = m10 * (m22 * m31 - m21 * m32) + m20 * (m11 * m32 - m12 * m31) + m30 * (m12 * m21 - m11 * m22);
	a[3][1] = m00 * (m21 * m32 - m22 * m31) + m20 * (m02 * m31 - m01 * m32) + m30 * (m01 * m22 - m02 * m21);
	a[3][2] = m00 * (m12 * m31 - m11 * m32) + m10 * (m01 * m32 - m02 * m31) + m30 * (m02 * m11 - m01 * m12);
	a[3][3] = m00 * (m11 * m22 - m12 * m21) + m10 * (m02 * m21 - m01 * m22) + m20 * (m01 * m12 - m02 * m11);
}

float
inv4x4(float4x4 a, float4x4 m)
{
	size_t i, j;
	float d;

	adj4x4(a, m);
	d = det4x4(m);
	if (d == 0)
		return d;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			a[i][j] /= d;
	}
	return d;
}

void
ipow4x4(float4x4 a, float4x4 m, int p)
{
	float4x4 x, y;

	if (p == 0) {
		eye4x4(a);
		return;
	}

	memcpy(x, m, sizeof(x));
	if (p < 0) {
		p = -p;
		inv4x4(x, x);
	}
	while ((p & 1) == 0) {
		mul4x4(x, x, x);
		p >>= 1;
	}
	memcpy(y, x, sizeof(x));
	for (p >>= 1; p > 0; p >>= 1) {
		mul4x4(x, x, x);
		if ((p & 1) != 0)
			mul4x4(y, y, x);
	}
	memcpy(a, y, sizeof(y));
}

float3
trf4x3(float4x4 m, float3 a)
{
	return (float3){
	    .x = m[0][0] * a.x + m[0][1] * a.y + m[0][2] * a.z + m[0][3],
	    .y = m[1][0] * a.x + m[1][1] * a.y + m[1][2] * a.z + m[1][3],
	    .z = m[2][0] * a.x + m[2][1] * a.y + m[2][2] * a.z + m[2][3],
	};
}

float4
trf4x4(float4x4 m, float4 a)
{
	return (float4){
	    .x = m[0][0] * a.x + m[0][1] * a.y + m[0][2] * a.z + m[0][3] * a.w,
	    .y = m[1][0] * a.x + m[1][1] * a.y + m[1][2] * a.z + m[1][3] * a.w,
	    .z = m[2][0] * a.x + m[2][1] * a.y + m[2][2] * a.z + m[2][3] * a.w,
	    .w = m[3][0] * a.x + m[3][1] * a.y + m[3][2] * a.z + m[3][3] * a.w,
	};
}

float3
trb4x3(float3 a, float4x4 m)
{
	return (float3){
	    .x = a.x * m[0][0] + a.y * m[1][0] + a.z * m[2][0] + m[3][0],
	    .y = a.x * m[0][1] + a.y * m[1][1] + a.z * m[2][1] + m[3][1],
	    .z = a.x * m[0][2] + a.y * m[1][2] + a.z * m[2][2] + m[3][2],
	};
}

float4
trb4x4(float4 a, float4x4 m)
{
	return (float4){
	    .x = a.x * m[0][0] + a.y * m[1][0] + a.z * m[2][0] + a.w * m[3][0],
	    .y = a.x * m[0][1] + a.y * m[1][1] + a.z * m[2][1] + a.w * m[3][1],
	    .z = a.x * m[0][2] + a.y * m[1][2] + a.z * m[2][2] + a.w * m[3][2],
	    .w = a.x * m[0][3] + a.y * m[1][3] + a.z * m[2][3] + a.w * m[3][3],
	};
}

void
mfdump(void *p, size_t n, size_t m)
{
	size_t i, j;
	float *f;

	f = p;
	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++)
			printf("%.6f ", *f++);
		printf("\n");
	}
	printf("\n");
}

int
mfclose(void *p, void *q, size_t n, size_t m, float eps)
{
	size_t i, j;
	float *f, *g;

	f = p;
	g = q;
	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			if (!(fabs(*f - *g) < eps))
				return 0;
			f++;
			g++;
		}
	}
	return 1;
}

float
saturatef(float x)
{
	return clampf(x, 0, 1);
}

float
clampf(float x, float a, float b)
{
	return min(max(x, a), b);
}

float
signf(float x)
{
	if (x == 0)
		return 0;
	if (x < 0)
		return -1;
	return 1;
}

float
lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float
unlerp(float t, float a, float b)
{
	return (t - a) / (b - a);
}

float
linear_remap(float x, float a, float b, float c, float d)
{
	return lerp(unlerp(x, a, b), c, d);
}

float
smoothstepf(float x0, float x1, float x)
{
	float t = clampf((x - x0) / (x1 - x0), 0.0, 1.0);
	return t * t * (3 - 2 * t);
}

float
stepf(float x0, float x)
{
	return (x < x0) ? 0 : 1;
}

void
swapf(float *x, float *y)
{
	float t;

	t = *y;
	*y = *x;
	*x = t;
}

float
wrapf(float x, float a, float b)
{
	if (x > b)
		x = a;
	else if (x < a)
		x = b;
	return x;
}

float
sincf(float x)
{
	if (x == 0)
		return 0;
	return sin(x) / x;
}

float
gmodf(float a, float b)
{
	float r;

	if (a >= 0)
		return fmod(a, b);

	r = fmod(-a, b);
	return (r == 0) ? r : b - r;
}

static int
approxf(float a, float b)
{
	static const float eps = 1e-8;
	return fabs(a - b) < eps;
}

static float
ncbrtf(float x)
{
	if (x >= 0)
		return cbrt(x);
	return -cbrt(-x);
}

int
roots1rf(float a, float b, float z[1])
{
	if (approxf(a, 0)) {
		if (approxf(b, 0)) {
			z[0] = 0;
			return 1;
		}
		return 0;
	}
	z[0] = -b / a;
	return 1;
}

int
roots2rf(float a, float b, float c, float z[2])
{
	float d, sd;

	if (approxf(a, 0))
		return roots1rf(b, c, z);

	d = b * b - 4 * a * c;
	if (d < 0)
		return 0;

	sd = sqrt(d);
	z[0] = (-b + sd) / (2 * a);
	z[1] = (-b - sd) / (2 * a);
	if (approxf(d, 0))
		return 1;
	return 2;
}

int
roots3rf(float pa, float pb, float pc, float pd, float z[3])
{
	static const float eps = 1e-8;

	if (approxf(pa, 0))
		return roots2rf(pb, pc, pd, z);

	float a = pb / pa;
	float b = pc / pa;
	float c = pd / pa;

	float a2 = a * a;
	float q = (a2 - 3 * b) / 9;
	float r = (a * (2 * a2 - 9 * b) + 27 * c) / 54;

	float r2 = r * r;
	float q3 = q * q * q;
	if (r2 <= (q3 + eps)) {
		float t = r / sqrt(q3);
		if (t < -1)
			t = -1;
		if (t > 1)
			t = 1;
		t = acos(t);
		a /= 3;
		q = -2 * sqrt(q);
		z[0] = q * cos(t / 3) - a;
		z[1] = q * cos((t + M_TAU) / 3) - a;
		z[2] = q * cos((t - M_TAU) / 3) - a;
		return 3;
	}

	float A = -ncbrtf(fabs(r) + sqrt(r2 - q3));
	if (r < 0)
		A = -A;
	float B = 0;
	if (A != 0)
		B = q / A;

	a /= 3;
	z[0] = (A + B) - a;
	z[1] = -0.5 * (A + B) - a;
	z[2] = 0.5 * M_SQRT3 * (A - B);
	if (fabs(z[2]) < eps) {
		z[2] = z[1];
		return 2;
	}
	return 1;
}
