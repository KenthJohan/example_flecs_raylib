#include "Mice.h"
#include "Spatials.h"
#include "Shapes.h"
#include "Colors.h"

ECS_TAG_DECLARE(MiceCollide);
ECS_TAG_DECLARE(MiceToggle);
ECS_COMPONENT_DECLARE(MicePosition);
ECS_COMPONENT_DECLARE(MicePositionLocal);

static void Test_Collision_Rectangle(ecs_iter_t *it)
{
	MicePositionLocal const *m = ecs_field(it, MicePositionLocal, 0);           // shared, in
	SpatialsTransform2 const *t = ecs_field(it, SpatialsTransform2, 1);         // self, in
	SpatialsWorldPosition2 const *p = ecs_field(it, SpatialsWorldPosition2, 2); // self, in
	ShapesRectangle const *r = ecs_field(it, ShapesRectangle, 3);               // self, in

	for (int i = 0; i < it->count; ++i, ++t, ++p, ++r) {
		// Inverse transform mouse position to local space
		float dx = m->x - p->x;
		float dy = m->y - p->y;
		float local_x = t->c0[0] * dx + t->c0[1] * dy;
		float local_y = t->c1[0] * dx + t->c1[1] * dy;
		bool hit = (local_x >= -r->w / 2) && (local_x <= r->w / 2) && (local_y >= -r->h / 2) && (local_y <= r->h / 2);
		if (hit) {
			ecs_add_id(it->world, it->entities[i], ecs_id(MiceCollide));
		} else {
			ecs_remove_id(it->world, it->entities[i], ecs_id(MiceCollide));
		}
	}
}

static void Test_Collision_Circle(ecs_iter_t *it)
{
	MicePositionLocal const *m = ecs_field(it, MicePositionLocal, 0);           // shared, in
	SpatialsWorldPosition2 const *p = ecs_field(it, SpatialsWorldPosition2, 1); // self, in
	ShapesCircle const *c = ecs_field(it, ShapesCircle, 2);                     // self, in

	for (int i = 0; i < it->count; ++i, ++p, ++c) {
		// Calculate distance between mouse and circle center
		float dx = m->x - p->x;
		float dy = m->y - p->y;
		float d2 = dx * dx + dy * dy;
		float r2 = c->r * c->r;
		bool hit = d2 <= r2;
		if (hit) {
			ecs_add_id(it->world, it->entities[i], ecs_id(MiceCollide));
		} else {
			ecs_remove_id(it->world, it->entities[i], ecs_id(MiceCollide));
		}
	}
}

static void Change_Color_MiceCollide(ecs_iter_t *it)
{
	ColorsRgb *c = ecs_field(it, ColorsRgb, 0);           // self, in
	ColorsWorldRgb *w = ecs_field(it, ColorsWorldRgb, 1); // self, out
	for (int i = 0; i < it->count; ++i, ++c, ++w) {
		w->r = 0.5f - c->r;
		w->g = 0.5f - c->g;
		w->b = 0.5f - c->b;
	}
}

static void Change_Color_MiceToggle(ecs_iter_t *it)
{
	ColorsRgb *c = ecs_field(it, ColorsRgb, 0);           // self, in
	ColorsWorldRgb *w = ecs_field(it, ColorsWorldRgb, 1); // self, out
	for (int i = 0; i < it->count; ++i, ++c, ++w) {
		/*
		w->r = 1.0f;
		w->g = 0;
		w->b = 0;
		*/
	}
}

#define MOUSE_BUTTON_LEFT 0

static void Toggle(ecs_iter_t *it)
{
	MicePosition const *m = ecs_field(it, MicePosition, 0); // singleton, in
	for (int i = 0; i < it->count; ++i) {
		if (m->pressed & (1 << MOUSE_BUTTON_LEFT)) {
			if (ecs_has_id(it->world, it->entities[i], MiceToggle)) {
				ecs_remove_id(it->world, it->entities[i], MiceToggle);
			} else {
				ecs_add_id(it->world, it->entities[i], MiceToggle);
			}
		}
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
	ECS_COMPONENT_DEFINE(world, MicePositionLocal);
	ECS_TAG_DEFINE(world, MiceCollide);
	ECS_TAG_DEFINE(world, MiceToggle);

	ecs_struct(world,
	{.entity = ecs_id(MicePosition),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	{.name = "pressed", .type = ecs_id(ecs_u32_t)},
	{.name = "down", .type = ecs_id(ecs_u32_t)},
	{.name = "wheel", .type = ecs_id(ecs_f32_t)},
	{.name = "dx", .type = ecs_id(ecs_f32_t)},
	{.name = "dy", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(MicePositionLocal),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_singleton_add(world, MicePosition);

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Test_Collision_Rectangle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Test_Collision_Rectangle,
	.query.terms = {
	{.id = ecs_id(MicePositionLocal), .src.id = EcsUp, .trav = EcsChildOf, .inout = EcsIn},
	{.id = ecs_id(SpatialsTransform2), .inout = EcsIn},
	{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
	{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Test_Collision_Circle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Test_Collision_Circle,
	.query.terms = {
	{.id = ecs_id(MicePositionLocal), .src.id = EcsUp, .trav = EcsChildOf, .inout = EcsIn},
	{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
	{.id = ecs_id(ShapesCircle), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Change_Color_MiceCollide", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Change_Color_MiceCollide,
	.query.terms = {
	{.id = ecs_id(ColorsRgb), .inout = EcsIn},
	{.id = ecs_id(ColorsWorldRgb), .inout = EcsOut},
	{.id = MiceCollide},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Change_Color_MiceToggle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Change_Color_MiceToggle,
	.query.terms = {
	{.id = ecs_id(ColorsRgb), .inout = EcsIn},
	{.id = ecs_id(ColorsWorldRgb), .inout = EcsOut},
	{.id = MiceToggle},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Toggle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Toggle,
	.query.terms = {
	{.id = ecs_id(MicePosition), .src.id = ecs_id(MicePosition), .inout = EcsIn},
	{.id = MiceCollide},
	}});
}