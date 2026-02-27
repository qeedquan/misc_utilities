#include <iostream>
#include <cassert>
#include <list>
#include <CGAL/minkowski_sum_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>

using namespace std;

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point_2;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2> Pgn_with_holes_2_container;

int main()
{
	// Construct the triangle.
	Polygon_2 P;
	P.push_back(Point_2(-1, -1));
	P.push_back(Point_2(1, -1));
	P.push_back(Point_2(0, 1));

	// Construct the square.
	Polygon_2 Q;
	Q.push_back(Point_2(3, -1));
	Q.push_back(Point_2(5, -1));
	Q.push_back(Point_2(5, 1));
	Q.push_back(Point_2(3, 1));

	// Compute the Minkowski sum.
	Polygon_with_holes_2 sum = CGAL::minkowski_sum_2(P, Q);
	assert(sum.number_of_holes() == 0);

	cout << "P = " << P << endl;
	cout << "Q = " << Q << endl;
	cout << "P (+) Q = " << sum.outer_boundary() << endl;

	return 0;
}
