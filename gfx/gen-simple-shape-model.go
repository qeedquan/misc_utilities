// use something like meshlab to view the model files
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"math"
	"os"
)

var (
	pointcloud = flag.Bool("p", false, "generate only points")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("shapeobj: ")

	flag.Parse()
	writeobj("cube_origin.obj", gencube(vec3{0, 0, 0}, 1))
	writeobj("tetrahedron_origin.obj", gentetrahedron(vec3{0, 0, 0}, 1))
	writeobj("cone_pointing_down_z_axis.obj", gencone(vec3{0, 0, -1}, vec3{0, 0, 0}, 1, 1, 1000))
	writeobj("torus_origin.obj", gentorus(vec3{0, 0, 0}, 3, 1.5, 100, 100))
	writeobj("cylinder_origin.obj", gencylinder(vec3{0, 0, 0}, 3, 5, 100))
	writeobj("sphere_origin.obj", gensphere(vec3{0, 0, 0}, 1, 100, 100))
	writeobj("plane_origin.obj", genplane(vec3{0, 0, 0}, vec3{1, 0, 0}, vec3{0, 1, 0}, 5))
	writeobj("sh_origin_00000000.obj", gensh(vec3{0, 0, 0}, []float64{0, 0, 0, 0, 0, 0, 0, 0}, 50))
	writeobj("sh_origin_00045012.obj", gensh(vec3{0, 0, 0}, []float64{0, 0, 0, 4, 5, 0, 1, 2}, 50))
	writeobj("sh_origin_30088888.obj", gensh(vec3{0, 0, 0}, []float64{3, 0, 0, 8, 8, 8, 8, 8}, 50))
	writeobj("sh_origin_11111111.obj", gensh(vec3{0, 0, 0}, []float64{1, 1, 1, 1, 1, 1, 1, 1}, 50))
	writeobj("sh_origin_12345678.obj", gensh(vec3{0, 0, 0}, []float64{1, 2, 3, 4, 5, 6, 7, 8}, 50))
	writeobj("sh_origin_66666666.obj", gensh(vec3{0, 0, 0}, []float64{6, 6, 6, 6, 6, 6, 6, 6}, 50))
	writeobj("sh_origin_33333333.obj", gensh(vec3{0, 0, 0}, []float64{3, 3, 3, 3, 3, 3, 3, 3}, 50))
	writeobj("sh_origin_44444444.obj", gensh(vec3{0, 0, 0}, []float64{4, 4, 4, 4, 4, 4, 4, 4}, 50))
	writeobj("sh_origin_61312132.obj", gensh(vec3{0, 0, 0}, []float64{6, 1, 3, 1, 2, 1, 3, 2}, 50))
	writeobj("rossler_origin.obj", genrossler(vec3{0, 0, 0}, vec3{0.1, 0, 0}, 0.05, 0.2, 0.2, 5.7, 100, 1000000))
	writeobj("lorenz_origin.obj", genlorenz(vec3{0, 0, 0}, vec3{0.1, 0.001, 0.01}, 0.00011, 10, 28, 8/3.0, 100, 1000000))
	writeobj("clifford_1_origin.obj", genclifford(vec3{0, 0, 0}, vec3{0.1, 0.1, 0.1}, -1.4, 1.6, 1.0, 0.7, 100000))
	writeobj("clifford_2_origin.obj", genclifford(vec3{0, 0, 0}, vec3{0.1, 0.1, 0.1}, 1.7, 1.7, 0.6, 1.2, 100000))
	writeobj("clifford_3_origin.obj", genclifford(vec3{0, 0, 0}, vec3{0.1, 0.5, 0.3}, -1.8, -2.0, -0.5, -0.9, 100000))
	writeobj("hopf_fibration.obj", genhopf(vec3{0, 0, 0}, 1, 10))
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

type vec3 struct{ x, y, z float64 }

func (v vec3) String() string { return fmt.Sprintf("[%.4f %.4f %.4f]", v.x, v.y, v.z) }

func add3(a, b vec3) vec3           { return vec3{a.x + b.x, a.y + b.y, a.z + b.z} }
func sub3(a, b vec3) vec3           { return vec3{a.x - b.x, a.y - b.y, a.z - b.z} }
func neg3(a vec3) vec3              { return vec3{-a.x, -a.y, -a.z} }
func scale3(a vec3, s float64) vec3 { return vec3{a.x * s, a.y * s, a.z * s} }
func dot3(a, b vec3) float64        { return a.x*b.x + a.y*b.y + a.z*b.z }
func len3(a vec3) float64           { return math.Sqrt(dot3(a, a)) }

func norm3(a vec3) vec3 {
	l := len3(a)
	if l == 0 {
		return a
	}
	return vec3{a.x / l, a.y / l, a.z / l}
}

func cross3(a, b vec3) vec3 {
	return vec3{
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x,
	}
}

func perp3(a vec3) vec3 {
	m := math.Abs(a.x)
	b := vec3{1, 0, 0}
	if n := math.Abs(a.y); n < m {
		m = n
		b = vec3{0, 1, 0}
	}
	if n := math.Abs(a.z); n < m {
		m = n
		b = vec3{0, 0, 1}
	}
	return cross3(a, b)
}

func det44(m [][]float64) float64 {
	return m[0][3]*m[1][2]*m[2][1]*m[3][0] - m[0][2]*m[1][3]*m[2][1]*m[3][0] -
		m[0][3]*m[1][1]*m[2][2]*m[3][0] + m[0][1]*m[1][3]*m[2][2]*m[3][0] +
		m[0][2]*m[1][1]*m[2][3]*m[3][0] - m[0][1]*m[1][2]*m[2][3]*m[3][0] -
		m[0][3]*m[1][2]*m[2][0]*m[3][1] + m[0][2]*m[1][3]*m[2][0]*m[3][1] +
		m[0][3]*m[1][0]*m[2][2]*m[3][1] - m[0][0]*m[1][3]*m[2][2]*m[3][1] -
		m[0][2]*m[1][0]*m[2][3]*m[3][1] + m[0][0]*m[1][2]*m[2][3]*m[3][1] +
		m[0][3]*m[1][1]*m[2][0]*m[3][2] - m[0][1]*m[1][3]*m[2][0]*m[3][2] -
		m[0][3]*m[1][0]*m[2][1]*m[3][2] + m[0][0]*m[1][3]*m[2][1]*m[3][2] +
		m[0][1]*m[1][0]*m[2][3]*m[3][2] - m[0][0]*m[1][1]*m[2][3]*m[3][2] -
		m[0][2]*m[1][1]*m[2][0]*m[3][3] + m[0][1]*m[1][2]*m[2][0]*m[3][3] +
		m[0][2]*m[1][0]*m[2][1]*m[3][3] - m[0][0]*m[1][2]*m[2][1]*m[3][3] -
		m[0][1]*m[1][0]*m[2][2]*m[3][3] + m[0][0]*m[1][1]*m[2][2]*m[3][3]
}

func radians(d float64) float64 { return d * math.Pi / 180 }

func nzf(d int) float64 {
	if d == 0 {
		return -1
	}
	return 1
}

type model struct {
	cmt  string
	pts  []vec3
	vtx  []vec3
	txc  []vec3
	nrm  []vec3
	cntr vec3
}

func writeobj(name string, m *model) {
	f, err := os.Create(name)
	ck(err)
	defer f.Close()

	w := bufio.NewWriter(f)
	defer w.Flush()

	if m.cmt != "" {
		fmt.Fprintf(w, "# %v\n", m.cmt)
	}
	if m.nrm == nil {
		fixwind(m.cntr, m.vtx, m.txc)
		m.nrm = trinrm(m.vtx)
	}

	fmt.Fprintf(w, "# v %v vt %v vn %v f %v\n",
		len(m.vtx)+len(m.pts), len(m.txc), len(m.nrm), len(m.vtx)/3)
	for _, v := range m.pts {
		fmt.Fprintf(w, "v %.4f %.4f %.4f\n", v.x, v.y, v.z)
	}
	for _, v := range m.vtx {
		fmt.Fprintf(w, "v %.4f %.4f %.4f\n", v.x, v.y, v.z)
	}
	for _, t := range m.txc {
		fmt.Fprintf(w, "vt %.4f %.4f %.4f\n", t.x, t.y, t.z)
	}
	for _, n := range m.nrm {
		fmt.Fprintf(w, "vn %.4f %.4f %.4f\n", n.x, n.y, n.z)
	}
	if *pointcloud {
		return
	}
	for e := 0; e < len(m.vtx); e += 3 {
		i := len(m.pts) + e
		fmt.Fprintf(w, "f %d %d %d\n", i+1, i+2, i+3)
	}
}

func fan2tri(f []vec3) []vec3 {
	n := len(f)
	if n < 3 {
		return nil
	}

	var t []vec3
	for i := 1; i < n-1; i++ {
		t = append(t, f[0], f[i], f[i+1])
	}
	return t
}

func strip2tri(s []vec3) []vec3 {
	n := len(s)
	if n < 3 {
		return nil
	}

	var t []vec3
	for i := 0; i < n-2; i++ {
		t = append(t, s[i], s[i+1], s[i+2])
	}
	return t
}

// https://stackoverflow.com/questions/40454789/computing-face-normals-and-winding
// since we decompose everything into triangles we can just compute the normals
// on the triangle, however when we generate the shape, the triangle winding orders
// might be different for each of them, we need to make sure they all have the same
// winding order, the way to do that is to make a 4x4 matrix where the last point is
// our center point (viewpoint) and find the determinant of that matrix
// that will tell us the orientation of the triangle with respect to the viewpoint
// if the determinant is positive, it points outward
// if the determinant is negative, it points inward
// if the determinant is zero, it lies on the same plane
// to flip the sign of the determinant (therefore the winding order), we need to swap the elements
// the determinant doesn't change if we shift the elements though
// if we don't want to do this, we need to compute the normals directly when we are triangulating the shape
// using the shape equation itself
func fixwind(c vec3, v, t []vec3) {
	for i := 0; i < len(v); i += 3 {
		M := [][]float64{
			{v[i].x, v[i].y, v[i].z, 1},
			{v[i+1].x, v[i+1].y, v[i+1].z, 1},
			{v[i+2].x, v[i+2].y, v[i+2].z, 1},
			{c.x, c.y, c.z, 1},
		}
		d := det44(M)
		if d < 0 {
			v[i], v[i+1] = v[i+1], v[i]
			if t != nil {
				t[i], t[i+1] = t[i+1], t[i]
			}
		}
	}
}

func trinrm(v []vec3) []vec3 {
	n := make([]vec3, len(v))
	for i := 0; i < len(v); i += 3 {
		p := sub3(v[i], v[i+1])
		q := sub3(v[i], v[i+2])
		n[i] = norm3(cross3(p, q))
	}
	return n
}

func gencube(c vec3, s float64) *model {
	// given a center point of a cube, all of the vertices at the cube edge can be figured out by
	// vector = [-1**(n&1), -1**(n&2), -1**(n&4)]
	// vertex = center + vector*length
	// n = [0..7]
	var pts []vec3
	for i := 0; i < 8; i++ {
		p := vec3{
			nzf(i & 1),
			nzf(i & 2),
			nzf(i & 4),
		}
		pts = append(pts, add3(c, scale3(p, s/2)))
	}

	var vtx []vec3
	faces := [][]int{
		{0, 4, 6, 2},
		{1, 3, 7, 5},
		{0, 1, 5, 4},
		{2, 6, 7, 3},
		{0, 2, 3, 1},
		{4, 5, 7, 6},
	}
	for _, f := range faces {
		vtx = append(vtx, pts[f[0]], pts[f[1]], pts[f[2]])
		vtx = append(vtx, pts[f[2]], pts[f[3]], pts[f[0]])
	}

	cmt := fmt.Sprintf("cube center=%v length=%.4f", c, s)
	return &model{
		cmt:  cmt,
		vtx:  vtx,
		cntr: c,
	}
}

func gentetrahedron(c vec3, l float64) *model {
	pts := []vec3{
		// Front face
		{0.0, 1.0, 0.0},
		{-1.0, -1.0, 1.0},
		{1.0, -1.0, 1.0},
		// Back face
		{0.0, 1.0, 0.0},
		{1.0, -1.0, -1.0},
		{-1.0, -1.0, -1.0},
		// Left face
		{0.0, 1.0, 0.0},
		{-1.0, -1.0, -1.0},
		{-1.0, -1.0, 1.0},
		// Right face
		{0.0, 1.0, 0.0},
		{1.0, -1.0, 1.0},
		{1.0, -1.0, -1.0},
		// Bottom face 1
		{0.0, -1.0, 1.0},
		{1.0, -1.0, 1.0},
		{1.0, -1.0, -1.0},
		{1.0, -1.0, -1.0},
		{0.0, -1.0, -1.0},
		{0.0, -1.0, 1.0},
		// Bottom face 2
		{-1.0, -1.0, 1.0},
		{1.0, -1.0, 1.0},
		{1.0, -1.0, -1.0},
		{1.0, -1.0, -1.0},
		{-1.0, -1.0, -1.0},
		{-1.0, -1.0, 1.0},
	}

	var vtx []vec3
	for _, p := range pts {
		vtx = append(vtx, add3(c, scale3(p, l/2)))
	}

	cmt := fmt.Sprintf("tetrahedron center=%v length=%.4f", c, l)
	return &model{
		cmt:  cmt,
		vtx:  vtx,
		cntr: c,
	}
}

// https://www.freemancw.com/2012/06/opengl-cone-function/
func gencone(d, a vec3, h, r float64, n int) *model {
	c := add3(a, scale3(neg3(d), h))
	e0 := perp3(d)
	e1 := cross3(e0, d)
	da := radians(360 / float64(n))

	pts := make([]vec3, n+1)
	for i := 0; i <= n; i++ {
		rad := da * float64(i)
		p0 := scale3(e0, math.Cos(rad)*r)
		p1 := scale3(e1, math.Sin(rad)*r)
		pts[i] = add3(c, add3(p0, p1))
	}

	// apex to the rotating directix to generate the sides
	sdfan := []vec3{a}
	for i := 0; i <= n; i++ {
		sdfan = append(sdfan, pts[i])
	}

	// center point at the circular base to the rotating directix for the bottom circle
	btfan := []vec3{c}
	for i := n; i >= 0; i-- {
		btfan = append(btfan, pts[i])
	}

	var vtx []vec3
	vtx = append(vtx, fan2tri(sdfan)...)
	vtx = append(vtx, fan2tri(btfan)...)

	cmt := fmt.Sprintf("cone directix=%v apex=%v height=%.4f rad=%.4f slices=%d", d, a, h, r, n)
	return &model{
		cmt:  cmt,
		vtx:  vtx,
		cntr: c,
	}
}

// https://mathworld.wolfram.com/Torus.html
func gentorus(c vec3, ri, ro float64, n, m int) *model {
	// split the torus up into little circular rings/tubes
	var pts []vec3
	du := radians(360 / float64(n))
	dv := radians(360 / float64(m))
	u := 0.0
	for i := 0; i <= n; i++ {
		// one iteration makes one circular ring/tube
		// connect them all together in a circular fashion
		// to make a torus
		su, cu := math.Sincos(u)

		v := 0.0
		for j := 0; j <= m; j++ {
			sv, cv := math.Sincos(v)

			p := vec3{
				(ri + ro*cv) * cu,
				(ri + ro*cv) * su,
				ro * sv,
			}
			pts = append(pts, add3(c, p))

			v += dv
		}
		u += du
	}

	var vtx, strip []vec3
	o := 0
	for i := 0; i < n; i++ {
		for j := 0; j <= m; j++ {
			strip = append(strip, pts[o], pts[o+m+1])
			o++
		}
		vtx = append(vtx, strip2tri(strip)...)
		strip = strip[:0]
	}

	cmt := fmt.Sprintf("torus center=%v inner_radius=%.4f outer_radius=%.4f main_segments=%v tube_segments=%v", c, ri, ro, n, m)
	return &model{
		cmt:  cmt,
		vtx:  vtx,
		cntr: c,
	}
}

// https://mathworld.wolfram.com/Cylinder.html
func gencylinder(c vec3, r, h float64, n int) *model {
	var pts [][2]float64
	u := 0.0
	du := radians(360 / float64(n))
	for i := 0; i <= n; i++ {
		s, c := math.Sincos(u)
		pts = append(pts, [2]float64{r * c, r * s})
		u += du
	}

	// sides
	var side []vec3
	for i := 0; i <= n; i++ {
		p := pts[i]
		t := vec3{p[0], h / 2, p[1]}
		b := vec3{p[0], -h / 2, p[1]}
		side = append(side, add3(c, t), add3(c, b))
	}

	// top
	var top []vec3
	top = append(top, add3(c, vec3{0, h / 2, 0}))
	for i := 0; i <= n; i++ {
		p := pts[i]
		t := vec3{p[0], h / 2, -p[1]}
		top = append(top, add3(c, t))
	}

	// bottom
	var bot []vec3
	bot = append(bot, add3(c, vec3{0, -h / 2, 0}))
	for i := 0; i <= n; i++ {
		p := pts[i]
		b := vec3{p[0], -h / 2, p[1]}
		bot = append(bot, add3(c, b))
	}

	var vtx []vec3
	vtx = append(vtx, strip2tri(side)...)
	vtx = append(vtx, fan2tri(top)...)
	vtx = append(vtx, fan2tri(bot)...)

	cmt := fmt.Sprintf("cylinder center=%v radius=%.4f height=%.4f slices=%v", c, r, h, n)
	return &model{
		cmt:  cmt,
		vtx:  vtx,
		cntr: c,
	}
}

// https://mathworld.wolfram.com/Sphere.html
func gensphere(c vec3, r float64, n, m int) *model {
	var slices, stacks [][2]float64
	u := 0.0
	du := 2 * math.Pi / float64(n)
	for i := 0; i <= n; i++ {
		s, c := math.Sincos(u)
		slices = append(slices, [2]float64{c, s})
		u += du
	}

	v := math.Pi / 2
	dv := -math.Pi / float64(m)
	for i := 0; i <= m; i++ {
		s, c := math.Sincos(v)
		stacks = append(stacks, [2]float64{c, s})
		v += dv
	}

	// the sphere is split up into a grid where stacks
	// represent the up/down movement and slices are sideways
	// generate points on them and connect them using a triangle
	// strip to obtain the sphere
	// then for the poles (near the top of the sphere, we connect them using triangles) to represent a 2d disk
	// to get less distortion due to the fact that they are so close
	var pts []vec3
	for i := 0; i <= m; i++ {
		for j := 0; j <= n; j++ {
			p := vec3{
				r * stacks[i][0] * slices[j][0],
				r * stacks[i][1],
				r * stacks[i][0] * slices[j][1],
			}
			pts = append(pts, add3(c, p))
		}
	}

	// north pole
	var vtx []vec3
	var north []vec3
	for i := 0; i < n; i++ {
		north = append(north, pts[i], pts[i+n+1], pts[i+n+2])
	}
	vtx = append(vtx, north...)

	// body (the sides of the sphere)
	var body []vec3
	o := 0
	for i := 0; i < m; i++ {
		for j := 0; j <= n; j++ {
			body = append(body, pts[o+j], pts[o+n+1+j])
		}
		o += n + 1
		vtx = append(vtx, strip2tri(body)...)
		body = body[:0]
	}

	// south pole
	var south []vec3
	o = (m+1)*(n+1) - 2*(n+1)
	for i := 0; i < n; i++ {
		south = append(south, pts[o+i], pts[o+i+1], pts[o+i+n+1])
	}
	vtx = append(vtx, south...)

	cmt := fmt.Sprintf("sphere center=%v radius=%.4f slices=%v stacks=%v", c, r, n, m)
	return &model{
		cmt:  cmt,
		vtx:  vtx,
		cntr: c,
	}
}

// https://mathinsight.org/plane_parametrization
func genplane(c, a, b vec3, l float64) *model {
	var vtx []vec3
	pts := [][2]float64{
		// first triangle
		{-1, -1},
		{1, -1},
		{1, 1},

		// second triangle
		{1, 1},
		{-1, 1},
		{-1, -1},
	}
	// plane parametric equation is
	// c + s*a + t*b
	// where c is a point on the plane
	// a and b are orthogonal vectors that are parallel to the plane
	for _, p := range pts {
		u := scale3(a, l/2*p[0])
		v := scale3(b, l/2*p[1])
		vtx = append(vtx, add3(c, add3(u, v)))
	}

	cmt := fmt.Sprintf("plane center=%v x_axis=%v y_axis=%v length=%.4f", c, a, b, l)
	return &model{
		cmt:  cmt,
		vtx:  vtx,
		cntr: c,
	}
}

func evalsh(theta, phi float64, m []float64) vec3 {
	r := 0.0
	r += math.Pow(math.Sin(m[0]*phi), m[1])
	r += math.Pow(math.Cos(m[2]*phi), m[3])
	r += math.Pow(math.Sin(m[4]*theta), m[5])
	r += math.Pow(math.Cos(m[6]*theta), m[7])
	return vec3{
		r * math.Sin(phi) * math.Cos(theta),
		r * math.Cos(phi),
		r * math.Sin(phi) * math.Sin(theta),
	}
}

// http://paulbourke.net/geometry/sphericalh/index.html
func gensh(c vec3, m []float64, n int) *model {
	var vtx []vec3
	du := 2 * math.Pi / float64(n)
	dv := math.Pi / float64(n)
	for i := 0; i < n; i++ {
		u := float64(i) * du
		for j := 0; j < n; j++ {
			v := float64(j) * dv

			p := []vec3{
				evalsh(u, v, m),
				evalsh(u+du, v, m),
				evalsh(u+du, v+dv, m),
				evalsh(u, v+dv, m),
			}
			for i := range p {
				p[i] = add3(p[i], c)
			}

			vtx = append(vtx, p[0], p[1], p[2])
			vtx = append(vtx, p[2], p[3], p[0])
		}
	}

	cmt := fmt.Sprintf("spherical_harmonics center=%v m=%v slices=%v", c, m, n)
	return &model{
		cmt: cmt,
		vtx: vtx,
	}
}

// http://paulbourke.net/fractals/rossler/index.html
// https://en.wikipedia.org/wiki/R%C3%B6ssler_attractor
func genrossler(cn, pl vec3, h, a, b, c float64, m, n int) *model {
	var pts []vec3
	pi := pl
	for i := 0; i < n; i++ {
		// euler integration
		p := vec3{
			pi.x + h*(-pi.y-pi.z),
			pi.y + h*(pi.x+a*pi.y),
			pi.z + h*(b+pi.z*(pi.x-c)),
		}
		if i > m {
			pts = append(pts, add3(cn, p))
		}
		pi = p
	}

	cmt := fmt.Sprintf("rossler_attractor center=%v initial=%v h=%.4f a=%.4f b=%.4f c=%.4f threshold=%v iters=%v",
		cn, pl, h, a, b, c, m, n)
	return &model{
		cmt: cmt,
		pts: pts,
	}
}

// http://paulbourke.net/fractals/lorenz/
// https://en.wikipedia.org/wiki/Lorenz_system
func genlorenz(cn, pl vec3, h, o, p, b float64, m, n int) *model {
	var pts []vec3
	pi := pl
	for i := 0; i < n; i++ {
		pt := vec3{
			pi.x + h*(o*(pi.y-pi.x)),
			pi.y + h*(pi.x*(p-pi.z)-pi.y),
			pi.z + h*(pi.x*pi.y-b*pi.z),
		}
		if i > m {
			pts = append(pts, add3(cn, pt))
		}
		pi = pt
	}

	cmt := fmt.Sprintf("lorenz_attractor center=%v initial=%v h=%.4f o=%.4f p=%.4f b=%.4f threshold=%v iters=%v",
		cn, pl, h, o, p, b, m, n)
	return &model{
		cmt: cmt,
		pts: pts,
	}
}

// http://paulbourke.net/fractals/clifford/
func genclifford(cn, pl vec3, a, b, c, d float64, n int) *model {
	pts := []vec3{add3(pl, cn)}
	pi := pl
	for i := 0; i < n; i++ {
		pi.x = math.Sin(a*pi.y) + c*math.Cos(a*pi.x)
		pi.y = math.Sin(b*pi.x) + d*math.Cos(b*pi.y)
		pts = append(pts, add3(pi, cn))
	}

	cmt := fmt.Sprintf("clifford_attractor center=%v initial=%v a=%.4f b=%.4f c=%.4f d=%.4f iters=%v",
		cn, pl, a, b, c, d, n)
	return &model{
		cmt: cmt,
		pts: pts,
	}
}

// https://mathworld.wolfram.com/HopfMap.html
// https://nilesjohnson.net/hopf-articles/Lyons_Elem-intro-Hopf-fibration.pdf
func genhopf(cn vec3, r float64, n int) *model {
	var pts []vec3
	s := 1 / float64(n)
	for a := -r; a <= r; a += s {
		for b := -r; b <= r; b += s {
			for c := -r; c <= r; c += s {
				for d := -r; d <= r; d += s {
					if a*a+b*b+c*c+d*d <= r*r {
						p := vec3{
							2 * (a*b + c*d),
							2 * (a*d - b*c),
							(a*a + b*b) - (c*c + d*d),
						}
						pts = append(pts, add3(cn, p))
					}
				}
			}
		}
	}

	cmt := fmt.Sprintf("hopf_fibration center=%v radius=%.4f slices=%v", cn, r, n)
	return &model{
		cmt: cmt,
		pts: pts,
	}
}
