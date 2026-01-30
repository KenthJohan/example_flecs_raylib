#include "Draws.h"
#include "Spatials.h"
#include "Shapes.h"
#include "raylib.h"

static void Draw_Circle(ecs_iter_t *it)
{
	SpatialsPosition2World *p = ecs_field(it, SpatialsPosition2World, 0); // self, in
	ShapesCircle *c = ecs_field(it, ShapesCircle, 1);                     // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++c) {
		DrawCircleV((Vector2){p->x, p->y}, c->r, RED);
	}
}

static void Draw_Rectangle(ecs_iter_t *it)
{
	SpatialsPosition2World *p = ecs_field(it, SpatialsPosition2World, 0); // self, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 1);               // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++r) {
		DrawRectangleV((Vector2){p->x - r->w / 2, p->y - r->h / 2}, (Vector2){r->w, r->h}, BLUE);
	}
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
	}});
}