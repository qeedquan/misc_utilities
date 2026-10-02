// https://blog.bruce-hill.com/meandering-triangles/
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

template <typename T>
struct Vec2
{
	T x, y;

	bool operator==(const Vec2 &v) const
	{
		return x == v.x && y == v.y;
	}

	bool operator<(const Vec2 &v) const
	{
		return x < v.x || (x == v.x && y < v.y);
	}

	friend ostream &operator<<(ostream &str, Vec2 const &v)
	{
		str << "Vec2(" << v.x << ", " << v.y << ')';
		return str;
	}
};

typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2d;

struct Triangle
{
	Vec2i v1, v2, v3;

	friend ostream &operator<<(ostream &str, Triangle const &t)
	{
		str << "Triangle{v1=" << t.v1 << ", v2=" << t.v2 << ", v3=" << t.v3 << "}";
		return str;
	}
};

template <typename T>
struct Edge
{
	Vec2<T> e1, e2;

	bool operator<(const Edge &e) const
	{
		return e1 < e.e1 || (e1 == e.e1 && e2 < e.e2);
	}
};

double elevation_function(double x, double y)
{
	return 1 / (2 + sin(2 * sqrt(x * x + y * y))) * (.75 + .5 * sin(x * 2));
}

double circle_function(double x, double y)
{
	x /= 100;
	y /= 100;
	return x * x + y * y;
}

vector<Edge<double>> find_contours(double (*elevation_function)(double, double), int xmin, int xmax, int ymin, int ymax, int spacing, double elevation)
{
	map<Vec2i, double> elevation_data;
	for (auto x = xmin; x < xmax + 1; x += spacing)
	{
		for (auto y = ymin; y < ymax + 1; y += spacing)
			elevation_data[Vec2i{x, y}] = elevation_function(x, y);
	}

	vector<Triangle> triangles;
	for (auto x = xmin; x < xmax; x += spacing)
	{
		for (auto y = ymin; y < ymax; y += spacing)
		{
			triangles.push_back(Triangle{{x, y}, {x + spacing, y}, {x, y + spacing}});
			triangles.push_back(Triangle{{x + spacing, y}, {x, y + spacing}, {x + spacing, y + spacing}});
		}
	}

	vector<Edge<double>> contour_segments;
	for (auto &triangle : triangles)
	{
		vector<Vec2i> below, above;

		vector<Vec2i> v = {triangle.v1, triangle.v2, triangle.v3};
		for (auto v : v)
		{
			if (elevation_data[v] < elevation)
				below.push_back(v);
			else
				above.push_back(v);
		}

		if (below.size() == 0 || above.size() == 0)
			continue;

		vector<Vec2i> &minority = (above.size() < below.size()) ? above : below;
		vector<Vec2i> &majority = (above.size() > below.size()) ? above : below;

		vector<Vec2d> contour_points;
		vector<Edge<int>> crossed_edges = {{minority[0], majority[0]}, {minority[0], majority[1]}};
		for (auto &triangle_edge : crossed_edges)
		{
			double how_far = ((elevation - elevation_data[triangle_edge.e2]) / (elevation_data[triangle_edge.e1] - elevation_data[triangle_edge.e2]));
			Vec2d crossing_point = {
			    how_far * triangle_edge.e1.x + (1 - how_far) * triangle_edge.e2.x,
			    how_far * triangle_edge.e1.y + (1 - how_far) * triangle_edge.e2.y,
			};
			contour_points.push_back(crossing_point);
		}
		contour_segments.push_back(Edge<double>{contour_points[0], contour_points[1]});
	}

	return contour_segments;
}

int main()
{
	find_contours(elevation_function, 0, 100, 0, 100, 1, 0.5);
	auto p = find_contours(circle_function, 0, 100, 0, 100, 1, 0.5);
	for (auto p : p)
	{
		cout << p.e1 << " " << p.e2 << endl;
	}
	return 0;
}
