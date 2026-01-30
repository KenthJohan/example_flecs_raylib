#include "Mice.h"
#include "Spatials.h"
#include "Shapes.h"
#include "Colors.h"

ECS_TAG_DECLARE(MiceCollide);
ECS_COMPONENT_DECLARE(MicePosition);

static void Test_Collision_Rectangle(ecs_iter_t *it)
{
	MicePosition const *m = ecs_field(it, MicePosition, 0);                     // singleton, in
	SpatialsTransform2 const *t = ecs_field(it, SpatialsTransform2, 1);         // self, in
	SpatialsPosition2World const *p = ecs_field(it, SpatialsPosition2World, 2); // self, in
	ShapesRectangle const *r = ecs_field(it, ShapesRectangle, 3);               // self, in

	for (int i = 0; i < it->count; ++i, ++t, ++p, ++r) {
		// Transform mouse position into local space of rectangle
		float sinRot = -t->c1[0]; // -sin(theta)
		float cosRot = t->c0[0];  // cos(theta)

		// Translate mouse into rectangle local space
		float localX = m->x - p->x;
		float localY = m->y - p->y;

		// Rotate point back inversely relative to the rectangle
		float rotatedX = cosRot * localX - sinRot * localY;
		float rotatedY = sinRot * localX + cosRot * localY;

		// Check if the local point lies within the bounds of the rectangle
		bool isColliding = (rotatedX >= -r->w / 2 && rotatedX <= r->w / 2 &&
		                    rotatedY >= -r->h / 2 && rotatedY <= r->h / 2);

		if (isColliding) {
			ecs_add_id(it->world, it->entities[i], ecs_id(MiceCollide));
		} else {
			ecs_remove_id(it->world, it->entities[i], ecs_id(MiceCollide));
		}
	}
}


static void Test_Collision_Circle(ecs_iter_t *it)
{
	MicePosition const *m = ecs_field(it, MicePosition, 0);                     // singleton, in
	SpatialsTransform2 const *t = ecs_field(it, SpatialsTransform2, 1);         // self, in
	SpatialsPosition2World const *p = ecs_field(it, SpatialsPosition2World, 2); // self, in
	ShapesCircle const *c = ecs_field(it, ShapesCircle, 3);               // self, in

	for (int i = 0; i < it->count; ++i, ++t, ++p, ++c) {
        // Transform mouse position into local space of circle
        float sinRot = -t->c1[0]; // -sin(theta)
        float cosRot = t->c0[0];  // cos(theta)

        // Translate mouse into circle local space
        float localX = m->x - p->x;
        float localY = m->y - p->y;

        // Rotate point back inversely relative to the circle
        float rotatedX = cosRot * localX - sinRot * localY;
        float rotatedY = sinRot * localX + cosRot * localY;

        // Check if the local point lies within the bounds of the circle
        bool isColliding = (rotatedX * rotatedX + rotatedY * rotatedY) <= (c->r * c->r);

        if (isColliding) {
            ecs_add_id(it->world, it->entities[i], ecs_id(MiceCollide));
        } else {
            ecs_remove_id(it->world, it->entities[i], ecs_id(MiceCollide));
        }
	}
}

static void Change_Color(ecs_iter_t *it)
{
	ColorsRgb *c = ecs_field(it, ColorsRgb, 0);
	for (int i = 0; i < it->count; ++i, ++c) {
		c->r = 1.0f - c->r;
		c->g = 1.0f - c->g;
		c->b = 1.0f - c->b;
	}
}

void MiceImport(ecs_world_t *world)
{
	ECS_MODULE(world, Mice);
	ecs_set_name_prefix(world, "Mice");
	ECS_IMPORT(world, Spatials);
	ECS_IMPORT(world, Shapes);
	ECS_IMPORT(world, Colors);

	ECS_COMPONENT_DEFINE(world, MicePosition);
	ECS_TAG_DEFINE(world, MiceCollide);

	ecs_struct(world,
	{.entity = ecs_id(MicePosition),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_singleton_add(world, MicePosition);

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Test_Collision_Rectangle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Test_Collision_Rectangle,
	.query.terms = {
	{.id = ecs_id(MicePosition), .src.id = ecs_id(MicePosition), .inout = EcsIn},
	{.id = ecs_id(SpatialsTransform2), .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(SpatialsPosition2World), .inout = EcsIn},
	{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Test_Collision_Circle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Test_Collision_Circle,
	.query.terms = {
	{.id = ecs_id(MicePosition), .src.id = ecs_id(MicePosition), .inout = EcsIn},
	{.id = ecs_id(SpatialsTransform2), .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(SpatialsPosition2World), .inout = EcsIn},
	{.id = ecs_id(ShapesCircle), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Change_Color", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Change_Color,
	.query.terms = {
	{.id = ecs_id(ColorsRgb), .inout = EcsIn},
	{.id = MiceCollide},
	}});
}