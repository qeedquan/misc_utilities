#ifndef _SDF_H_
#define _SDF_H_

float sdcircle(float2, float);
float sdroundrect(float2, float2, float4);
float sdrect(float2, float2);
float sdorientedrect(float2, float2, float2, float);
float sdsegment(float2, float2, float2);
float sdrhombus(float2, float2);
float sdequtriangle(float2);
float sdisotriangle(float2, float2);
float sdunevencapsuley(float2, float, float, float);
float sdunevencapsule(float2, float2, float2, float, float);
float sdtriangle(float2, float2, float2, float2);
float sdpie(float2, float2, float);
float sdroundx(float2, float, float);
float sdpentagon(float2, float);
float sdhexagon(float2, float);
float sdoctagon(float2, float);
float sdhexagram(float2, float);
float sdstar5(float2, float, float);
float sdstar(float2, float, int, float);
float sdtrapezoidy(float2, float, float, float);
float sdtrapezoid(float2, float2, float2, float, float);
float sdarc(float2, float2, float2, float, float);
float sdhorseshoe(float2, float2, float, float2);
float sdvesica(float2, float, float);
float sdegg(float2, float, float);
float sdcross(float2, float2, float);
float sdpolygon(float2, float2 *, size_t);
float sdellipse(float2, float2);
float sdparabola(float2, float);
float sdsegparabola(float2, float, float);
float sdbezier2(float2, float2, float2, float2);

float sdsphere(float3, float);
float sdbox(float3, float3);
float sdroundbox(float3, float3, float);
float sdboundingbox(float3, float3, float);
float sdtorus(float3, float2);
float sdcappedtorus(float3, float2, float, float);
float sdlink(float3, float, float, float);
float sdcylinder(float3, float3);
float sdcone(float3, float2, float);
float sdplane(float3, float3, float);
float sdtriprism(float3, float2);
float sdcapsule(float3, float3, float3, float);
float sdverticalcapsule(float3, float, float);
float sdsolidangle(float3, float2, float);
float sdoctahedron(float3, float);
float sdpyramid(float3, float);

#endif
