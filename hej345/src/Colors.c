#include "Colors.h"

ECS_COMPONENT_DECLARE(ColorsRgb);
ECS_COMPONENT_DECLARE(ColorsWorldRgb);

static void Copy_Color(ecs_iter_t *it)
{
	ColorsWorldRgb *dst = ecs_field(it, ColorsWorldRgb, 0); // self, out
	ColorsRgb *src = ecs_field(it, ColorsRgb, 1);           // self, in
	for (int i = 0; i < it->count; ++i, ++dst, ++src) {
		dst->r = src->r;
		dst->g = src->g;
		dst->b = src->b;
	}
}

void ColorsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Colors);
	ecs_set_name_prefix(world, "Colors");

	ECS_COMPONENT_DEFINE(world, ColorsRgb);
	ECS_COMPONENT_DEFINE(world, ColorsWorldRgb);

	ecs_struct(world,
	{.entity = ecs_id(ColorsRgb),
	.members = {
	{.name = "r", .type = ecs_id(ecs_u8_t)},
	{.name = "g", .type = ecs_id(ecs_u8_t)},
	{.name = "b", .type = ecs_id(ecs_u8_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(ColorsWorldRgb),
	.members = {
	{.name = "r", .type = ecs_id(ecs_u8_t)},
	{.name = "g", .type = ecs_id(ecs_u8_t)},
	{.name = "b", .type = ecs_id(ecs_u8_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Copy_Color", .add = ecs_ids(ecs_dependson(EcsPreFrame))}),
	.callback = Copy_Color,
	.query.terms = {
	{.id = ecs_id(ColorsWorldRgb), .inout = EcsOut},
	{.id = ecs_id(ColorsRgb), .inout = EcsIn},
	}});
}