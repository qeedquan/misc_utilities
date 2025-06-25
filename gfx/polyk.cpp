// ported from http://polyk.ivank.net/polyk.js

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <cfloat>
#include <ctime>
#include <cstdarg>
#include <cassert>
#include <algorithm>
#include <vector>
#include <random>
#include <SDL.h>

using namespace std;

struct Point
{
	float x, y;

	Point(float x = 0, float y = 0);
	Point operator+(Point);
	Point operator-(Point);
	float dot(Point);
	float length();
	Point normalize();
	float dist(Point);
	bool operator==(Point p)
	{
		auto eps = 1e-6f;
		return fabs(x - p.x) < eps && fabs(y - p.y) < eps;
	}
};

struct Rect
{
	float x, y, w, h;

	Rect(float x = 0, float y = 0, float w = 0, float h = 0);
	SDL_Rect to_sdl_rect();
};

struct Edgecast
{
	float dist;
	size_t edge;
	Point closest;
	Point norm;
	Point refl;

	Edgecast();
};

// all of the algorithms only work for simple polygon
struct Polyk
{
	vector<Point> points;

	Point operator[](size_t);
	size_t size();
	void clear();
	void push(Point);
	float get_signed_area();
	Rect get_aabb();
	bool is_simple();
	bool is_convex();
	bool contains_point(Point);
	bool gen_rand_simple(Rect, size_t);
	bool gen_rand_convex(Rect, size_t);
	Edgecast closest_edge(Point);
	bool raycast(Point, Point, Edgecast &);
	bool triangulate(vector<int> &);
	void clip(vector<Point> &, Point, Point);
	Polyk clip_polygon(Polyk &);
	Polyk clip_rect(Rect);
	void flip_winding_direction();
	Polyk slice(Point, Point);

private:
	bool convex(Point, Point, Point);
	bool point_in_triangle(Point, Point, Point, Point);
	bool point_in_rect(Point, Point, Point);
	bool orient2d(Point, Point, Point, Point, Point &, Point &, Point &, Point &);
	bool ray_line_intersection(Point, Point, Point, Point, Point &);
	bool line_intersection(Point, Point, Point, Point, Point &);
	void point_line_dist(Point, Point, Point, size_t, Edgecast &);
	void update_isc(Point, Point, Point, Point, Point, size_t, Edgecast &);
};

#define nelem(x) (sizeof(x) / sizeof(x[0]))

Point::Point(float x, float y)
{
	this->x = x;
	this->y = y;
}

Point Point::operator+(Point p)
{
	return Point(x + p.x, y + p.y);
}

Point Point::operator-(Point p)
{
	return Point(x - p.x, y - p.y);
}

float Point::dot(Point p)
{
	return x * p.x + y * p.y;
}

float Point::length()
{
	return hypot(x, y);
}

Point Point::normalize()
{
	auto l = length();
	return Point(x / l, y / l);
}

float Point::dist(Point p)
{
	return hypot(x - p.x, y - p.y);
}

Rect::Rect(float x, float y, float w, float h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

SDL_Rect Rect::to_sdl_rect()
{
	SDL_Rect r;
	r.x = roundf(x);
	r.y = roundf(y);
	r.w = roundf(w);
	r.h = roundf(h);
	return r;
}

Edgecast::Edgecast()
{
	dist = 0.0f;
	edge = 0;
	closest = Point();
	norm = Point();
	refl = Point();
}

void Polyk::flip_winding_direction()
{
	reverse(points.begin(), points.end());
}

size_t Polyk::size()
{
	return points.size();
}

void Polyk::clear()
{
	points.clear();
}

void Polyk::push(Point a)
{
	points.push_back(a);
}

float Polyk::get_signed_area()
{
	auto &p = points;
	auto n = p.size();

	// points and lines don't have an area
	if (n < 3)
		return 0;

	// shoelace formula
	auto s = 0.0f;
	for (size_t i = 0; i < n - 1; i++)
		s += (p[i + 1].x + p[i].x) * (p[i].y - p[i + 1].y);
	s += (p[0].x + p[n - 1].x) * (p[n - 1].y - p[0].y);

	return s * 0.5;
}

Rect Polyk::get_aabb()
{
	float minx = FLT_MAX;
	float miny = FLT_MAX;
	float maxx = -FLT_MAX;
	float maxy = -FLT_MAX;

	for (auto &p : points)
	{
		minx = min(minx, p.x);
		maxx = max(maxx, p.x);
		miny = min(miny, p.y);
		maxy = max(maxy, p.y);
	}
	return Rect(minx, miny, maxx - minx, maxy - miny);
}

bool Polyk::convex(Point a, Point b, Point c)
{
	// determinant formula to determine inside or out based on the edges
	return (a.y - b.y) * (c.x - b.x) + (b.x - a.x) * (c.y - b.y) >= 0;
}

bool Polyk::is_simple()
{
	// simple if no edges crosses each other
	auto &p = points;

	// if we have less than 2 edges, that means
	// we have a point, line, or triangle and they
	// are simple
	if (p.size() < 4)
		return true;

	Point c;
	ssize_t n = p.size();
	for (ssize_t i = 0; i < n; i++)
	{
		auto a1 = p[i];
		auto a2 = p[(i + 1) % n];
		for (ssize_t j = i + 1; j < n; j++)
		{
			// if the points are the same or right next
			// to each other so it makes an edge, one
			// edge can't intersect with each other
			if (labs(i - j) < 2)
				continue;
			if (i == 0 && j == n - 1)
				continue;
			if (i == n - 1 && j == 0)
				continue;

			auto b1 = p[j];
			auto b2 = p[(j + 1) % n];

			// here we have an edge (a1, a2) and edge (b1, b2)
			// we check for a intersection, if they intersect
			// it means they are not simple
			if (line_intersection(a1, a2, b1, b2, c))
				return false;
		}
	}

	return true;
}

bool Polyk::is_convex()
{
	auto &p = points;

	// points and lines are always convex
	if (p.size() < 3)
		return true;

	// polygon is a convex when each inner angle is <= 180 degrees
	auto l = p.size() - 2;
	for (size_t i = 0; i < l; i++)
	{
		if (!convex(p[i], p[i + 1], p[i + 2]))
			return false;
	}
	if (!convex(p[l], p[l + 1], p[0]))
		return false;
	if (!convex(p[l + 1], p[0], p[1]))
		return false;

	return true;
}

void Polyk::point_line_dist(Point u, Point a, Point b, size_t edge, Edgecast &isc)
{
	// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
	auto x = u.x;
	auto y = u.y;
	auto x1 = a.x;
	auto y1 = a.y;
	auto x2 = b.x;
	auto y2 = b.y;

	auto A = x - x1;
	auto B = y - y1;
	auto C = x2 - x1;
	auto D = y2 - y1;

	auto dot = A * C + B * D;
	auto len_sq = C * C + D * D;
	auto param = dot / len_sq;

	float xx, yy;
	if (param < 0 || (x1 == x2 && y1 == y2))
	{
		xx = x1;
		yy = y1;
	}
	else if (param > 1)
	{
		xx = x2;
		yy = y2;
	}
	else
	{
		xx = x1 + param * C;
		yy = y1 + param * D;
	}

	auto dx = x - xx;
	auto dy = y - yy;
	auto dist = hypot(dx, dy);
	if (dist < isc.dist)
	{
		isc.dist = dist;
		isc.edge = edge;
		isc.closest.x = xx;
		isc.closest.y = yy;
	}
}

bool Polyk::point_in_triangle(Point p, Point a, Point b, Point c)
{
	// convert into a triangle using barycentric coordinates
	// if the coordinates are between 0 and 1, it is within the triangle
	// h = 1 / (e, f, g) * [(h, f, g)e + (e, h, g)f + (e, f, h)g]
	// where (e, f, g) is dot(cross(e, f), g)
	auto v0 = c - a;
	auto v1 = b - a;
	auto v2 = p - a;

	auto dot00 = v0.dot(v0);
	auto dot01 = v0.dot(v1);
	auto dot02 = v0.dot(v2);
	auto dot11 = v1.dot(v1);
	auto dot12 = v1.dot(v2);

	auto det = (dot00 * dot11 - dot01 * dot01);
	if (det != 0.0f)
		det = 1.0f / det;

	auto u = (dot11 * dot02 - dot01 * dot12) * det;
	auto v = (dot00 * dot12 - dot01 * dot02) * det;

	return (u >= 0) && (v >= 0) && (u + v < 1);
}

bool Polyk::point_in_rect(Point a, Point b, Point c)
{
	// check for point a inside rectangle with min/max point of b/c
	auto eps = 1e-6f;
	auto minx = min(b.x, c.x);
	auto maxx = max(b.x, c.x);
	auto miny = min(b.y, c.y);
	auto maxy = max(b.x, c.y);

	if (minx == maxx)
		return miny <= a.y && a.y <= maxy;
	if (miny == maxy)
		return minx <= a.x && a.x <= maxx;
	return minx <= a.x + eps && a.x - eps <= maxx && miny <= a.y + eps && a.y - eps <= maxy;
}

bool Polyk::orient2d(Point a1, Point a2, Point b1, Point b2, Point &da, Point &db, Point &I, Point &c)
{
	// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
	// using determinant method
	da = a1 - a2;
	db = b1 - b2;

	auto det = da.x * db.y - da.y * db.x;
	if (det == 0.0f)
		return false;
	det = 1.0f / det;

	auto A = a1.x * a2.y - a1.y * a2.x;
	auto B = b1.x * b2.y - b1.y * b2.x;
	I = Point(
	    (A * db.x - da.x * B) * det,
	    (A * db.y - da.y * B) * det);
	c = I;

	return true;
}

bool Polyk::ray_line_intersection(Point a1, Point a2, Point b1, Point b2, Point &c)
{
	Point da, db, I;
	if (!orient2d(a1, a2, b1, b2, da, db, I, c))
		return false;
	if (!point_in_rect(I, b1, b2))
		return false;
	if ((da.y > 0 && I.y > a1.y) || (da.y < 0 && I.y < a1.y))
		return false;
	if ((da.x > 0 && I.x > a1.x) || (da.x < 0 && I.x < a1.x))
		return false;
	return true;
}

bool Polyk::line_intersection(Point a1, Point a2, Point b1, Point b2, Point &c)
{
	Point da, db, I;
	if (!orient2d(a1, a2, b1, b2, da, db, I, c))
		return false;
	return point_in_rect(I, a1, a2) && point_in_rect(I, b1, b2);
}

bool Polyk::contains_point(Point u)
{
	// https://en.wikipedia.org/wiki/Point_in_polygon
	// raycast algorithm
	auto &p = points;
	auto n = p.size();

	// polygons has at least 3 points
	if (n < 3)
		return false;

	Point a;
	auto b = p[n - 1];
	auto lup = false;
	for (size_t i = 0; i < n; i++)
	{
		a = b;
		b = p[i] - u;
		if (a.y == b.y)
			continue;
		lup = b.y > a.y;
	}

	size_t depth = 0;
	for (size_t i = 0; i < n; i++)
	{
		a = b;
		b = p[i] - u;

		// both up or down
		if (a.y < 0 && b.y < 0)
			continue;

		// both up or down
		if (a.y > 0 && b.y > 0)
			continue;

		// both points on the left
		if (a.x < 0 && b.x < 0)
			continue;

		if (a.y == b.y && min(a.x, b.x) <= 0)
			return true;

		if (a.y == b.y)
			continue;

		auto lx = a.x + (b.x - a.x) * (-a.y / (b.y - a.y));
		if (lx == 0)
			return true;
		if (lx > 0)
			depth++;
		if (a.y == 0 && lup && b.y > a.y)
			depth--;
		if (a.y == 0 && !lup && b.y < a.y)
			depth--;
		lup = b.y > a.y;
	}

	// odd-even rule, odd crossings mean the point is inside the polygon
	return (depth & 1) != 0;
}

Edgecast Polyk::closest_edge(Point u)
{
	Edgecast isc;
	isc.dist = FLT_MAX;

	auto &p = points;
	auto n = p.size();
	if (n < 2)
		return isc;

	// loop through all edges and find calculate the distance
	// from the point to that edge and minimize the distance
	auto a1 = u;
	Point b1, b2;
	for (size_t i = 0; i < n - 1; i++)
	{
		b1 = p[i];
		b2 = p[i + 1];
		point_line_dist(a1, b1, b2, i, isc);
	}
	b1 = b2;
	b2 = p[0];
	point_line_dist(a1, b1, b2, n - 1, isc);

	isc.norm.x = (u.x - isc.closest.x) / isc.dist;
	isc.norm.y = (u.y - isc.closest.y) / isc.dist;
	return isc;
}

bool Polyk::gen_rand_simple(Rect r, size_t n)
{
	if (n < 3)
		return false;

	auto &p = points;

	// use rejection sampling, really slow for anything except small number of edges
	p.clear();
	for (size_t i = 0; i < n;)
	{
		p.push_back(Point(r.x + drand48() * r.w, r.y + drand48() * r.h));
		if (!is_simple())
			p.pop_back();
		else
			i++;
	}

	return true;
}

bool Polyk::gen_rand_convex(Rect r, size_t n)
{
	// https://cglab.ca/~sander/misc/ConvexGeneration/convex.html
	// rejection sampling is way too slow for convex shapes
	// use valtr algorithm to generate (any convex polygon is also a simple polygon)

	if (n < 3)
		return false;

	// generate 2 random list of x and y coordinates
	vector<float> xp;
	vector<float> yp;
	for (size_t i = 0; i < n; i++)
	{
		xp.push_back(drand48());
		yp.push_back(drand48());
	}

	// sort them smaller to larger
	sort(xp.begin(), xp.end());
	sort(yp.begin(), yp.end());

	// isolate extreme points
	auto minx = xp[0];
	auto maxx = xp[n - 1];
	auto miny = yp[0];
	auto maxy = yp[n - 1];

	auto last_top = minx;
	auto last_bot = minx;
	auto last_left = miny;
	auto last_right = miny;

	// divide the interior points into two random chains
	// and extract the vector components, interior means
	// between the min/max points here
	vector<float> xc;
	vector<float> yc;
	for (size_t i = 1; i < n - 1; i++)
	{
		auto x = xp[i];
		auto y = yp[i];

		if (random() & 1)
		{
			xc.push_back(x - last_top);
			last_top = x;
		}
		else
		{
			xc.push_back(last_bot - x);
			last_bot = x;
		}

		if (random() & 1)
		{
			yc.push_back(y - last_left);
			last_left = y;
		}
		else
		{
			yc.push_back(last_right - y);
			last_right = y;
		}
	}
	xc.push_back(maxx - last_top);
	xc.push_back(last_bot - maxx);
	yc.push_back(maxy - last_left);
	yc.push_back(last_right - maxy);

	// shuffle the list
	shuffle(yc.begin(), yc.end(), default_random_engine{});

	// sort them by angles
	vector<Point> vec;
	for (size_t i = 0; i < n; i++)
	{
		vec.push_back(Point(xc[i], yc[i]));
	}

	sort(vec.begin(), vec.end(), [&](Point a, Point b) -> bool {
		double ta = atan2(a.y, a.x);
		double tb = atan2(b.y, b.x);
		return ta < tb;
	});

	// lay them end to end
	auto x = 0.0f;
	auto y = 0.0f;
	auto min_polygonx = 0.0f;
	auto min_polygony = 0.0f;

	points.clear();
	for (size_t i = 0; i < n; i++)
	{
		points.push_back(Point(x, y));
		x += vec[i].x;
		y += vec[i].y;

		min_polygonx = min(min_polygonx, x);
		min_polygony = min(min_polygony, y);
	}

	// move the polygon to the original min and max coordinates
	auto xshift = minx - min_polygonx;
	auto yshift = miny - min_polygony;
	for (size_t i = 0; i < n; i++)
	{
		points[i] = Point(points[i].x + xshift, points[i].y + yshift);
		points[i].x = points[i].x * r.w + r.x;
		points[i].y = points[i].y * r.h + r.y;
	}

	// if we have too many points here, is_convex() can fail
	// due to floating point precision errors though we can return true
	// here because the algorithm above should generate a convex shape

	return true;
}

bool Polyk::triangulate(vector<int> &tgs)
{
	// https://en.wikipedia.org/wiki/Polygon_triangulation#Ear_clipping_method
	// https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf
	// the algorithm implemented below is not the full ear clipping method
	// because it only works for convex polygons, not the full simple polygon
	tgs.clear();

	auto &p = points;
	if (p.size() < 3)
		return false;

	vector<int> avl;
	for (size_t i = 0; i < p.size(); i++)
		avl.push_back(i);

	auto status = true;
	size_t i = 0;
	auto al = p.size();
	while (al > 3)
	{
		auto i0 = avl[(i + 0) % al];
		auto i1 = avl[(i + 1) % al];
		auto i2 = avl[(i + 2) % al];

		auto a = p[i0];
		auto b = p[i1];
		auto c = p[i2];

		auto ear_found = false;
		if (convex(a, b, c))
		{
			ear_found = true;
			for (size_t j = 0; j < al; j++)
			{
				auto vi = avl[j];
				if (vi == i0 || vi == i1 || vi == i2)
					continue;
				if (point_in_triangle(p[vi], a, b, c))
				{
					ear_found = false;
					break;
				}
			}
		}
		if (ear_found)
		{
			tgs.push_back(i0);
			tgs.push_back(i1);
			tgs.push_back(i2);
			avl.erase(avl.begin() + ((i + 1) % al));
			al--;
			i = 0;
		}
		else if (i++ > 3 * al)
		{
			// no convex angles
			status = false;
			break;
		}
	}
	tgs.push_back(avl[0]);
	tgs.push_back(avl[1]);
	tgs.push_back(avl[2]);

	return status;
}

bool Polyk::raycast(Point u, Point d, Edgecast &isc)
{
	isc.dist = FLT_MAX;

	auto &p = points;
	auto n = p.size();
	if (n == 0)
		return false;

	auto a1 = u;
	auto a2 = u + d;
	Point b1, b2, c;
	for (size_t i = 0; i < n - 1; i++)
	{
		b1 = p[i];
		b2 = p[i + 1];
		if (ray_line_intersection(a1, a2, b1, b2, c))
			update_isc(d, a1, b1, b2, c, i, isc);
	}
	b1 = b2;
	b2 = p[0];
	if (ray_line_intersection(a1, a2, b1, b2, c))
		update_isc(d, a1, b1, b2, c, n - 1, isc);

	if (isc.dist == FLT_MAX)
		return false;

	return true;
}

void Polyk::update_isc(Point d, Point a1, Point b1, Point b2, Point c, size_t edge, Edgecast &isc)
{
	auto nrl = a1.dist(c);
	if (nrl < isc.dist)
	{
		auto ibl = 1 / (b1.dist(b2));
		auto nx = -(b2.y - b1.y) * ibl;
		auto ny = (b2.x - b1.x) * ibl;
		auto ddot = 2 * (d.x * nx + d.y * ny);
		isc.dist = nrl;
		isc.norm = Point(nx, ny);
		isc.refl = Point(-ddot * nx + d.x, -ddot * ny + d.y);
		isc.edge = edge;
	}
}

void Polyk::clip(vector<Point> &poly_points, Point p1, Point p2)
{
	// https://www.geeksforgeeks.org/polygon-clipping-sutherland-hodgman-algorithm-please-change-bmp-images-jpeg-png/
	// works only on clockwise orientation, so need to flip direction on polygons if needed
	vector<Point> new_points;
	Point pt;

	auto x1 = p1.x;
	auto y1 = p1.y;
	auto x2 = p2.x;
	auto y2 = p2.y;
	for (size_t i = 0; i < poly_points.size(); i++)
	{
		// i and k form a line in polygon
		size_t k = (i + 1) % poly_points.size();
		auto ix = poly_points[i].x;
		auto iy = poly_points[i].y;
		auto kx = poly_points[k].x;
		auto ky = poly_points[k].y;

		// Calculating position of first point
		// w.r.t. clipper line
		auto i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

		// Calculating position of second point
		// w.r.t. clipper line
		auto k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

		// Case 1 : When both points are inside
		if (i_pos < 0 && k_pos < 0)
		{
			// Only second point is added
			new_points.push_back(Point(kx, ky));
		}

		// Case 2: When only first point is outside
		else if (i_pos >= 0 && k_pos < 0)
		{
			// Point of intersection with edge
			// and the second point is added
			line_intersection(Point(x1, y1), Point(x2, y2), Point(ix, iy), Point(kx, ky), pt);
			new_points.push_back(pt);
			new_points.push_back(Point(kx, ky));
		}

		// Case 3: When only second point is outside
		else if (i_pos < 0 && k_pos >= 0)
		{
			// Only point of intersection with edge is added
			line_intersection(Point(x1, y1), Point(x2, y2), Point(ix, iy), Point(kx, ky), pt);
			new_points.push_back(pt);
		}

		// Case 4: When both points are outside
		else
		{
			// No points are added
		}
	}

	// Copying new points into original array
	// and changing the no. of vertices
	poly_points.resize(new_points.size());
	for (size_t i = 0; i < poly_points.size(); i++)
		poly_points[i] = new_points[i];
}

Polyk Polyk::clip_polygon(Polyk &clipper)
{
	Polyk poly;

	for (size_t i = 0; i < points.size(); i++)
		poly.push(points[i]);

	for (size_t i = 0; i < clipper.size(); i++)
	{
		size_t k = (i + 1) % clipper.size();
		clip(poly.points, clipper.points[i], clipper.points[k]);
	}

	return poly;
}

Polyk Polyk::clip_rect(Rect r)
{
	Polyk pl;

	pl.push(Point(r.x, r.y));
	pl.push(Point(r.x, r.y + r.h));
	pl.push(Point(r.x + r.w, r.y + r.h));
	pl.push(Point(r.x + r.w, r.y));
	if (pl.get_signed_area() < 0)
		pl.flip_winding_direction();
	return clip_polygon(pl);
}

Polyk Polyk::slice(Point a, Point b)
{
	Polyk result;

	// if the line is inside the polygon, return the whole polygon
	if (contains_point(a) || contains_point(b))
	{
		for (size_t i = 0; i < points.size(); i++)
			result.push(points[i]);
		return result;
	}

	vector<pair<Point, bool>> ps, iscs;
	for (size_t i = 0; i < points.size(); i++)
		ps.push_back(make_pair(points[i], false));

	for (size_t i = 0; i < ps.size(); i++)
	{
		pair<Point, bool> fisc, lisc;
		Point isc;

		if (iscs.size() > 0)
		{
			fisc = iscs[0];
			lisc = iscs[iscs.size() - 1];
		}

		// loop through the list of edges and see if they intersect line a and b
		if (line_intersection(a, b, ps[i].first, ps[(i + 1) % ps.size()].first, isc))
		{
			// if the current edge intersect with the cut line
			// make sure that this edge is not duplicated
			if ((!fisc.second || isc.dist(fisc.first) > 1e-10) && (!lisc.second || isc.dist(lisc.first) > 1e-10))
			{
				iscs.push_back(make_pair(isc, true));

				// add the intersection point to the list
				// so we can test the intersection of that point
				ps.insert(ps.begin() + i + 1, make_pair(isc, true));

				// each edge only get used once, they don't form a strip
				i++;
			}
		}
	}

	// if there is no intersection points to make a line
	// return the polygon
	if (iscs.size() < 2)
	{
		for (size_t i = 0; i < points.size(); i++)
			result.push(points[i]);
		return result;
	}

	// sort distance closest to the point a (the start point of our cutline)
	sort(iscs.begin(), iscs.end(), [&](auto u, auto v) {
		return a.dist(u.first) < a.dist(v.first);
	});

	vector<vector<pair<Point, bool>>> pgs;
	size_t dir = 0;
	while (iscs.size() > 0)
	{
		auto i0 = iscs[0];
		auto i1 = iscs[1];

		auto index_of = [&](pair<Point, bool> val) -> ssize_t {
			for (size_t i = 0; i < ps.size(); i++)
			{
				if (ps[i].first == val.first && ps[i].second == val.second)
					return i;
			}
			return -1;
		};

		auto first_with_flag = [&](ssize_t ind) {
			for (;;)
			{
				ind = (ind + 1) % ps.size();
				if (ps[ind].second)
					return ind;
			}
		};

		auto get_points = [&](ssize_t ind0, ssize_t ind1) -> vector<pair<Point, bool>> {
			vector<pair<Point, bool>> nps;
			ssize_t n = ps.size();
			if (ind1 < ind0)
				ind1 += n;
			for (ssize_t i = ind0; i <= ind1; i++)
				nps.push_back(ps[i % n]);
			return nps;
		};

		auto ind0 = index_of(i0);
		auto ind1 = index_of(i1);
		auto solved = false;

		if (first_with_flag(ind0) == ind1)
			solved = true;
		else
		{
			i0 = iscs[1];
			i1 = iscs[0];
			ind0 = index_of(i0);
			ind1 = index_of(i1);
			if (first_with_flag(ind0) == ind1)
				solved = true;
		}

		if (solved)
		{
			dir--;
			auto pgn = get_points(ind0, ind1);
			pgs.push_back(pgn);
			ps = get_points(ind1, ind0);
			i0.second = i1.second = false;
			iscs.erase(iscs.begin() + 2);
			if (iscs.size() == 0)
				pgs.push_back(ps);
		}
		else
		{
			dir++;
			reverse(iscs.begin(), iscs.end());
		}

		if (dir > 1)
			break;
	}

	for (size_t i = 0; i < pgs.size(); i++)
	{
		for (size_t j; j < pgs[i].size(); j++)
			result.push(pgs[i][j].first);
	}

	return result;
}

SDL_Window *window;
SDL_Renderer *renderer;
int state;
int flag;
int sel;
Polyk polysel;
Rect rectsel;
SDL_Color colsel;
vector<pair<Polyk, SDL_Color>> polys;
vector<pair<Rect, SDL_Color>> rects;

float sign(float x)
{
	if (x < 0)
		return -1.0f;
	if (x > 0)
		return 1.0f;
	return 0.0f;
}

SDL_Color rand_rgb()
{
	auto c = SDL_Color{(Uint8)random(), (Uint8)random(), (Uint8)random(), 255};
	return c;
}

void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

void init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fatal("failed to init sdl: %s", SDL_GetError());

	if (SDL_CreateWindowAndRenderer(800, 800, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
		fatal("failed to create window: %s", SDL_GetError());

	SDL_SetWindowTitle(window, "Polygon operations");
}

void reset(int newstate)
{
	static const char *titles[] = {
	    "Polygon Simple Test",
	    "Polygon Raycast",
	    "Polygon Clipping",
	};

	state = newstate;
	switch (state)
	{
	case 0:
	case 1:
		polysel.gen_rand_convex(Rect(200, 200, 200, 200), 10);
		break;
	case 2:
		polysel.clear();
		break;
	default:
		assert(0);
	}
	polys.clear();
	rects.clear();
	rectsel = Rect();
	sel = 0;
	flag = 0x1 | 0x2 | 0x4;

	SDL_SetWindowTitle(window, titles[state]);
}

void event()
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
		case SDL_QUIT:
			exit(0);
		case SDL_KEYDOWN:
			switch (ev.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				exit(0);
			case SDLK_1:
				SDL_SetWindowTitle(window, "Polygon operations");
				reset(0);
				break;
			case SDLK_2:
				SDL_SetWindowTitle(window, "Polygon Raycast");
				reset(1);
				break;
			case SDLK_3:
				reset(2);
				break;
			case SDLK_q:
				flag ^= 0x1;
				break;
			case SDLK_w:
				flag ^= 0x2;
				break;
			case SDLK_e:
				flag ^= 0x4;
				break;
			case SDLK_a:
				if (state < 2)
					polysel.gen_rand_simple(Rect(200, 200, 200, 200), 3 + rand() % 10);
				break;
			case SDLK_s:
				if (state < 2)
					polysel.gen_rand_convex(Rect(200, 200, 200, 200), 3 + rand() % 100);
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (state == 2)
			{
				switch (ev.button.button)
				{
				case SDL_BUTTON_LEFT:
					if (sel == 0)
					{
						polysel.push(Point(ev.button.x, ev.button.y));
						rectsel = Rect();
						colsel = rand_rgb();
						sel = 1;
					}
					break;
				case SDL_BUTTON_MIDDLE:
					if (sel == 0)
					{
						rectsel = Rect(ev.button.x, ev.button.y, 0, 0);
						polysel.clear();
						colsel = rand_rgb();
						sel = 2;
					}
					break;
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (state == 2)
			{
				switch (ev.button.button)
				{
				case SDL_BUTTON_LEFT:
					polysel.push(Point(ev.button.x, ev.button.y));
					break;
				case SDL_BUTTON_RIGHT:
					if (polysel.get_signed_area() < 0)
						polysel.flip_winding_direction();
					polys.push_back(make_pair(polysel, colsel));
					polysel.clear();
					sel = 0;
					break;
				case SDL_BUTTON_MIDDLE:
					if (sel == 2)
					{
						rects.push_back(make_pair(rectsel, colsel));
						rectsel = Rect();
						polysel.clear();
						sel = 0;
					}
					break;
				}
			}
			break;
		case SDL_MOUSEMOTION:
			if (state == 2)
			{
				switch (sel)
				{
				case 0:
					break;
				case 2:
					rectsel.w = ev.button.x - rectsel.x;
					rectsel.h = ev.button.y - rectsel.y;
					break;
				}
			}
			break;
		}
	}
}

void draw_line(Point p1, Point p2, SDL_Color col)
{
	SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
	SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
}

void draw_rect(Rect r, SDL_Color col)
{
	auto sr = r.to_sdl_rect();
	SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
	SDL_RenderDrawRect(renderer, &sr);
}

void draw_triangle(Point a, Point b, Point c)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawLine(renderer, a.x, a.y, b.x, b.y);

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderDrawLine(renderer, a.x, a.y, c.x, c.y);

	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	SDL_RenderDrawLine(renderer, b.x, b.y, c.x, c.y);
}

void draw_polyk(Polyk &pl, bool aabb, bool insides, bool triangulate, SDL_Color bordercol)
{
	for (size_t i = 0; i < pl.size(); i++)
	{
		size_t j = (i + 1) % pl.size();
		SDL_SetRenderDrawColor(renderer, bordercol.r, bordercol.g, bordercol.b, bordercol.a);
		SDL_RenderDrawLine(renderer, pl.points[i].x, pl.points[i].y, pl.points[j].x, pl.points[j].y);
	}

	auto r = pl.get_aabb();
	if (aabb)
	{
		auto sr = r.to_sdl_rect();
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &sr);
	}

	if (insides)
	{
		for (auto y = r.y; y <= r.y + r.h; y++)
		{
			for (auto x = r.x; x <= r.x + r.w; x++)
			{
				if (pl.contains_point(Point(x, y)))
				{
					SDL_SetRenderDrawColor(renderer, 200, 231, 135, 255);
					SDL_RenderDrawPoint(renderer, x, y);
				}
			}
		}
	}

	if (triangulate)
	{
		vector<int> t;
		pl.triangulate(t);
		for (size_t i = 2; i < t.size(); i += 3)
		{
			auto i0 = t[i - 2];
			auto i1 = t[i - 1];
			auto i2 = t[i];

			auto p0 = pl.points[i0];
			auto p1 = pl.points[i1];
			auto p2 = pl.points[i2];

			draw_triangle(p0, p1, p2);
		}
	}
}

void draw_closest_edge(Polyk &pl, Point p)
{
	auto c = pl.closest_edge(p);
	SDL_SetRenderDrawColor(renderer, 10, 40, 60, 255);
	SDL_RenderDrawLine(renderer, c.closest.x, c.closest.y, p.x, p.y);
}

void draw_rand_raycasts(Polyk &pl)
{
	static const size_t rn = 512;

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	mx++;

	auto diff = 2 * M_PI / rn;
	Edgecast isc;
	for (size_t i = 0; i < rn; i++)
	{
		auto dx = cos(i * diff);
		auto dy = sin(i * diff);
		if (!pl.raycast(Point(mx, my), Point(dx, dy), isc))
			isc.dist = 4000.0f;
		SDL_SetRenderDrawColor(renderer, 100, 150, 60, 255);
		SDL_RenderDrawLine(renderer, mx, my, mx + dx * isc.dist, my + dy * isc.dist);
	}
}

void draw()
{
	SDL_Color black = {0, 0, 0, 255};
	int mx, my;

	SDL_GetMouseState(&mx, &my);
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderClear(renderer);
	switch (state)
	{
	case 0:
		draw_polyk(polysel, flag & 1, flag & 2, flag & 4, black);
		draw_closest_edge(polysel, Point(mx, my));
		break;
	case 1:
		draw_polyk(polysel, flag & 1, flag & 2, flag & 4, black);
		draw_rand_raycasts(polysel);
		break;
	case 2:
		for (size_t i = 0; i < rects.size(); i++)
			draw_rect(rects[i].first, rects[i].second);
		draw_rect(rectsel, colsel);
		for (size_t i = 0; i < polys.size(); i++)
		{
			if (rects.size() == 0)
				draw_polyk(polys[i].first, false, false, false, polys[i].second);
			else
			{
				for (size_t j = 0; j < rects.size(); j++)
				{
					auto clipped = polys[i].first.clip_rect(rects[j].first);
					if (clipped.size() > 0)
						draw_polyk(clipped, false, false, false, polys[i].second);
				}
			}
		}
		for (size_t i = 0; i < polysel.size(); i++)
		{
			draw_line(polysel.points[i], polysel.points[(i + 1) % polysel.size()], colsel);
		}
		if (sel == 1 && polysel.size() > 0)
		{
			draw_line(polysel.points[0], Point(mx, my), colsel);
			draw_line(polysel.points[polysel.size() - 1], Point(mx, my), colsel);
		}
		break;
	}
	SDL_RenderPresent(renderer);
}

int main()
{
	srand48(time(NULL));
	srandom(time(NULL));
	init_sdl();
	setvbuf(stdout, NULL, _IONBF, 0);
	reset(0);
	for (;;)
	{
		event();
		draw();
	}

	return 0;
}
