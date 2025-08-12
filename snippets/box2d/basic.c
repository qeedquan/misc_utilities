#include <stdio.h>
#include <box2d/box2d.h>

int
main()
{
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = (b2Vec2){ 0.0f, -10.0f };

	b2WorldId worldId = b2CreateWorld(&worldDef);
	b2BodyDef groundBodyDef = b2DefaultBodyDef();
	groundBodyDef.position = (b2Vec2){ 0.0f, -10.0f };
	b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
	b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
	b2ShapeDef groundShapeDef = b2DefaultShapeDef();
	b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = (b2Vec2){ 0.0f, 4.0f };
	b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1.0f;
	shapeDef.material.friction = 0.3f;
	b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);

	float timeStep = 1.0f / 60.0f;
	int subStepCount = 4;

	for (int i = 0; i < 90; i++) {
		b2World_Step(worldId, timeStep, subStepCount);
		b2Vec2 position = b2Body_GetPosition(bodyId);
		b2Rot rotation = b2Body_GetRotation(bodyId);
		printf("%4.2f %4.2f %4.2f\n", position.x, position.y, b2Rot_GetAngle(rotation));
	}

	b2DestroyWorld(worldId);

	return 0;
}
