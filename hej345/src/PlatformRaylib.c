#include "PlatformRaylib.h"
#include "Spatials.h"
#include "Shapes.h"
#include "Colors.h"
#include "Mice.h"

#include "raygui.h" // Required for GUI controls

ECS_COMPONENT_DECLARE(PlatformRaylibState);


static void Update(ecs_iter_t *it)
{
	MicePosition *m = ecs_field(it, MicePosition, 0); // singleton, in
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

void PlatformRaylibImport(ecs_world_t *world)
{
	ECS_MODULE(world, PlatformRaylib);
	ECS_IMPORT(world, Spatials);
	ECS_IMPORT(world, Shapes);
	ECS_IMPORT(world, Colors);
	ECS_IMPORT(world, Mice);
	ecs_set_name_prefix(world, "PlatformRaylib");

	ECS_COMPONENT_DEFINE(world, PlatformRaylibState);

	ecs_singleton_add(world, PlatformRaylibState);
	ecs_add(world, ecs_id(PlatformRaylibState), MicePosition);


	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "DrawsRaylibCanvas_Draw", .add = ecs_ids(ecs_dependson(EcsPostUpdate))}),
	.callback = Update,
	.query.terms = {
	{.id = ecs_id(MicePosition), .src.id = ecs_id(PlatformRaylibState), .inout = EcsIn},
	}});
}