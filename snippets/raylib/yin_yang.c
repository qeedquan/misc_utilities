/*

https://en.wikipedia.org/wiki/Yin_and_yang

*/

#include <math.h>
#include <raylib.h>

void
tajitsu(Vector2 center, float radius)
{
	Vector2 center1;
	Vector2 center2;
	Color color1;
	Color color2;
	Color color3;
	float margin;
	int i;

	margin = 10.0;
	DrawCircleV(center, radius + margin, BLACK);
	DrawCircleSector(center, radius, 90.0, 270.0, 100, WHITE);
	DrawCircleSector(center, radius, 0.0, 90.0, 100, BLACK);
	DrawCircleSector(center, radius, 270.0, 360.0, 100, BLACK);

	center1 = center2 = center;
	color1 = WHITE;
	color2 = BLACK;
	for (i = 0; i < 2; i++) {
		if (i == 0)
			radius *= 0.5;
		else
			radius *= 0.25;

		center1.y -= radius;
		center2.y += radius;

		DrawCircleV(center1, radius, color1);
		DrawCircleV(center2, radius, color2);

		color3 = color1;
		color1 = color2;
		color2 = color3;
	}
}

int
main(void)
{
	Vector2 center;
	float radius;

	InitWindow(800, 800, "Taijitu");
	SetTargetFPS(60);
	SetConfigFlags(FLAG_MSAA_4X_HINT);

	center.x = GetRenderWidth() / 2.0;
	center.y = GetRenderHeight() / 2.0;
	radius = fmin(center.x, center.y) - 15.0;
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(WHITE);
		tajitsu(center, radius);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
