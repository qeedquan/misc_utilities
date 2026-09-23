#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

int
main(void)
{
	// If the width and height are not the same, the aspect ratio will differ and give different results
	// against processing, need to do more work
	const float width = 600;
	const float height = 600;

	InitWindow(width, height, "Processing 3D Camera Equivalent");

	// Simulates gluLookAt camera matrix
	// Raylib is right handed by default, so to convert to a left-handed one like Processing (with y-down)
	// Need to negate the z value in the eye position (position), and the up vector (y-down)
	Camera3D camera = { 0 };
	camera.position = (Vector3){ width / 2.0, height / 2.0, -(height / 2.0) / tan(PI * 30.0 / 180.0) };
	camera.target = (Vector3){ width / 2.0, height / 2.0, 0 };
	camera.up = (Vector3){ 0.0f, -1.0f, 0.0f };

	// The perspective matrix matches processing (raylib hardcodes near/far to a small/large value
	// shouldn't affect the display that much from processing; processing allows you to set near/far by yourself
	camera.fovy = 60.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

	SetTargetFPS(60);
	while (!WindowShouldClose()) {

		BeginDrawing();

		ClearBackground((Color){ 204, 204, 204, 255 });

		BeginMode3D(camera);

		rlTranslatef(200, 200, 0);

		// rotation are negated to match processing
		rlRotatef(45, 1, 0, 0);
		rlRotatef(-60, 0, 1, 0);

		float size = 150.0f;
		DrawCubeWires(cubePosition, size, size, size, GRAY);

		{
			rlPushMatrix();
			rlTranslatef(300, 0, 0);
			size = 50.0f;
			DrawCubeWires(cubePosition, size, size, size, GRAY);

			// there is a push matrix to save state here
			// in processing it is implicit
			{
				rlPushMatrix();
				rlTranslatef(0, 400, 0);
				rlRotatef(-36, 0, 0, 1);
				size = 120.0f;
				DrawCubeWires(cubePosition, size, size, size, GRAY);
				rlPopMatrix();
			}

			rlPopMatrix();
		}

		EndMode3D();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
