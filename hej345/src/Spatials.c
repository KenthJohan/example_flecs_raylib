#include "Spatials.h"

ECS_COMPONENT_DECLARE(SpatialsPosition2Local);
ECS_COMPONENT_DECLARE(SpatialsPosition2World);

static void Position2_Cascade(ecs_iter_t *it)
{
	SpatialsPosition2World *g = ecs_field(it, SpatialsPosition2World, 0);       // self, out
	SpatialsPosition2Local const *l = ecs_field(it, SpatialsPosition2Local, 1); // self, in
	SpatialsPosition2World const *p = ecs_field(it, SpatialsPosition2World, 2); // parent, in
	for (int i = 0; i < it->count; ++i, ++l, ++g) {
		g->x = l->x;
		g->y = l->y;
		if (p) {
			g->x += p->x;
			g->y += p->y;
		}
	}
}

void SpatialsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Spatials);
	ecs_set_name_prefix(world, "Spatials");

	ECS_COMPONENT_DEFINE(world, SpatialsPosition2Local);
	ECS_COMPONENT_DEFINE(world, SpatialsPosition2World);

	ecs_struct(world,
	{.entity = ecs_id(SpatialsPosition2Local),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsPosition2World),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Position2_Cascade", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Position2_Cascade,
	.query.terms = {
	{.id = ecs_id(SpatialsPosition2World), .inout = EcsOut},
	{.id = ecs_id(SpatialsPosition2Local), .inout = EcsIn},
	{.id = ecs_id(SpatialsPosition2World), .src.id = EcsCascade, .inout = EcsIn, .oper = EcsOptional},
	}});
}