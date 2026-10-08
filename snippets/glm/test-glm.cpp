#define GLM_ENABLE_EXPERIMENTAL
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "linalg.h"

using namespace std;

glm::mat4 mat2glm(Mat4f m)
{
	auto p = &m[0][0];
	return glm::mat4{
		p[0],
		p[1],
		p[2],
		p[3],
		p[4],
		p[5],
		p[6],
		p[7],
		p[8],
		p[9],
		p[10],
		p[11],
		p[12],
		p[13],
		p[14],
		p[15],
	};
}

void hexdump(void *buf, size_t len)
{
	auto ptr = (unsigned char *)buf;
	for (size_t i = 0; i < len; i++)
	{
		printf("%02X ", ptr[i]);
		if ((i & 15) == 15)
			printf("\n");
	}
	printf("\n");
}

void vecdump(void *buf)
{
	auto ptr = (float *)buf;
	printf("%.3f %.3f %.3f\n", ptr[0], ptr[1], ptr[2]);
	printf("\n");
}

void matdump(void *buf)
{
	auto ptr = (float *)buf;
	auto k = 0;
	for (auto i = 0; i < 4; i++)
	{
		for (auto j = 0; j < 4; j++)
			printf("%.3f ", ptr[k++]);
		printf("\n");
	}
	printf("\n");
}

void test_print()
{
	cout << "# Test Print" << endl;

	auto e1 = glm::mat4(1);
	auto v1 = glm::vec3(1, 2, 3);
	auto v2 = glm::vec4(1, 2, 3, 4);

	cout << glm::to_string(e1) << endl;
	cout << glm::to_string(v1) << endl;
	cout << glm::to_string(v2) << endl;
	cout << endl;
}

void test_rotation()
{
	cout << "# Test Rotation" << endl;

	auto M = glm::mat4(1);
	for (auto i = 0; i < 10; i++)
	{
		auto v = glm::normalize(glm::vec3(drand48(), drand48(), drand48()));
		float r = drand48() * 2 * M_PI;
		auto R = glm::rotate(M, r, v);
		cout << glm::to_string(v) << " " << r << endl;
		cout << glm::to_string(R) << endl;
		cout << endl;
	}

	auto R = glm::rotate(M, glm::radians(90.0f), glm::vec3(0, 0, 1));
	cout << glm::to_string(R) << endl;
	cout << endl;
}

void test_translation()
{
	cout << "# Test Translation" << endl;

	auto M = glm::mat4(1);
	auto T = glm::translate(M, glm::vec3(205.1, 4, 10));
	cout << glm::to_string(T) << endl;
	cout << T[3][0] << endl;
	cout << T[3][1] << endl;
	cout << T[3][2] << endl;
	cout << endl;
}

void test_projection()
{
	cout << "# Test Projection" << endl;

	auto w = 1024.0f;
	auto h = 768.0f;
	auto P = glm::perspective(45.0f, w / h, 0.5f, 1500.0f);
	auto M = glm::ortho(0.0f, w, 0.0f, h);
	auto L = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cout << glm::to_string(P) << endl;
	cout << glm::to_string(M) << endl;
	cout << glm::to_string(L) << endl;
	cout << endl;
}

void test_raw_data()
{
	cout << "# Test Raw Data" << endl;

	auto T = glm::translate(glm::mat4(1), glm::vec3(5.0f, 6.0f, 7.0f));
	hexdump(&T, sizeof(T));

	glm::mat4 M{
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8,
		9,
		10,
		11,
		12,
		13,
		14,
		15,
		16,
	};
	hexdump(&M, sizeof(M));
	cout << endl;

	glm::vec3 pts[] = {
		glm::vec3(-12.0f, 7.0f, 0.0f),
		glm::vec3(12.0f, 7.0f, 0.0f),
		glm::vec3(12.0f, -7.0f, 0.0f),
		glm::vec3(-12.0f, -7.0f, 0.0f)
	};

	for (auto p : pts)
	{
		glm::mat4 M = glm::mat4(1.0);
		M = glm::translate(M, p);
		M = glm::rotate(M, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));
		hexdump(&M, sizeof(M));
	}

	glm::vec3 vp[] = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
	};
	hexdump(vp, sizeof(vp));
	cout << endl;
}

void test_data_layout()
{
	cout << "# Test Data Layout" << endl;

	Mat4f M = { {
		{ 1, 2, 3, 4 },
		{ 5, 6, 7, 8 },
		{ 9, 10, 11, 12 },
		{ 13, 14, 15, 16 },
	} };
	auto N = mat2glm(M);
	hexdump(&M, sizeof(M));
	hexdump(&N, sizeof(N));

	// we want to keep our matrix looking like math notation, but GL expects column order
	// the slow way to get this right is to transpose all matrices when doing the xforms
	// or if we want to batch it, we can do all the xforms and then transpose at the end
	// though if we were using opengl, we can set a flag for it to transpose it automatically
	// us when we upload the matrix
	printf("translate\n");
	M = Mat4f::translate(Mat4f::eye(3), { 17, 13, 34 });
	N = glm::translate(glm::mat4(3), { 17, 13, 34 });
	auto MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	printf("scale\n");
	M = Mat4f::scale(M, { 4, 3, 70 });
	N = glm::scale(N, glm::vec3(4.0f, 3.0f, 70.0f));
	MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	printf("translate\n");
	M = Mat4f::translate(M, { 45, 6, 2 });
	N = glm::translate(N, { 45, 6, 2 });
	MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	printf("transpose\n");
	M = Mat4f::transpose(M);
	N = glm::transpose(N);
	MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	printf("rotation 1\n");
	M = Mat4f::rotate(M, 3.0f, { 45.0f, 6.0f, 2.0f });
	N = glm::rotate(N, 3.0f, { 45.0f, 6.0f, 2.0f });
	MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	printf("rotation 2\n");
	M = Mat4f::rotate(M, 6.0f, { 4.0f, 63.0f, 12.0f });
	N = glm::rotate(N, 6.0f, { 4.0f, 63.0f, 12.0f });
	MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	printf("ortho\n");
	M = Mat4f::ortho(-400.0f, 400.0f, 120.0f, -1200.0f, -100.0f, 100.0f);
	N = glm::ortho(-400.0f, 400.0f, 120.0f, -1200.0f, -100.0f, 100.0f);
	MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	printf("perspective\n");
	M = Mat4f::perspective(radians(45.0f), 4.0f / 3.0f, 0.15f, 1500.0f);
	N = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.15f, 1500.0f);
	MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	printf("lookat\n");
	M = Mat4f::lookAt({ 1, 2, 3 }, { 4, 5, 6 }, { 4, 15, 490 });
	N = glm::lookAt(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(4.0f, 5.0f, 6.0f), glm::vec3(4.0f, 15.0f, 490.0f));
	MT = Mat4f::transpose(M);
	matdump(&MT);
	matdump(&N);

	auto V1 = M * Vec3f{ 5.3, 76.5, 49.1 };
	auto V2 = glm::vec3(N * glm::vec4(5.3, 76.5, 49.1, 1));
	vecdump(&V1);
	vecdump(&V2);
}

/*

Test projection of 2D points onto the screen

The coordinate system input for GL is [-1, -1, -1] to [1, 1, 1] (CUBE) where [0, 0, 0] is the center

When the modelview and projection matrix is an identity matrix, the coordinates is just a linear remap.
[-1, -1, z] maps to [0, 0]
[1, 1, z] maps to [width, height]
[0, 0, z] maps to [width/2, height/2]

Notice that the Z coordinate is ignored here, it can be any value (GLM projects the Z to around z/2)

When there is a perspective projection matrix however, the z values matter because of the perspective divide
Note that when the z coordinate is near znear the values blows up because perspective division is not linear
Because you are dividing by a variable depth (Z), the operation behaves like a hyperbola f(z) = 1/z

*/

void test_project_2d()
{
	for (float z = -1000; z <= 1000; z += 0.5)
	{
		glm::vec3 worldPosLeft(-1.0f, -1.0f, z);
		glm::vec3 worldPosCenter(0.0f, 0.0f, z);
		glm::vec3 worldPosRight(1.0f, 1.0f, z);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f); //glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::vec4 viewport(0.0f, 0.0f, 800.0f, 600.0f);
		glm::vec3 screenPosLeft = glm::project(worldPosLeft, model, projection, viewport);
		glm::vec3 screenPosCenter = glm::project(worldPosCenter, model, projection, viewport);
		glm::vec3 screenPosRight = glm::project(worldPosRight, model, projection, viewport);
		printf("%f %f %f | ", screenPosLeft.x, screenPosLeft.y, screenPosLeft.z);
		printf("%f %f %f | ", screenPosCenter.x, screenPosCenter.y, screenPosCenter.z);
		printf("%f %f %f\n", screenPosRight.x, screenPosRight.y, screenPosRight.z);
	}

	for (float z = -100; z <= 100; z += 0.01)
	{
		glm::vec3 worldPosLeft(-1.0f, -1.0f, z);
		glm::vec3 worldPosCenter(0.0f, 0.0f, z);
		glm::vec3 worldPosRight(1.0f, 1.0f, z);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::vec4 viewport(0.0f, 0.0f, 800.0f, 600.0f);
		glm::vec3 screenPosLeft = glm::project(worldPosLeft, model, projection, viewport);
		glm::vec3 screenPosCenter = glm::project(worldPosCenter, model, projection, viewport);
		glm::vec3 screenPosRight = glm::project(worldPosRight, model, projection, viewport);
		printf("%f %f %f | ", screenPosLeft.x, screenPosLeft.y, screenPosLeft.z);
		printf("%f %f %f | ", screenPosCenter.x, screenPosCenter.y, screenPosCenter.z);
		printf("%f %f %f | ", screenPosRight.x, screenPosRight.y, screenPosRight.z);
		printf("%f\n", z);
	}
}

int main()
{
	srand(time(NULL));
	test_print();
	test_rotation();
	test_translation();
	test_projection();
	test_raw_data();
	test_data_layout();
	test_project_2d();

	return 0;
}
