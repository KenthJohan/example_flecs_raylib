#include "Shapes.h"

ECS_COMPONENT_DECLARE(ShapesRectangle);
ECS_COMPONENT_DECLARE(ShapesRectangleScale);
ECS_COMPONENT_DECLARE(ShapesCircle);

static void System_ShapesRectangleScale(ecs_iter_t *it)
{
	ShapesRectangle *r0 = ecs_field(it, ShapesRectangle, 0);          // shared, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 1);           // self, in
	ShapesRectangleScale *s = ecs_field(it, ShapesRectangleScale, 2); // self, in
	for (int i = 0; i < it->count; ++i, ++r, ++s) {
		r->w = r0->w * s->w;
		r->h = r0->h * s->h;
	}
}

void ShapesImport(ecs_world_t *world)
{
	ECS_MODULE(world, Shapes);
	ecs_set_name_prefix(world, "Shapes");

	ECS_COMPONENT_DEFINE(world, ShapesRectangle);
	ECS_COMPONENT_DEFINE(world, ShapesRectangleScale);
	ECS_COMPONENT_DEFINE(world, ShapesCircle);

	ecs_struct(world,
	{.entity = ecs_id(ShapesRectangle),
	.members = {
	{.name = "w", .type = ecs_id(ecs_f32_t)},
	{.name = "h", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(ShapesRectangleScale),
	.members = {
	{.name = "w", .type = ecs_id(ecs_f32_t)},
	{.name = "h", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(ShapesCircle),
	.members = {
	{.name = "r", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_ShapesRectangleScale", .add = ecs_ids(ecs_dependson(EcsPreUpdate))}),
	.callback = System_ShapesRectangleScale,
	.query.terms = {
	{.id = ecs_id(ShapesRectangle), .src.id = EcsCascade, .inout = EcsIn},
	{.id = ecs_id(ShapesRectangle), .inout = EcsOut},
	{.id = ecs_id(ShapesRectangleScale), .inout = EcsIn},
	}});
}