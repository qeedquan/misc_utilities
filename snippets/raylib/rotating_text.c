/*

The idea is to render text to a texture and draw the texture as a
triangle/quad in 3D space so we can apply transformations to it

*/

#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

int
main(void)
{
	float width = 1200;
	float height = 800;

	Camera3D camera = { 0 };
	camera.position = (Vector3){ width / 2.0, height / 2.0, -(height / 2.0) / tan(PI * 30.0 / 180.0) };
	camera.target = (Vector3){ width / 2.0, height / 2.0, 0 };
	camera.up = (Vector3){ 0.0f, -1.0f, 0.0f };

	camera.fovy = 60.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	InitWindow(width, height, "Rotating Text");

	RenderTexture2D target = LoadRenderTexture(width, height);
	SetTextureFilter(target.texture, TEXTURE_FILTER_TRILINEAR);

	SetTargetFPS(60);

	Vector3 angle = {};
	while (!WindowShouldClose()) {
		angle.z = fmod(angle.z + 1, 360);

		BeginTextureMode(target);
		DrawText("Hello\nWorld\nGood to Be Here\nNumber One Reason", 0, 0, 32, WHITE);
		EndTextureMode();

		BeginDrawing();

		BeginMode3D(camera);

		ClearBackground(GRAY);

		Vector2 offset = { -50, -50 };
		rlTranslatef((width / 2) + offset.x, (height / 2) + offset.y, 0);
		rlRotatef(angle.z, 0, 0, 1);
		DrawTextureRec(target.texture, (Rectangle){ 0, 0, target.texture.width, -target.texture.height }, (Vector2){ 0, 0 }, WHITE);

		EndMode3D();

		EndDrawing();
	}
	UnloadRenderTexture(target);

	CloseWindow();

	return 0;
}
