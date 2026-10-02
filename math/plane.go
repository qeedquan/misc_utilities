/*

https://en.wikipedia.org/wiki/Euclidean_planes_in_three-dimensional_space
https://en.wikipedia.org/wiki/Plane_(mathematics)
http://mathonline.wikidot.com/point-normal-form-of-a-plane
https://mathinsight.org/distance_point_plane

A plane is usually represented using the following information:

Normal vector to the plane (CW/CCW, right hand or left hand is a choice made by the code)
Distance from the origin to the plane (euclidean distance)

Standard Cartesian Equation:
Ax + By + Cz + D = 0
(A, B, C) is the normal vector
D is the position of the plane (distance) relative to the origin

Point Normal Form:
dot(N, P - P0) = 0
N       normal vector (A, B, C)
P/r     position vector of any point on the plane (x, y, z)
P0/r0   known point on the plane (x0, y0, z0)

P-P0 forms a direction vector that lies on the plane and is perpendicular to the normal vector N
If we write out the dot product equation, we get the same equation as the standard cartesian equation

Plane using two vectors:
r = r0 + s*v + t*w
v and w are direction vectors
s and t are scaling factors
r0 is the origin (arbitrary point)

Plane using three points (The points have to be non-collinear):
p1 = (x1, y1, z1)
p2 = (x2, y2, z2)
p3 = (x3, z2, z3)
Solve for the system of equation to get the coefficient for the standard cartesian equation:
ax1 + by1 + cz1 + d = 0
ax2 + by2 + cz2 + d = 0
ax3 + by3 + cz3 + d = 0

Example:
Find equation of plane that passes point P (-2, 3, 4) and perpendicular to normal (1, 3, -7)
a(x-x0) + b(y-y0) + c(z-z0) = 0
(x + 2) + 3(y - 3) - 7(z - 4) = 0
Expand it out:
x + 3y - 7z + 21 = 0

Example:
Find normal vector of plane equation 2x + 3y - 6z + 3 = 0
N = (2, 3, -6)

Example:
Determine the point-normal form of a plane that goes through the points P(1,4,2), Q(−10,4,3), and R(2,2,4).
Construct two direction vector PQ and PR
Q - P = <-11, 0, 1>
R - P = <1, -2, 2>

cross(PQ, PR) = N = <2, 23, 22>

Assume all the 3 points are on a plane, you can pick an arbitrary point and plug it in to the equation:
2(x−1) + 23(x−4) + 22(x−2)=0

Example:
Given a plane as a normal and a point on the plane, calculate the distance to a given point
Normal         (A, B, C)
Point on plane (x, y, z)
Given point    (a, b, c)

distance = abs(A*(a-x) + B*(b-y) + C*(c-z)) / sqrt(A^2 + B^2 + C^2)

*/

package main

import (
	"fmt"
	"math"
)

func main() {
	plane := Plane{Vec3{3, 4, -12}, 5}
	point := Vec3{3, 7, -4}
	fmt.Println(distanceToPlaneFull(plane, point))

	plane.normal = normalize(plane.normal)
	fmt.Println(distanceToPlaneCommon(plane, point))
	fmt.Println(distanceToPlaneFull(plane, point))
}

type Vec3 struct {
	x, y, z float64
}

type Plane struct {
	normal   Vec3
	distance float64
}

/*

A common implementation of distance to plane function assumes that the normal vector is normalized
and it will returned a signed distance so it doesn't use abs()

The full equation for distance to point is:
d = abs(Ax + By + Cz + D) / sqrt(A^2 + B^2 + C^2)

*/

func distanceToPlaneCommon(plane Plane, point Vec3) float64 {
	return dot(plane.normal, point) + plane.distance
}

func distanceToPlaneFull(plane Plane, point Vec3) float64 {
	return math.Abs(dot(plane.normal, point)+plane.distance) / length(plane.normal)
}

/*

To be project the vector a onto a plane:
Calculate the vector component that is orthogonal to the plane (its projection onto the normal vector)
and subtract that from the original vector

proj_plane(a) = a - scale(dot(a, n)/dot(n, n), n)

If n is normalized, dot(n, n) = 1

*/

func projectToPlaneCommon(normal Vec3, vector Vec3) Vec3 {
	return sub(vector, scale(dot(normal, vector), normal))
}

func projectToPlaneFull(normal Vec3, vector Vec3) Vec3 {
	return sub(vector, scale(dot(normal, vector)/dot(normal, normal), normal))
}

func sub(a, b Vec3) Vec3 {
	return Vec3{a.x - b.x, a.y - b.y, a.z - b.z}
}

func scale(s float64, p Vec3) Vec3 {
	return Vec3{p.x * s, p.y * s, p.z * s}
}

func normalize(p Vec3) Vec3 {
	l := length(p)
	return Vec3{p.x / l, p.y / l, p.z / l}
}

func length(p Vec3) float64 {
	return math.Sqrt(p.x*p.x + p.y*p.y + p.z*p.z)
}

func dot(a, b Vec3) float64 {
	return a.x*b.x + a.y*b.y + a.z*b.z
}
