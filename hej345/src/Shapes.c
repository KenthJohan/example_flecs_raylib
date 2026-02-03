#include "Shapes.h"

ECS_COMPONENT_DECLARE(ShapesRectangle);
ECS_COMPONENT_DECLARE(ShapesCircle);
ECS_COMPONENT_DECLARE(ShapesCrank);



void ShapesImport(ecs_world_t *world)
{
	ECS_MODULE(world, Shapes);
	ecs_set_name_prefix(world, "Shapes");

	ECS_COMPONENT_DEFINE(world, ShapesRectangle);
	ECS_COMPONENT_DEFINE(world, ShapesCircle);
	ECS_COMPONENT_DEFINE(world, ShapesCrank);

	ecs_struct(world,
	{.entity = ecs_id(ShapesRectangle),
	.members = {
	{.name = "w", .type = ecs_id(ecs_f32_t)},
	{.name = "h", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(ShapesCircle),
	.members = {
	{.name = "r", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(ShapesCrank),
	.members = {
	{.name = "l", .type = ecs_id(ecs_f32_t)},
	{.name = "t", .type = ecs_id(ecs_f32_t)},
	}});
}