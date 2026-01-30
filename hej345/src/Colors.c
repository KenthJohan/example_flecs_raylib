#include "Colors.h"

ECS_COMPONENT_DECLARE(ColorsRgb);

void ColorsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Colors);
	ecs_set_name_prefix(world, "Colors");

	ECS_COMPONENT_DEFINE(world, ColorsRgb);

	ecs_struct(world,
	{.entity = ecs_id(ColorsRgb),
	.members = {
	{.name = "r", .type = ecs_id(ecs_f32_t)},
	{.name = "g", .type = ecs_id(ecs_f32_t)},
	{.name = "b", .type = ecs_id(ecs_f32_t)},
	}});
}