#ifndef _LINALG_H_
#define _LINALG_H_

template <typename T>
T radians(T deg)
{
	return deg / 180 * M_PI;
}

template <typename T>
struct Vec3
{
	union
	{
		struct
		{
			T x, y, z;
		};
		struct
		{
			T r, g, b;
		};
		T v[3];
	};

	T operator[](int i)
	{
		return v[i];
	}

	Vec3 operator-()
	{
		return { -x, -y, -z };
	}

	Vec3 operator-(Vec3 v)
	{
		return { x - v.x, y - v.y, z - v.z };
	}

	T length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vec3 normalize()
	{
		auto l = length();
		return { x / l, y / l, z / l };
	}

	T dot(Vec3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vec3 cross(Vec3 v)
	{
		return {
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		};
	}
};

template <typename T>
struct Mat4
{
	T m[4][4];

	T *operator[](int i)
	{
		return m[i];
	}

	Mat4 operator*(const Mat4 &mat) const
	{
		Mat4 p = {};
		for (auto i = 0; i < 4; i++)
			for (auto j = 0; j < 4; j++)
				for (auto k = 0; k < 4; k++)
					p[i][j] += m[i][k] * mat.m[k][j];
		return p;
	}

	Mat4 &operator*=(const Mat4 &mat)
	{
		*this = *this * mat;
		return *this;
	}

	Vec3<T> operator*(Vec3<T> vec)
	{
		return Vec3<T>{
			m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3],
			m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3],
			m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3],
		};
	}

	static Mat4 transpose(const Mat4 &mat)
	{
		Mat4 p = {};
		for (auto i = 0; i < 4; i++)
			for (auto j = 0; j < 4; j++)
				p[j][i] = mat.m[i][j];
		return p;
	}

	static Mat4 eye(T val)
	{
		return Mat4{ {
			{ val, 0, 0, 0 },
			{ 0, val, 0, 0 },
			{ 0, 0, val, 0 },
			{ 0, 0, 0, val },
		} };
	}

	static Mat4 translate(const Mat4 &mat, Vec3<T> off)
	{
		auto xfm = Mat4{ {
			{ 1, 0, 0, off.x },
			{ 0, 1, 0, off.y },
			{ 0, 0, 1, off.z },
			{ 0, 0, 0, 1 },
		} };
		return mat * xfm;
	}

	static Mat4 scale(const Mat4 &mat, Vec3<T> scale)
	{
		auto xfm = Mat4{ {
			{ scale.x, 0, 0, 0 },
			{ 0, scale.y, 0, 0 },
			{ 0, 0, scale.z, 0 },
			{ 0, 0, 0, 1 },
		} };
		return mat * xfm;
	}

	// https://mathworld.wolfram.com/RodriguesRotationFormula.html
	static Mat4 rotate(const Mat4 &mat, T rad, Vec3<T> axis)
	{
		auto c = cos(rad);
		auto s = sin(rad);
		auto w = axis.normalize();
		auto xfm = Mat4{};
		xfm[0][0] = c + w.x * w.x * (1 - c);
		xfm[0][1] = w.x * w.y * (1 - c) - w.z * s;
		xfm[0][2] = w.y * s + w.x * w.z * (1 - c);
		xfm[1][0] = w.z * s + w.x * w.y * (1 - c);
		xfm[1][1] = c + w.y * w.y * (1 - c);
		xfm[1][2] = -w.x * s + w.y * w.z * (1 - c);
		xfm[2][0] = -w.y * s + w.x * w.z * (1 - c);
		xfm[2][1] = w.x * s + w.y * w.z * (1 - c);
		xfm[2][2] = c + w.z * w.z * (1 - c);
		xfm[3][3] = 1;
		return mat * xfm;
	}

	static Mat4 ortho(T left, T right, T bottom, T top, T near, T far)
	{
		auto sx = 2 / (right - left);
		auto sy = 2 / (top - bottom);
		auto sz = -2 / (far - near);
		auto tx = -(right + left) / (right - left);
		auto ty = -(top + bottom) / (top - bottom);
		auto tz = -(far + near) / (far - near);
		auto mat = Mat4{ {
			{ sx, 0, 0, tx },
			{ 0, sy, 0, ty },
			{ 0, 0, sz, tz },
			{ 0, 0, 0, 1 },
		} };
		return mat;
	}

	static Mat4 perspective(T fovy, T aspect, T znear, T zfar)
	{
		float f = 1 / tan(fovy / 2);
		auto sx = f / aspect;
		auto sy = f;
		auto sz = (zfar + znear) / (znear - zfar);
		auto tz = 2 * zfar * znear / (znear - zfar);
		auto mat = Mat4{ {
			{ sx, 0, 0, 0 },
			{ 0, sy, 0, 0 },
			{ 0, 0, sz, tz },
			{ 0, 0, -1, 0 },
		} };
		return mat;
	}

	static Mat4 lookAt(Vec3<T> eye, Vec3<T> center, Vec3<T> up)
	{
		auto f = (center - eye).normalize();
		auto u = up.normalize();
		auto s = f.cross(u).normalize();
		u = s.cross(f);
		auto mat = Mat4{ {
			{ s.x, s.y, s.z, 0 },
			{ u.x, u.y, u.z, 0 },
			{ -f.x, -f.y, -f.z, 0 },
			{ 0, 0, 0, 1 },
		} };
		return translate(mat, -eye);
	}
};

typedef Vec3<float> Vec3f;
typedef Mat4<float> Mat4f;

#endif
