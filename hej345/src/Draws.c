#include "Draws.h"
#include "Spatials.h"
#include "Shapes.h"
#include "Colors.h"
#include "raylib.h"
#include "rlgl.h"
#include <math.h>

static void Draw_Circle(ecs_iter_t *it)
{
	SpatialsWorldPosition2 *p = ecs_field(it, SpatialsWorldPosition2, 0); // self, in
	ShapesCircle *c = ecs_field(it, ShapesCircle, 1);                     // self, in
	ColorsRgb *color = ecs_field(it, ColorsRgb, 2);                       // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++c, ++color) {
		DrawCircleV((Vector2){p->x, p->y}, c->r, (Color){color->r, color->g, color->b, 255});
	}
}

static void Draw_Rectangle(ecs_iter_t *it)
{
	SpatialsWorldPosition2 *p = ecs_field(it, SpatialsWorldPosition2, 0); // self, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 1);               // self, in
	ColorsRgb *color = ecs_field(it, ColorsRgb, 2);                       // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++r, ++color) {
		// DrawRectangleV((Vector2){p->x - r->w / 2, p->y - r->h / 2}, (Vector2){r->w, r->h}, BLUE);
		// DrawLineEx((Vector2){p->x, p->y}, (Vector2){p->x + r->w, p->y}, 1.0f, RED);   // Draw X line
		// DrawLineEx((Vector2){p->x, p->y}, (Vector2){p->x, p->y - r->h}, 1.0f, GREEN); // Draw Y line
	}
}

void generate_6vertices_rectangle(SpatialsVector2 *v, float w, float h)
{
	// First triangle
	v[2].x = -w / 2;
	v[2].y = -h / 2;
	v[1].x = w / 2;
	v[1].y = -h / 2;
	v[0].x = w / 2;
	v[0].y = h / 2;
	// Second triangle
	v[5].x = -w / 2;
	v[5].y = -h / 2;
	v[4].x = w / 2;
	v[4].y = h / 2;
	v[3].x = -w / 2;
	v[3].y = h / 2;
}

void SpatialsTransform2_transform_points(SpatialsTransform2 *t, SpatialsVector2 *points, int count)
{
	for (int i = 0; i < count; i++) {
		float x = points[i].x;
		float y = points[i].y;
		points[i].x = t->c0[0] * x + t->c1[0] * y;
		points[i].y = t->c0[1] * x + t->c1[1] * y;
	}
}

void SpatialsVector2_translate(SpatialsVector2 *v, int count, SpatialsVector2 t)
{
	for (int i = 0; i < count; i++) {
		v[i].x += t.x;
		v[i].y += t.y;
	}
}

void SpatialsVector2_draw(SpatialsVector2 *v, int count, Color color)
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
	SpatialsWorldPosition2 *p = ecs_field(it, SpatialsWorldPosition2, 0); // self, in
	SpatialsTransform2 *t = ecs_field(it, SpatialsTransform2, 1);         // self, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 2);               // self, in
	ColorsRgb *color = ecs_field(it, ColorsRgb, 3);                       // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++r, ++t, ++color) {
		SpatialsVector2 vertices[6] = {0};
		generate_6vertices_rectangle(vertices, r->w, r->h);
		SpatialsTransform2_transform_points(t, vertices, 6);
		SpatialsVector2_translate(vertices, 6, (SpatialsVector2){p->x, p->y});
		SpatialsVector2_draw(vertices, 6, (Color){color->r, color->g, color->b, 255});
	}
}

static void Draw_Transform(ecs_iter_t *it)
{
	SpatialsWorldPosition2 *p = ecs_field(it, SpatialsWorldPosition2, 0); // self, in
	SpatialsTransform2 *t = ecs_field(it, SpatialsTransform2, 1);         // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++t) {
		DrawLineEx((Vector2){p->x, p->y}, (Vector2){p->x + t->c0[0] * 20, p->y + t->c0[1] * 20}, 2.0f, RED);   // Draw X line
		DrawLineEx((Vector2){p->x, p->y}, (Vector2){p->x + t->c1[0] * 20, p->y + t->c1[1] * 20}, 2.0f, GREEN); // Draw Y line
	}
}

void DrawsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Draws);
	ecs_set_name_prefix(world, "Draws");
	ECS_IMPORT(world, Spatials);
	ECS_IMPORT(world, Shapes);
	ECS_IMPORT(world, Colors);

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Draw_Circle", .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = Draw_Circle,
	.query.terms = {
	{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
	{.id = ecs_id(ShapesCircle), .inout = EcsIn},
	{.id = ecs_id(ColorsRgb), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Draw_Rectangle", .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = Draw_Rectangle,
	.query.terms = {
	{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
	{.id = ecs_id(SpatialsTransform2), .inout = EcsIn, .oper = EcsNot},
	{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
	{.id = ecs_id(ColorsRgb), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Draw_Rectangle_Rotated", .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = Draw_Rectangle_Rotated,
	.query.terms = {
	{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
	{.id = ecs_id(SpatialsTransform2), .inout = EcsIn},
	{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
	{.id = ecs_id(ColorsRgb), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Draw_Transform", .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = Draw_Transform,
	.query.terms = {
	{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
	{.id = ecs_id(SpatialsTransform2), .inout = EcsIn},
	}});
}