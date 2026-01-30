#include "Draws.h"
#include "Spatials.h"
#include "Shapes.h"
#include "raylib.h"
#include "rlgl.h"
#include <math.h>

static void Draw_Circle(ecs_iter_t *it)
{
	SpatialsPosition2World *p = ecs_field(it, SpatialsPosition2World, 0); // self, in
	ShapesCircle *c = ecs_field(it, ShapesCircle, 1);                     // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++c) {
		//DrawCircleV((Vector2){p->x, p->y}, c->r, RED);
	}
}

static void Draw_Rectangle(ecs_iter_t *it)
{
	SpatialsPosition2World *p = ecs_field(it, SpatialsPosition2World, 0); // self, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 1);               // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++r) {
		//DrawRectangleV((Vector2){p->x - r->w / 2, p->y - r->h / 2}, (Vector2){r->w, r->h}, BLUE);
		//DrawLineEx((Vector2){p->x, p->y}, (Vector2){p->x + r->w, p->y}, 1.0f, RED);   // Draw X line
		//DrawLineEx((Vector2){p->x, p->y}, (Vector2){p->x, p->y - r->h}, 1.0f, GREEN); // Draw Y line
	}
}

void generate_6vertices_rectangle(Vector2 *v, float w, float h)
{
	// First triangle
	v[0].x = -w / 2;
	v[0].y = -h / 2;
	v[1].x = w / 2;
	v[1].y = -h / 2;
	v[2].x = w / 2;
	v[2].y = h / 2;
	// Second triangle
	v[3].x = -w / 2;
	v[3].y = -h / 2;
	v[4].x = w / 2;
	v[4].y = h / 2;
	v[5].x = -w / 2;
	v[5].y = h / 2;
}

void rotate_vertices(Vector2 *v, int count, float angle)
{
	float sinRot = sinf(angle * DEG2RAD);
	float cosRot = cosf(angle * DEG2RAD);
	for (int i = 0; i < count; i++) {
		float tx = v[i].x;
		float ty = v[i].y;
		v[i].x = cosRot * tx - sinRot * ty;
		v[i].y = sinRot * tx + cosRot * ty;
	}
}

void translate_vertices(Vector2 *v, int count, Vector2 translation)
{
	for (int i = 0; i < count; i++) {
		v[i].x += translation.x;
		v[i].y += translation.y;
	}
}

void draw_vertices(Vector2 *v, int count, Color color)
{
	rlBegin(RL_TRIANGLES);
	for (int i = 0; i < count; i++, ++v) {
		rlColor4ub(color.r, color.g, color.b, color.a);
		rlVertex2f(v->x, v->y);
	}
	rlEnd();
}

static void Draw_Rectangle_Rotated(ecs_iter_t *it)
{
	SpatialsPosition2World *p = ecs_field(it, SpatialsPosition2World, 0); // self, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 1);               // self, in
	SpatialsRotationWorld *rot = ecs_field(it, SpatialsRotationWorld, 2); // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++r, ++rot) {
		float x = p->x - r->w / 2;
		float y = p->y - r->h / 2;
		float w = r->w;
		float h = r->h;
		DrawRectangleV((Vector2){x, y}, (Vector2){w, h}, BLUE);
		DrawLineEx((Vector2){x, y}, (Vector2){x + w, y}, 1.0f, RED);   // Draw X line
		DrawLineEx((Vector2){x, y}, (Vector2){x, y + h}, 1.0f, GREEN); // Draw Y line


		Vector2 vertices[6] = {0};
		generate_6vertices_rectangle(vertices, r->w, r->h);
		rotate_vertices(vertices, 6, rot->angle);
		translate_vertices(vertices, 6, (Vector2){p->x, p->y});
		draw_vertices(vertices, 6, BLUE);


		
	}
/*
    Vector2 startingPositions[3] = {{ 400.0f, 150.0f }, { 300.0f, 300.0f }, { 500.0f, 300.0f }};
    Vector2 trianglePositions[3] = { startingPositions[0], startingPositions[1], startingPositions[2] };
                // Draw triangle as a triangle
                rlBegin(RL_TRIANGLES);
                    // One triangle, three points
                    // Define color for next vertex
                    rlColor4ub(255, 0, 0, 255);
                    // Define vertex
                    rlVertex2f(trianglePositions[0].x, trianglePositions[0].y);
                    rlColor4ub(0, 255, 0, 255);
                    rlVertex2f(trianglePositions[1].x, trianglePositions[1].y);
                    rlColor4ub(0, 0, 255, 255);
                    rlVertex2f(trianglePositions[2].x, trianglePositions[2].y);
                rlEnd();
				*/

}

void DrawsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Draws);
	ecs_set_name_prefix(world, "Draws");
	ECS_IMPORT(world, Spatials);
	ECS_IMPORT(world, Shapes);

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Draw_Circle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Draw_Circle,
	.query.terms = {
	{.id = ecs_id(SpatialsPosition2World), .inout = EcsIn},
	{.id = ecs_id(ShapesCircle), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Draw_Rectangle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Draw_Rectangle,
	.query.terms = {
	{.id = ecs_id(SpatialsPosition2World), .inout = EcsIn},
	{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
	{.id = ecs_id(SpatialsRotationWorld), .inout = EcsIn, .oper = EcsNot},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Draw_Rectangle_Rotated", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Draw_Rectangle_Rotated,
	.query.terms = {
	{.id = ecs_id(SpatialsPosition2World), .inout = EcsIn},
	{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
	{.id = ecs_id(SpatialsRotationWorld), .inout = EcsIn},
	}});
}