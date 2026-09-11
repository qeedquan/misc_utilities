#ifndef _LINALG_H_
#define _LINALG_H_

#ifndef M_PI
#define M_E 2.7182818284590452354
#define M_LOG2E 1.4426950408889634074
#define M_LN2 0.69314718055994530942
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_SQRT2 1.41421356237309504880
#define M_SQRT1_2 0.70710678118654752440
#endif

#define M_TAU (2 * M_PI)
#define M_SQRT3 1.73205080756887729352
#define M_SQRT5 2.23606797749978969640
#define M_PHI 1.61803398874989484820
#define M_SQRTPHI 1.27201964951406896425
#define M_SQRTE 1.64872127070012814684
#define M_SQRTPI 1.77245385090551602729

#define S2(s) ((float2){{s, s}})
#define S3(s) ((float3){{s, s, s}})
#define S4(s) ((float4){{s, s, s, s}})

#define V2(...) ((float2){{.x = __VA_ARGS__}})
#define V3(...) ((float3){{.x = __VA_ARGS__}})
#define V4(...) ((float4){{.x = __VA_ARGS__}})

typedef union {
	struct {
		float x, y;
	};
	struct {
		float u, v;
	};
	struct {
		float r, t;
	};
	float e[2];
} float2;

typedef union {
	struct {
		float x, y, z;
	};
	struct {
		float u, v, s;
	};
	struct {
		float r, g, b;
	};
	struct {
		float rd, t, p;
	};
	float e[3];
} float3;

typedef union {
	struct {
		float x, y, z, w;
	};
	struct {
		float u, v, s, t;
	};
	struct {
		float r, g, b, a;
	};
	struct {
		float2 min, max;
	};
	float e[4];
} float4;

typedef float float2x2[2][2];
typedef float float3x3[3][3];
typedef float float4x4[4][4];

#define deg2rad(x) ((x) * M_PI / 180)
#define rad2deg(x) ((x) * 180 / M_PI)

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

float2 xyplr(float2);
float2 plrxy(float2);
float3 xyz2(float2);
float2 reciprocal2(float2);
float2 add2(float2, float2);
float2 sub2(float2, float2);
float2 scale2(float2, float);
float2 min2(float2, float2);
float2 max2(float2, float2);
float2 abs2(float2);
float mincomp2(float2);
float maxcomp2(float2);
float len2(float2);
float dot2(float2, float2);
float ndot2(float2, float2);
float2 normalize2(float2);
float2 perp2(float2);
float2 proj2(float2, float2);
float2 rej2(float2, float2);
float2 hadamard2(float2, float2);
float2 lerp2(float, float2, float2);
float2 unlerp2(float, float2, float2);
float2 saturate2(float2);
float wedge2(float2, float2);
float2 reflect2(float2, float2);
void outer2(float2x2, float2, float2);
float2 sin2(float2);
float2 cos2(float2);
float2 tan2(float2);
float2 sqrt2(float2);
float2 cbrt2(float2);
float2 pow2(float2, float2);
float2 fma2(float2, float2, float2);
float2 sign2(float2);
float2 clamp2(float2, float2, float2);

float3 xyzsph(float3);
float3 sphxyz(float3);
float3 xyzcyl(float3);
float3 cylxyz(float3);
float2 xy3(float3);
float2 xz3(float3);
float3 yzx3(float3);
float3 zxy3(float3);
float4 xyzw3(float3, float);
float3 reciprocal3(float3);
float3 add3(float3, float3);
float3 sub3(float3, float3);
float3 scale3(float3, float);
float3 min3(float3, float3);
float3 max3(float3, float3);
float3 abs3(float3);
float mincomp3(float3);
float maxcomp3(float3);
float len3(float3);
float dot3(float3, float3);
float ndot3(float3, float3);
float3 cross3(float3, float3);
float3 normalize3(float3);
float3 proj3(float3, float3);
float3 rej3(float3, float3);
float3 hadamard3(float3, float3);
float3 lerp3(float, float3, float3);
float3 unlerp3(float, float3, float3);
float3 saturate3(float3);
float3 wedge3(float3, float3);
float3 reflect3(float3, float3);
void outer3(float3x3, float3, float3);
float3 sin3(float3);
float3 cos3(float3);
float3 tan3(float3);
float3 sqrt3(float3);
float3 cbrt3(float3);
float3 pow3(float3, float3);
float3 fma3(float3, float3, float3);
float3 sign3(float3);
float3 clamp3(float3, float3, float3);

float2 xy4(float4);
float3 xyz4(float4);
float4 reciprocal4(float4);
float4 add4(float4, float4);
float4 sub4(float4, float4);
float4 scale4(float4, float);
float4 min4(float4, float4);
float4 max4(float4, float4);
float4 abs4(float4);
float mincomp4(float4);
float maxcomp4(float4);
float dot4(float4, float4);
float ndot4(float4, float4);
float len4(float4);
float4 normalize4(float4);
float4 hadamard4(float4, float4);
float4 lerp4(float, float4, float4);
float4 unlerp4(float, float4, float4);
float4 saturate4(float4);
void outer4(float4x4, float4, float4);
float4 sin4(float4);
float4 cos4(float4);
float4 tan4(float4);
float4 sqrt4(float4);
float4 cbrt4(float4);
float4 pow4(float4, float4);
float4 fma4(float4, float4, float4);
float4 sign4(float4);
float4 clamp4(float4, float4, float4);

float4 qeye(void);
float4 qmul(float4, float4);
float4 qconj(float4);
float4 qaxis(float3, float);
float4 qeuler(float3);
float4 qrotate4x4(float4x4);
float3 qtrf3(float4, float3);
float4 qtrf4(float4, float4);
float3 qang(float4);
float qdist(float4, float4);
float4 qsqrt(float4);
float4 qpowu(float4, float);
float4 qpow(float4, float);
float4 qexp(float4);
float4 qlog(float4);
float4 qlerp(float, float4, float4);
float4 qslerp(float, float4, float4);
void qmat4x4(float4x4, float4);

void setrow2x2(float2x2, int, float2);
void setcol2x2(float2x2, int, float2);
float2 row2x2(float2x2, int);
float2 col2x2(float2x2, int);
void add2x2(float2x2, float2x2, float2x2);
void sub2x2(float2x2, float2x2, float2x2);
void hadamard2x2(float2x2, float2x2, float2x2);
void mul2x1(float2x2, float2x2, float);
void mul2x2(float2x2, float2x2, float2x2);
void eye2x2(float2x2);
void scale2x2(float2x2, float2);
void rotate2x2(float2x2, float);
float det2x2(float2x2);
void adj2x2(float2x2, float2x2);
float inv2x2(float2x2, float2x2);
void ipow2x2(float2x2, float2x2, int);
void transpose2x2(float2x2, float2x2);
float trace2x2(float2x2);
float2 trf2x2(float2x2, float2);
float2 trb2x2(float2, float2x2);
void decompose2x2(float2x2, float2x2, float2x2);

void setrow3x3(float3x3, int, float3);
void setcol3x3(float3x3, int, float3);
float3 row3x3(float3x3, int);
float3 col3x3(float3x3, int);
void add3x3(float3x3, float3x3, float3x3);
void sub3x3(float3x3, float3x3, float3x3);
void hadamard3x3(float3x3, float3x3, float3x3);
void mul3x1(float3x3, float3x3, float);
void mul3x3(float3x3, float3x3, float3x3);
void eye3x3(float3x3);
void scale3x2(float3x3, float2);
void rotate3x2(float3x3, float);
void translate3x2(float3x3, float2);
float det3x3(float3x3);
void adj3x3(float3x3, float3x3);
float inv3x3(float3x3, float3x3);
void ipow3x3(float3x3, float3x3, int);
void transpose3x3(float3x3, float3x3);
float trace3x3(float3x3);
float2 trf3x2(float3x3, float2);
float3 trf3x3(float3x3, float3);
float2 trb3x2(float2, float3x3);
float3 trb3x3(float3, float3x3);

void setrow4x4(float4x4, int, float4);
void setcol4x4(float4x4, int, float4);
float4 row4x4(float4x4, int);
float4 col4x4(float4x4, int);
void add4x4(float4x4, float4x4, float4x4);
void sub4x4(float4x4, float4x4, float4x4);
void hadamard4x4(float4x4, float4x4, float4x4);
void mul4x1(float4x4, float4x4, float);
void mul4x4(float4x4, float4x4, float4x4);
void eye4x4(float4x4);
void translate4x3(float4x4, float3);
void scale4x3(float4x4, float3);
void rotate4x3(float4x4, float3, float);
void shear4x3(float4x4, int, float3);
void lookat4x4(float4x4, float3, float3, float3);
void ortho4x4(float4x4, float, float, float, float, float, float);
void persp4x4(float4x4, float, float, float, float);
void frustum4x4(float4x4, float, float, float, float, float, float);
void transpose4x4(float4x4, float4x4);
float trace4x4(float4x4);
float det4x4(float4x4);
void adj4x4(float4x4, float4x4);
float inv4x4(float4x4, float4x4);
void ipow4x4(float4x4, float4x4, int);
float3 trf4x3(float4x4, float3);
float4 trf4x4(float4x4, float4);
float3 trb4x3(float3, float4x4);
float4 trb4x4(float4, float4x4);

void mfdump(void *, size_t, size_t);
int mfclose(void *, void *, size_t, size_t, float);

float clampf(float, float, float);
float saturatef(float);
float signf(float);

float lerp(float, float, float);
float unlerp(float, float, float);
float linear_remap(float, float, float, float, float);

float smoothstepf(float, float, float);
float stepf(float, float);

float wrapf(float, float, float);
void swapf(float *, float *);
float sincf(float);

float gmodf(float, float);

int roots1rf(float, float, float[1]);
int roots2rf(float, float, float, float[2]);
int roots3rf(float, float, float, float, float[3]);

#endif
