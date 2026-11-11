#include <stdio.h>
#include <math.h>
#include "linalg.h"
#include "sdf.h"

float
sdcircle(float2 p, float r)
{
	return len2(p) - r;
}

float
sdroundrect(float2 p, float2 b, float4 r)
{
	if (p.x <= 0.0) {
		r.x = r.z;
		r.y = r.w;
	}
	r.x = (p.y > 0.0) ? r.x : r.y;
	float2 q = add2(sub2(abs2(p), b), V2(r.x, r.x));
	return min(max(q.x, q.y), 0.0) + len2(max2(q, V2(0.0, 0.0))) - r.x;
}

float
sdrect(float2 p, float2 b)
{
	float2 d = sub2(abs2(p), b);
	return len2(max2(d, V2(0, 0))) + min(max(d.x, d.y), 0.0);
}

float
sdorientedrect(float2 p, float2 a, float2 b, float th)
{
	float l = len2(sub2(b, a));
	float2 d = scale2(sub2(b, a), 1.0 / l);
	float2 q = sub2(p, scale2(add2(a, b), 0.5));
	float2x2 m = {{d.x, -d.y}, {d.y, d.x}};
	q = trf2x2(m, q);
	q = sub2(abs2(q), scale2(V2(l, th), 0.5));
	return len2(max2(q, V2(0, 0))) + min(max(q.x, q.y), 0.0);
}

float
sdsegment(float2 p, float2 a, float2 b)
{
	float2 pa = sub2(p, a);
	float2 ba = sub2(b, a);
	float h = clampf(dot2(pa, ba) / dot2(ba, ba), 0.0, 1.0);
	return len2(sub2(pa, scale2(ba, h)));
}

float
sdrhombus(float2 p, float2 b)
{
	float2 q = abs2(p);
	float h = clampf((-2.0 * ndot2(q, b) + ndot2(b, b)) / dot2(b, b), -1.0, 1.0);
	float2 c = hadamard2(b, V2(1 - h, 1 + h));
	c = scale2(c, 0.5);
	float d = len2(sub2(q, c));
	return d * signf(q.x * b.y + q.y * b.x - b.x * b.y);
}

float
sdequtriangle(float2 p)
{
	const float k = M_SQRT3;
	p.x = fabsf(p.x) - 1.0;
	p.y = p.y + 1.0 / k;
	if (p.x + k * p.y > 0.0) {
		p = V2(p.x - k * p.y, -k * p.x - p.y);
		p = scale2(p, 0.5);
	}
	p.x -= clampf(p.x, -2.0, 0.0);
	return -len2(p) * signf(p.y);
}

float
sdisotriangle(float2 p, float2 q)
{
	p.x = fabsf(p.x);
	float2 a = sub2(p, scale2(q, clampf(dot2(p, q) / dot2(q, q), 0.0, 1.0)));
	float2 b = sub2(p, hadamard2(q, V2(clampf(p.x / q.x, 0.0, 1.0), 1.0)));
	float s = -signf(q.y);
	float2 d = min2(V2(dot2(a, a), s * (p.x * q.y - p.y * q.x)),
	                V2(dot2(b, b), s * (p.y - q.y)));
	return -sqrtf(d.x) * signf(d.y);
}

float
sdunevencapsuley(float2 p, float r1, float r2, float h)
{
	p.x = fabsf(p.x);
	float b = (r1 - r2) / h;
	float a = sqrtf(1.0 - b * b);
	float k = dot2(p, V2(-b, a));
	if (k < 0.0)
		return len2(p) - r1;
	if (k > a * h)
		return len2(sub2(p, V2(0.0, h))) - r2;
	return dot2(p, V2(a, b)) - r1;
}

float
sdunevencapsule(float2 p, float2 pa, float2 pb, float ra, float rb)
{
	p = sub2(p, pa);
	pb = sub2(pb, pa);
	float h = dot2(pb, pb);
	float2 q = V2(dot2(p, V2(pb.y, -pb.x)), dot2(p, pb));
	q = scale2(q, 1.0 / h);

	q.x = fabsf(q.x);

	float b = ra - rb;
	float2 c = V2(sqrtf(h - b * b), b);

	float k = wedge2(c, q);
	float m = dot2(c, q);
	float n = dot2(q, q);

	if (k < 0.0)
		return sqrtf(h * (n)) - ra;
	else if (k > c.x)
		return sqrtf(h * (n + 1.0 - 2.0 * q.y)) - rb;
	return m - ra;
}

float
sdtriangle(float2 p, float2 p0, float2 p1, float2 p2)
{
	float2 e0 = sub2(p1, p0);
	float2 e1 = sub2(p2, p1);
	float2 e2 = sub2(p0, p2);
	float2 v0 = sub2(p, p0);
	float2 v1 = sub2(p, p1);
	float2 v2 = sub2(p, p2);
	float2 pq0 = sub2(v0, scale2(e0, clampf(dot2(v0, e0) / dot2(e0, e0), 0.0, 1.0)));
	float2 pq1 = sub2(v1, scale2(e1, clampf(dot2(v1, e1) / dot2(e1, e1), 0.0, 1.0)));
	float2 pq2 = sub2(v2, scale2(e2, clampf(dot2(v2, e2) / dot2(e2, e2), 0.0, 1.0)));
	float s = signf(e0.x * e2.y - e0.y * e2.x);
	float2 a = {{dot2(pq0, pq0), s * (v0.x * e0.y - v0.y * e0.x)}};
	float2 b = {{dot2(pq1, pq1), s * (v1.x * e1.y - v1.y * e1.x)}};
	float2 c = {{dot2(pq2, pq2), s * (v2.x * e2.y - v2.y * e2.x)}};
	float2 d = min2(min2(a, b), c);
	return -sqrtf(d.x) * signf(d.y);
}

float
sdpentagon(float2 p, float r)
{
	const float3 k = {{0.809016994, 0.587785252, 0.726542528}};
	p.x = fabsf(p.x);
	p = sub2(p, scale2(V2(-k.x, k.y), 2.0 * min(dot2(V2(-k.x, k.y), p), 0.0)));
	p = sub2(p, scale2(V2(k.x, k.y), 2.0 * min(dot2(V2(k.x, k.y), p), 0.0)));
	p = sub2(p, V2(clampf(p.x, -r * k.z, r * k.z), r));
	return len2(p) * signf(p.y);
}

float
sdhexagon(float2 p, float r)
{
	const float3 k = {{-0.866025404, 0.5, 0.577350269}};
	p = abs2(p);
	p = sub2(p, scale2(xy3(k), 2.0 * min(dot2(xy3(k), p), 0.0)));
	p = sub2(p, V2(clampf(p.x, -k.z * r, k.z * r), r));
	return len2(p) * signf(p.y);
}

float
sdoctagon(float2 p, float r)
{
	const float3 k = {{-0.9238795325, 0.3826834323, 0.4142135623}};
	p = abs2(p);
	p = sub2(p, scale2(xy3(k), 2.0 * min(dot2(xy3(k), p), 0.0)));
	p = sub2(p, scale2(V2(-k.x, k.y), 2.0 * min(dot2(V2(-k.x, k.y), p), 0.0)));
	p = sub2(p, V2(clampf(p.x, -k.z * r, k.z * r), r));
	return len2(p) * signf(p.y);
}

float
sdhexagram(float2 p, float r)
{
	const float4 k = {{-0.5, 0.8660254038, 0.5773502692, 1.7320508076}};
	p = abs2(p);
	p = sub2(p, scale2(V2(k.x, k.y), 2.0 * min(dot2(V2(k.x, k.y), p), 0.0)));
	p = sub2(p, scale2(V2(k.y, k.x), 2.0 * min(dot2(V2(k.y, k.x), p), 0.0)));
	p = sub2(p, V2(clampf(p.x, r * k.z, r * k.w), r));
	return len2(p) * signf(p.y);
}

float
sdstar5(float2 p, float r, float rf)
{
	const float2 k1 = {{0.809016994375, -0.587785252292}};
	const float2 k2 = {{-k1.x, k1.y}};
	p.x = fabsf(p.x);
	p = sub2(p, scale2(k1, 2.0 * max(dot2(k1, p), 0.0)));
	p = sub2(p, scale2(k2, 2.0 * max(dot2(k2, p), 0.0)));
	p.x = fabsf(p.x);
	p.y -= r;
	float2 ba = sub2(scale2(V2(-k1.y, k1.x), rf), V2(0, 1));
	float h = clampf(dot2(p, ba) / dot2(ba, ba), 0.0, r);
	return len2(sub2(p, scale2(ba, h))) * signf(p.y * ba.x - p.x * ba.y);
}

float
sdstar(float2 p, float r, int n, float m)
{
	// next 4 lines can be precomputed for a given shape
	float an = M_PI / n;
	float en = M_PI / m; // m is between 2 and n
	float2 acs = {{cosf(an), sinf(an)}};
	float2 ecs = {{cosf(en), sinf(en)}}; // ecs=vec2(0,1) for regular polygon

	float bn = gmodf(atan2f(p.x, p.y), 2.0 * an) - an;
	p = scale2(V2(cosf(bn), fabsf(sinf(bn))), len2(p));
	p = sub2(p, scale2(acs, r));
	p = add2(p, scale2(ecs, clampf(-dot2(p, ecs), 0.0, r * acs.y / ecs.y)));
	return len2(p) * signf(p.x);
}

float
sdtrapezoidy(float2 p, float r1, float r2, float he)
{
	float2 k1 = {{r2, he}};
	float2 k2 = {{r2 - r1, 2.0 * he}};
	p.x = fabsf(p.x);
	float2 ca = {{p.x - min(p.x, (p.y < 0.0) ? r1 : r2), fabsf(p.y) - he}};
	float2 cb = add2(sub2(p, k1), scale2(k2, clampf(dot2(sub2(k1, p), k2) / dot2(k2, k2), 0.0, 1.0)));
	float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
	return s * sqrtf(min(dot2(ca, ca), dot2(cb, cb)));
}

float
sdtrapezoid(float2 p, float2 a, float2 b, float ra, float rb)
{
	float rba = rb - ra;
	float baba = dot2(sub2(b, a), sub2(b, a));
	float papa = dot2(sub2(p, a), sub2(p, a));
	float paba = dot2(sub2(p, a), sub2(b, a)) / baba;
	float x = sqrtf(papa - paba * paba * baba);
	float cax = max(0.0, x - ((paba < 0.5) ? ra : rb));
	float cay = fabsf(paba - 0.5f) - 0.5;
	float k = rba * rba + baba;
	float f = clampf((rba * (x - ra) + paba * baba) / k, 0.0, 1.0);
	float cbx = x - ra - f * rba;
	float cby = paba - f;
	float s = (cbx < 0.0 && cay < 0.0) ? -1.0 : 1.0;
	return s * sqrtf(min(cax * cax + cay * cay * baba,
	                     cbx * cbx + cby * cby * baba));
}

// c is the sin/cos of the angle. r is the radius
float
sdpie(float2 p, float2 c, float r)
{
	p.x = fabsf(p.x);
	float l = len2(p) - r;
	float m = len2(sub2(p, scale2(c, clampf(dot2(p, c), 0.0, r))));
	return max(l, m * signf(c.y * p.x - c.x * p.y));
}

float
sdhorseshoe(float2 p, float2 c, float r, float2 w)
{
	p.x = fabsf(p.x);
	float l = len2(p);
	float2x2 m = {{-c.x, c.y}, {c.y, c.x}};
	p = trf2x2(m, p);
	p = V2((p.y > 0.0) ? p.x : l * signf(-c.x),
	       (p.x > 0.0) ? p.y : l);
	p = sub2(V2(p.x, fabsf(p.y - r)), w);
	return len2(max2(p, V2(0.0, 0.0))) + min(0.0, max(p.x, p.y));
}

float
sdvesica(float2 p, float r, float d)
{
	p = abs2(p);
	float b = sqrtf(r * r - d * d);
	return ((p.y - b) * d > p.x * b) ? len2(sub2(p, V2(0.0, b))) * signf(d)
	                                 : len2(sub2(p, V2(-d, 0.0))) - r;
}

float
sdegg(float2 p, float ra, float rb)
{
	const float k = M_SQRT3;
	p.x = fabsf(p.x);
	float r = ra - rb;
	float rc;
	if (p.y < 0.0)
		rc = len2(p) - r;
	else if ((k * (p.x + r) < p.y))
		rc = len2(V2(p.x, p.y - k * r));
	else
		rc = len2(V2(p.x + r, p.y)) - 2.0 * r;
	return rc - rb;
}

float
sdroundx(float2 p, float w, float r)
{
	p = abs2(p);
	float u = min(p.x + p.y, w);
	float2 q = {{u * 0.5, u * 0.5}};
	return len2(sub2(p, q)) - r;
}

float
sdarc(float2 p, float2 sca, float2 scb, float ra, float rb)
{
	float3x3 m = {{sca.x, sca.y, 0}, {-sca.y, sca.x, 0}, {0, 0, 1}};
	p = trb3x2(p, m);
	p.x = fabsf(p.x);
	float k = (scb.y * p.x > scb.x * p.y) ? dot2(p, scb) : len2(p);
	return sqrtf(dot2(p, p) + ra * ra - 2.0 * ra * k) - rb;
}

float
sdcross(float2 p, float2 b, float r)
{
	p = abs2(p);
	if (p.y > p.x)
		swapf(&p.x, &p.y);
	float2 q = sub2(p, b);
	float k = max(q.y, q.x);
	float2 w = (k > 0.0) ? q : V2(b.y - p.x, -k);
	return signf(k) * len2(max2(w, V2(0.0, 0.0))) + r;
}

float
sdpolygon(float2 p, float2 *v, size_t n)
{
	if (n == 0)
		return 0;
	float d = dot2(sub2(p, v[0]), sub2(p, v[0]));
	float s = 1.0;
	for (size_t i = 0, j = n - 1; i < n; j = i, i++) {
		float2 e = sub2(v[j], v[i]);
		float2 w = sub2(p, v[i]);
		float2 b = sub2(w, scale2(e, clampf(dot2(w, e) / dot2(e, e), 0.0, 1.0)));
		float bd = dot2(b, b);
		d = min(d, bd);
		int b1 = p.y >= v[i].y;
		int b2 = p.y < v[j].y;
		int b3 = e.x * w.y > e.y * w.x;
		if ((b1 && b2 && b3) || (!b1 && !b2 && !b3))
			s *= -1.0;
	}
	return s * sqrtf(d);
}

float
sdellipse(float2 p, float2 ab)
{
	p = abs2(p);
	if (p.x > p.y) {
		swapf(&p.x, &p.y);
		swapf(&ab.x, &ab.y);
	}
	float l = ab.y * ab.y - ab.x * ab.x;
	float m = ab.x * p.x / l;
	float m2 = m * m;
	float n = ab.y * p.y / l;
	float n2 = n * n;
	float c = (m2 + n2 - 1.0) / 3.0;
	float c3 = c * c * c;
	float q = c3 + m2 * n2 * 2.0;
	float d = c3 + m2 * n2;
	float g = m + m * n2;
	float co;
	if (d < 0.0) {
		float h = acosf(q / c3) / 3.0;
		float s = cosf(h);
		float t = sinf(h) * sqrtf(3.0);
		float rx = sqrtf(-c * (s + t + 2.0) + m2);
		float ry = sqrtf(-c * (s - t + 2.0) + m2);
		co = (ry + signf(l) * rx + fabsf(g) / (rx * ry) - m) / 2.0;
	} else {
		float h = 2.0 * m * n * sqrtf(d);
		float s = signf(q + h) * powf(fabsf(q + h), 1.0 / 3.0);
		float u = signf(q - h) * powf(fabsf(q - h), 1.0 / 3.0);
		float rx = -s - u - c * 4.0 + 2.0 * m2;
		float ry = (s - u) * sqrtf(3.0);
		float rm = sqrtf(rx * rx + ry * ry);
		co = (ry / sqrtf(rm - rx) + 2.0 * g / rm - m) / 2.0;
	}
	float2 r = hadamard2(ab, V2(co, sqrtf(1.0 - co * co)));
	return len2(sub2(r, p)) * signf(p.y - r.y);
}

float
sdparabola(float2 pos, float k)
{
	pos.x = fabsf(pos.x);
	float ik = 1.0 / k;
	float p = ik * (pos.y - 0.5 * ik) / 3.0;
	float q = 0.25 * ik * ik * pos.x;
	float h = q * q - p * p * p;
	float r = sqrtf(fabsf(h));
	float x = (h > 0.0) ? powf(q + r, 1.0 / 3.0) - powf(fabsf(q - r), 1.0 / 3.0) * signf(r - q) : 2.0 * cos(atan2(r, q) / 3.0) * sqrtf(p);
	return len2(sub2(pos, V2(x, k * x * x))) * signf(pos.x - x);
}

float
sdsegparabola(float2 pos, float wi, float he)
{
	pos.x = fabsf(pos.x);
	float ik = wi * wi / he;
	float p = ik * (he - pos.y - 0.5 * ik) / 3.0;
	float q = pos.x * ik * ik * 0.25;
	float h = q * q - p * p * p;
	float r = sqrtf(fabsf(h));
	float x = (h > 0.0) ? powf(q + r, 1.0 / 3.0) - powf(fabsf(q - r), 1.0 / 3.0) * signf(r - q) : 2.0 * cosf(atanf(r / q) / 3.0) * sqrtf(p);
	x = min(x, wi);
	return len2(sub2(pos, V2(x, he - x * x / ik))) *
	       signf(ik * (pos.y - he) + pos.x * pos.x);
}

float
sdbezier2(float2 pos, float2 A, float2 B, float2 C)
{
	float2 a = sub2(B, A);
	float2 b = add2(sub2(A, scale2(B, 2.0)), C);
	float2 c = scale2(a, 2.0);
	float2 d = sub2(A, pos);

	float kk = 1.0 / dot2(b, b);
	float kx = kk * dot2(a, b);
	float ky = kk * (2.0 * dot2(a, a) + dot2(d, b)) / 3.0;
	float kz = kk * dot2(d, a);

	float res = 0.0;
	float sgn = 0.0;

	float p = ky - kx * kx;
	float p3 = p * p * p;
	float q = kx * (2.0 * kx * kx - 3.0 * ky) + kz;
	float h = q * q + 4.0 * p3;

	if (h >= 0.0) { // 1 root
		h = sqrtf(h);
		float2 x = scale2(V2(h - q, -h - q), 0.5);
		float2 uv = hadamard2(pow2(abs2(x), S2(1.0 / 3.0)), V2(signf(x.x), signf(x.y)));
		float t = clampf(uv.x + uv.y - kx, 0.0, 1.0);
		float2 q1 = fma2(b, S2(t), c);
		float2 q2 = fma2(q1, S2(t), d);
		float2 q = q2;
		res = dot2(q, q);
		sgn = wedge2(fma2(b, S2(2 * t), c), q);
	} else { // 3 roots
		float z = sqrtf(-p);
		float v = acosf(q / (p * z * 2.0)) / 3.0;
		float m = cosf(v);
		float n = sinf(v) * 1.732050808;
		float3 t = saturate3(sub3(scale3(V3(m + m, -n - m, n - m), z), S3(kx)));
		float2 q1 = fma2(b, S2(t.x), c);
		float2 q2 = fma2(q1, S2(t.x), d);
		float2 qx = q2;
		float dx = dot2(qx, qx);
		float sx = wedge2(add2(c, scale2(b, 2.0 * t.x)), qx);
		q1 = fma2(b, S2(t.y), c);
		q2 = fma2(q1, S2(t.y), d);
		float2 qy = q2;
		float dy = dot2(qy, qy);
		float sy = wedge2(add2(c, scale2(b, 2.0 * t.y)), qy);
		if (dx < dy) {
			res = dx;
			sgn = sx;
		} else {
			res = dy;
			sgn = sy;
		}
	}

	return sqrtf(res) * signf(sgn);
}

float
sdsphere(float3 p, float s)
{
	return len3(p) - s;
}

float
sdbox(float3 p, float3 b)
{
	float3 q = sub3(abs3(p), b);
	return len3(max3(q, S3(0.0))) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float
sdroundbox(float3 p, float3 b, float r)
{
	float3 q = sub3(abs3(p), b);
	return len3(max3(q, S3(0.0))) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float
sdboundingbox(float3 p, float3 b, float e)
{
	p = sub3(abs3(p), b);
	float3 q = sub3(abs3(add3(p, S3(e))), S3(e));
	return min(min(
	               len3(max3(V3(p.x, q.y, q.z), S3(0.0))) + min(max(p.x, max(q.y, q.z)), 0.0),
	               len3(max3(V3(q.x, p.y, q.z), S3(0.0))) + min(max(q.x, max(p.y, q.z)), 0.0)),
	           len3(max3(V3(q.x, q.y, p.z), S3(0.0))) + min(max(q.x, max(q.y, p.z)), 0.0));
}

float
sdtorus(float3 p, float2 t)
{
	float2 q = V2(len2(xz3(p)) - t.x, p.y);
	return len2(q) - t.y;
}

float
sdcappedtorus(float3 p, float2 sc, float ra, float rb)
{
	p.x = fabsf(p.x);
	float k = (sc.y * p.x > sc.x * p.y) ? dot2(xy3(p), sc) : len2(xy3(p));
	return sqrtf(dot3(p, p) + ra * ra - 2.0 * ra * k) - rb;
}

float
sdlink(float3 p, float le, float r1, float r2)
{
	float3 q = V3(p.x, max(fabsf(p.y) - le, 0.0), p.z);
	return len2(V2(len2(xy3(q)) - r1, q.z)) - r2;
}

float
sdcone(float3 p, float2 c, float h)
{
	// c is the sin/cos of the angle, h is height
	// Alternatively pass q instead of (c,h),
	// which is the point at the base in 2D
	float2 q = scale2(V2(c.x / c.y, -1.0), h);

	float2 w = V2(len2(xz3(p)), p.y);
	float2 a = sub2(w, scale2(q, clampf(dot2(w, q) / dot2(q, q), 0.0, 1.0)));
	float2 b = sub2(w, hadamard2(q, V2(clampf(w.x / q.x, 0.0, 1.0), 1.0)));
	float k = signf(q.y);
	float d = min(dot2(a, a), dot2(b, b));
	float s = max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
	return sqrtf(d) * signf(s);
}

float
sdcylinder(float3 p, float3 c)
{
	return len2(sub2(xz3(p), xy3(c))) - c.z;
}

float
sdplane(float3 p, float3 n, float h)
{
	// n must be normalized
	return dot3(p, n) + h;
}

float
sdtriprism(float3 p, float2 h)
{
	float3 q = abs3(p);
	return max(q.z - h.y, max(q.x * 0.866025 + p.y * 0.5, -p.y) - h.x * 0.5);
}

float
sdcapsule(float3 p, float3 a, float3 b, float r)
{
	float3 pa = sub3(p, a);
	float3 ba = sub3(b, a);
	float h = clampf(dot3(pa, ba) / dot3(ba, ba), 0.0, 1.0);
	return len3(sub3(pa, scale3(ba, h))) - r;
}

float
sdverticalcapsule(float3 p, float h, float r)
{
	p.y -= clampf(p.y, 0.0, h);
	return len3(p) - r;
}

float
sdsolidangle(float3 p, float2 c, float ra)
{
	// c is the sin/cos of the angle
	float2 q = V2(len2(xz3(p)), p.y);
	float l = len2(q) - ra;
	float m = len2(sub2(q, scale2(c, clampf(dot2(q, c), 0.0, ra))));
	return max(l, m * signf(c.y * q.x - c.x * q.y));
}

float
sdellipsoid(float3 p, float3 r)
{
	float k0 = len3(hadamard3(p, reciprocal3(r)));
	float k1 = len3(hadamard3(p, reciprocal3(hadamard3(r, r))));
	return k0 * (k0 - 1.0) / k1;
}

float
sdoctahedron(float3 p, float s)
{
	p = abs3(p);
	float m = p.x + p.y + p.z - s;
	float3 q;
	if (3.0 * p.x < m)
		q = p;
	else if (3.0 * p.y < m)
		q = yzx3(p);
	else if (3.0 * p.z < m)
		q = zxy3(p);
	else
		return m * 0.57735027;

	float k = clampf(0.5 * (q.z - q.y + s), 0.0, s);
	return len3(V3(q.x, q.y - s + k, q.z - k));
}

float
sdpyramid(float3 p, float h)
{
	float m2 = h * h + 0.25;

	p.x = fabsf(p.x);
	p.z = fabsf(p.z);
	if (p.z > p.x)
		swapf(&p.x, &p.z);
	p.x -= 0.5;
	p.z -= 0.5;

	float3 q = V3(p.z, h * p.y - 0.5 * p.x, h * p.x + 0.5 * p.y);

	float s = max(-q.x, 0.0);
	float t = clampf((q.y - 0.5 * p.z) / (m2 + 0.25), 0.0, 1.0);

	float a = m2 * (q.x + s) * (q.x + s) + q.y * q.y;
	float b = m2 * (q.x + 0.5 * t) * (q.x + 0.5 * t) + (q.y - m2 * t) * (q.y - m2 * t);

	float d2 = min(q.y, -q.x * m2 - q.y * 0.5) > 0.0 ? 0.0 : min(a, b);

	return sqrtf((d2 + q.z * q.z) / m2) * signf(max(q.z, -p.y));
}
