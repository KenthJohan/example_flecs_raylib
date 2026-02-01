#include "Renders.h"
#include "Spatials.h"
#include "Shapes.h"
#include "Colors.h"

ECS_COMPONENT_DECLARE(RendersCanvas2);

ECS_TAG_DECLARE(RendersGroup);

void RendersImport(ecs_world_t *world)
{
	ECS_MODULE(world, Renders);
	ECS_IMPORT(world, Spatials);
	ECS_IMPORT(world, Shapes);
	ECS_IMPORT(world, Colors);
	ecs_set_name_prefix(world, "Renders");

	ECS_COMPONENT_DEFINE(world, RendersCanvas2);
	ECS_TAG_DEFINE(world, RendersGroup);
}