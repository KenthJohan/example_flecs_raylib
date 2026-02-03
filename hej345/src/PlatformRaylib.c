#include "PlatformRaylib.h"
#include "Spatials.h"
#include "Shapes.h"
#include "Colors.h"
#include "Mice.h"
#include "Renders.h"

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" // Required for GUI controls
#include "raymath.h"
#include "rlgl.h"

typedef struct {
	RenderTexture render;
	Camera2D camera;
	ecs_query_t *query_circles;
	ecs_query_t *query_rectangles;
	ecs_query_t *query_cranks;
	ecs_query_t *query_transforms;
} PlatformRaylibCanvas2;

typedef struct {
	int32_t dummy;
} PlatformRaylibWindow;

ECS_COMPONENT_DECLARE(PlatformRaylibCanvas2);
ECS_COMPONENT_DECLARE(PlatformRaylibWindow);
ECS_COMPONENT_DECLARE(PlatformRaylibState);

static void generate_6vertices_rectangle(SpatialsVector2 *v, float w, float h)
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

static void SpatialsTransform2_transform_points(SpatialsTransform2 *t, SpatialsVector2 *points, int count)
{
	for (int i = 0; i < count; i++) {
		float x = points[i].x;
		float y = points[i].y;
		points[i].x = t->c0[0] * x + t->c1[0] * y;
		points[i].y = t->c0[1] * x + t->c1[1] * y;
	}
}

static void SpatialsVector2_translate(SpatialsVector2 *v, int count, SpatialsVector2 t)
{
	for (int i = 0; i < count; i++) {
		v[i].x += t.x;
		v[i].y += t.y;
	}
}

static void SpatialsVector2_draw(SpatialsVector2 *v, int count, Color color)
{
	rlBegin(RL_TRIANGLES);
	for (int i = 0; i < count; i++, ++v) {
		rlColor4ub(color.r, color.g, color.b, color.a);
		rlVertex2f(v->x, v->y);
	}
	rlEnd();
}

static void Draw_Crank(ecs_iter_t *it)
{
	SpatialsWorldPosition2 *a = ecs_field(it, SpatialsWorldPosition2, 0); // shared, in
	SpatialsWorldPosition2 *b = ecs_field(it, SpatialsWorldPosition2, 1); // self, in
	ShapesCrank *c = ecs_field(it, ShapesCrank, 2);                       // self, in
	ColorsWorldRgb *color = ecs_field(it, ColorsWorldRgb, 3);             // self, in
	for (int i = 0; i < it->count; ++i, ++b, ++c, ++color) {
		DrawLineEx((Vector2){a->x, a->y}, (Vector2){b->x, b->y}, c->t, (Color){color->r, color->g, color->b, 255});
	}
}

static void Update(ecs_iter_t *it)
{
	MiceState *m = ecs_field(it, MiceState, 0); // singleton, in
	// Vector2 mousePosWorld = GetScreenToWorld2D(GetMousePosition(), c->camera);
	uint32_t pressed = 0;
	pressed |= IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ? 1 << MOUSE_BUTTON_LEFT : 0;
	pressed |= IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ? 1 << MOUSE_BUTTON_RIGHT : 0;
	pressed |= IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) ? 1 << MOUSE_BUTTON_MIDDLE : 0;
	pressed |= IsMouseButtonPressed(MOUSE_BUTTON_SIDE) ? 1 << MOUSE_BUTTON_SIDE : 0;
	pressed |= IsMouseButtonPressed(MOUSE_BUTTON_EXTRA) ? 1 << MOUSE_BUTTON_EXTRA : 0;
	pressed |= IsMouseButtonPressed(MOUSE_BUTTON_FORWARD) ? 1 << MOUSE_BUTTON_FORWARD : 0;
	pressed |= IsMouseButtonPressed(MOUSE_BUTTON_BACK) ? 1 << MOUSE_BUTTON_BACK : 0;
	uint32_t down = 0;
	down |= IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 1 << MOUSE_BUTTON_LEFT : 0;
	down |= IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ? 1 << MOUSE_BUTTON_RIGHT : 0;
	down |= IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) ? 1 << MOUSE_BUTTON_MIDDLE : 0;
	down |= IsMouseButtonDown(MOUSE_BUTTON_SIDE) ? 1 << MOUSE_BUTTON_SIDE : 0;
	down |= IsMouseButtonDown(MOUSE_BUTTON_EXTRA) ? 1 << MOUSE_BUTTON_EXTRA : 0;
	down |= IsMouseButtonDown(MOUSE_BUTTON_FORWARD) ? 1 << MOUSE_BUTTON_FORWARD : 0;
	down |= IsMouseButtonDown(MOUSE_BUTTON_BACK) ? 1 << MOUSE_BUTTON_BACK : 0;
	float wheel = GetMouseWheelMove();
	m->down = down;
	m->pressed = pressed;
	m->wheel = wheel;
	m->dx = GetMouseDelta().x;
	m->dy = GetMouseDelta().y;
	m->x = GetMousePosition().x;
	m->y = GetMousePosition().y;
}

static void RendersCanvas2_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	RendersCanvas2 *c = ecs_field(it, RendersCanvas2, 0);   // self, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 1); // self, in
	for (int i = 0; i < it->count; ++i, ++c, ++r) {
		PlatformRaylibCanvas2 *canvas = ecs_ensure(world, it->entities[i], PlatformRaylibCanvas2);
		canvas->render = LoadRenderTexture(r->w, r->h);
		canvas->camera = (Camera2D){0};
		canvas->query_circles = ecs_query_init(world,
		&(ecs_query_desc_t){
		.terms = {
		{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
		{.id = ecs_id(ShapesCircle), .inout = EcsIn},
		{.id = ecs_id(ColorsWorldRgb), .inout = EcsIn},
		}});
		canvas->query_rectangles = ecs_query_init(world,
		&(ecs_query_desc_t){
		.terms = {
		{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
		{.id = ecs_id(SpatialsTransform2), .inout = EcsIn},
		{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
		{.id = ecs_id(ColorsWorldRgb), .inout = EcsIn},
		}});
		canvas->query_transforms = ecs_query_init(world,
		&(ecs_query_desc_t){
		.terms = {
		{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
		{.id = ecs_id(SpatialsTransform2), .inout = EcsIn},
		}});
		canvas->query_cranks = ecs_query_init(world,
		&(ecs_query_desc_t){
		.terms = {
		{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn, .trav = EcsChildOf, .src.id = EcsUp},
		{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsIn},
		{.id = ecs_id(ShapesCrank), .inout = EcsIn},
		{.id = ecs_id(ColorsWorldRgb), .inout = EcsIn},
		}});
		canvas->camera.zoom = 1.0f;
	}
}

static void Draw_Circle(ecs_iter_t *it)
{
	SpatialsWorldPosition2 *p = ecs_field(it, SpatialsWorldPosition2, 0); // self, in
	ShapesCircle *c = ecs_field(it, ShapesCircle, 1);                     // self, in
	ColorsWorldRgb *color = ecs_field(it, ColorsWorldRgb, 2);             // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++c, ++color) {
		if (ecs_has(it->world, it->entities[i], MiceToggle)) {
			float padding = 2.0f;
			DrawCircleV((Vector2){p->x, p->y}, c->r, (Color){~color->r, ~color->g, ~color->b, 255});
			DrawCircleV((Vector2){p->x, p->y}, c->r - padding, (Color){color->r, color->g, color->b, 255});
		} else {
			DrawCircleV((Vector2){p->x, p->y}, c->r, (Color){color->r, color->g, color->b, 255});
		}
	}
}

static void Draw_Rectangle(ecs_iter_t *it)
{
	SpatialsWorldPosition2 *p = ecs_field(it, SpatialsWorldPosition2, 0); // self, in
	SpatialsTransform2 *t = ecs_field(it, SpatialsTransform2, 1);         // self, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 2);               // self, in
	ColorsWorldRgb *color = ecs_field(it, ColorsWorldRgb, 3);             // self, in
	for (int i = 0; i < it->count; ++i, ++p, ++r, ++t, ++color) {
		SpatialsVector2 vertices[6] = {0};
		float padding = 4.0f;
		if (ecs_has(it->world, it->entities[i], MiceToggle)) {

			generate_6vertices_rectangle(vertices, r->w, r->h);
			SpatialsTransform2_transform_points(t, vertices, 6);
			SpatialsVector2_translate(vertices, 6, (SpatialsVector2){p->x, p->y});
			SpatialsVector2_draw(vertices, 6, (Color){~color->r, ~color->g, ~color->b, 255});

			generate_6vertices_rectangle(vertices, r->w - padding, r->h - padding);
			SpatialsTransform2_transform_points(t, vertices, 6);
			SpatialsVector2_translate(vertices, 6, (SpatialsVector2){p->x, p->y});
			SpatialsVector2_draw(vertices, 6, (Color){color->r, color->g, color->b, 255});
		} else {
			generate_6vertices_rectangle(vertices, r->w, r->h);
			SpatialsTransform2_transform_points(t, vertices, 6);
			SpatialsVector2_translate(vertices, 6, (SpatialsVector2){p->x, p->y});
			SpatialsVector2_draw(vertices, 6, (Color){color->r, color->g, color->b, 255});
		}
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

static void DrawsRaylibCanvas_Update(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	PlatformRaylibCanvas2 *c = ecs_field(it, PlatformRaylibCanvas2, 0); // self, in
	MicePositionLocal *ml = ecs_field(it, MicePositionLocal, 1);        // self, in
	MiceState *m = ecs_field(it, MiceState, 2);                         // singleton, in
	for (int i = 0; i < it->count; ++i, ++c, ++ml) {
		// Update mouse world position
		Vector2 wmouse = GetScreenToWorld2D((Vector2){m->x, m->y}, c->camera);
		ml->x = wmouse.x;
		ml->y = wmouse.y;

		// Translate based on mouse right click
		if (m->down & (1 << MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = {m->dx, m->dy};
			delta = Vector2Scale(delta, -1.0f / c->camera.zoom);
			c->camera.target = Vector2Add(c->camera.target, delta);
		}

		if (m->wheel != 0) {
			// Get the world point that is under the mouse
			Vector2 mouseWorldPos = GetScreenToWorld2D((Vector2){m->x, m->y}, c->camera);
			// Set the offset to where the mouse is
			c->camera.offset = (Vector2){m->x, m->y};
			// Set the target to match, so that the camera maps the world space point
			// under the cursor to the screen space point under the cursor at any zoom
			c->camera.target = mouseWorldPos;
			// Zoom increment
			// Uses log scaling to provide consistent zoom speed
			float scale = 0.2f * m->wheel;
			c->camera.zoom = Clamp(expf(logf(c->camera.zoom) + scale), 0.125f, 64.0f);
		}

		BeginTextureMode(c->render);
		ClearBackground(RAYWHITE);
		BeginMode2D(c->camera);

		// Draw the 3d grid, rotated 90 degrees and centered around 0,0
		// just so we have something in the XY plane
		rlPushMatrix();
		rlTranslatef(0, 25 * 50, 0);
		rlRotatef(90, 1, 0, 0);
		DrawGrid(100, 50);
		rlPopMatrix();

		ecs_iter_t it2;
		it2 = ecs_query_iter(world, c->query_circles);
		while (ecs_query_next(&it2)) {
			Draw_Circle(&it2);
		}
		it2 = ecs_query_iter(world, c->query_rectangles);
		while (ecs_query_next(&it2)) {
			Draw_Rectangle(&it2);
		}
		it2 = ecs_query_iter(world, c->query_transforms);
		while (ecs_query_next(&it2)) {
			Draw_Transform(&it2);
		}
		it2 = ecs_query_iter(world, c->query_cranks);
		while (ecs_query_next(&it2)) {
			Draw_Crank(&it2);
		}
		EndMode2D();
		EndTextureMode();
	}
}

static void DrawsRaylibCanvas_Draw(ecs_iter_t *it)
{
	BeginDrawing();
	ClearBackground(BLACK);
	while (ecs_query_next(it)) {
		PlatformRaylibCanvas2 *c = ecs_field(it, PlatformRaylibCanvas2, 0); // self, in
		for (int i = 0; i < it->count; ++i, ++c) {
			Rectangle splitScreenRect = {0.0f, 0.0f, (float)c->render.texture.width, (float)-c->render.texture.height};
			DrawTextureRec(c->render.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
		}
	}
	DrawRectangle(GetScreenWidth() / 2 - 2, 0, 4, GetScreenHeight(), LIGHTGRAY);
	EndDrawing();
}

static void PlatformRaylibWindow_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	RendersWindow *w = ecs_field(it, RendersWindow, 0);     // self, in
	ShapesRectangle *r = ecs_field(it, ShapesRectangle, 1); // self, in
	ecs_entity_t e = it->entities[0];

	w->close_requested = 0;
	PlatformRaylibWindow *wr = ecs_ensure(world, e, PlatformRaylibWindow);
	wr->dummy = 0;
	InitWindow((int)r->w, (int)r->h, "raylib + flecs");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(60);

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "Update", .parent = e, .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = Update,
	.query.terms = {
	{.id = ecs_id(MiceState), .src.id = ecs_id(MiceState), .inout = EcsIn},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "RendersCanvas2_Create", .parent = e, .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = RendersCanvas2_Create,
	.query.terms = {
	{.id = ecs_id(RendersCanvas2), .inout = EcsIn},
	{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
	{.id = ecs_id(PlatformRaylibCanvas2), .oper = EcsNot},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "DrawsRaylibCanvas_Update", .parent = e, .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = DrawsRaylibCanvas_Update,
	.query.terms = {
	{.id = ecs_id(PlatformRaylibCanvas2), .inout = EcsIn},
	{.id = ecs_id(MicePositionLocal), .inout = EcsIn},
	{.id = ecs_id(MiceState), .src.id = ecs_id(MiceState), .inout = EcsIn},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "DrawsRaylibCanvas_Draw", .parent = e, .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.run = DrawsRaylibCanvas_Draw,
	.query.terms = {
	{.id = ecs_id(PlatformRaylibCanvas2), .inout = EcsIn},
	}});
}

static void PlatformRaylibWindow_Update(ecs_iter_t *it)
{
	RendersWindow *w = ecs_field(it, RendersWindow, 0); // self, in
	w->close_requested = WindowShouldClose();           // Does update and check for close
	if (IsKeyDown(KEY_ONE)) {
		MaximizeWindow();
	}
	if (w->close_requested) {
		ecs_delete(it->world, it->entities[0]);
		CloseWindow();
	}
}

void PlatformRaylibImport(ecs_world_t *world)
{
	ECS_MODULE(world, PlatformRaylib);
	ECS_IMPORT(world, Spatials);
	ECS_IMPORT(world, Shapes);
	ECS_IMPORT(world, Colors);
	ECS_IMPORT(world, Mice);
	ECS_IMPORT(world, Renders);
	ecs_set_name_prefix(world, "PlatformRaylib");

	ECS_COMPONENT_DEFINE(world, PlatformRaylibState);
	ECS_COMPONENT_DEFINE(world, PlatformRaylibCanvas2);
	ECS_COMPONENT_DEFINE(world, PlatformRaylibWindow);

	ecs_singleton_add(world, PlatformRaylibState);
	ecs_add(world, ecs_id(MiceState), MiceState);

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "PlatformRaylibWindow_Update", .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = PlatformRaylibWindow_Update,
	.query.terms = {
	{.id = ecs_id(RendersWindow), .inout = EcsIn},
	{.id = ecs_id(PlatformRaylibWindow), .inout = EcsIn},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "PlatformRaylibWindow_Create", .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = PlatformRaylibWindow_Create,
	.immediate = true,
	.query.terms = {
	{.id = ecs_id(RendersWindow), .inout = EcsIn},
	{.id = ecs_id(ShapesRectangle), .inout = EcsIn},
	{.id = ecs_id(PlatformRaylibWindow), .oper = EcsNot},
	}});
}