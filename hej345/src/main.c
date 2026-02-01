#include <raylib.h>
#include <flecs.h>

#include "rlgl.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" // Required for GUI controls

#include "Spatials.h"
#include "Shapes.h"
#include "Renders.h"
#include "Colors.h"
#include "Mice.h"
#include "PlatformRaylib.h"

// include header for getcwd
#include <unistd.h>
#include <limits.h> // For PATH_MAX

static ecs_os_api_t os_api_default = {0};

void main_abort()
{
	exit(1);
}

void main_log(int32_t level, const char *file, int32_t line, const char *msg)
{
	os_api_default.log_(level, file, line, msg);
	switch (level) {
	case -3:
		printf("Break here\n");
		break;
	case -4:
		printf("Break here\n");
		break;
	}
}

int main(void)
{
	ecs_os_set_api_defaults();
	os_api_default = ecs_os_get_api();
	ecs_os_api_t os_api = os_api_default;
	os_api.log_ = main_log;
	os_api.abort_ = main_abort;
	ecs_os_set_api(&os_api);

	// print current directory
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir: %s\n", cwd);
	} else {
		perror("getcwd() error");
	}

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, Spatials);
	ECS_IMPORT(world, Shapes);
	ECS_IMPORT(world, Mice);
	ECS_IMPORT(world, Renders);
	ECS_IMPORT(world, Colors);
	ECS_IMPORT(world, PlatformRaylib);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	const int screenWidth = 800;
	const int screenHeight = 450;
	InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera mouse zoom");
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "./config/world.flecs");
	ecs_log_set_level(-1);

	ecs_entity_t canvas1 = ecs_lookup(world, "canvas1");
	if (canvas1 == 0) {
		ecs_os_abort();
	}

	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		if (IsKeyDown(KEY_ONE)) {
			MaximizeWindow();
		}
		ecs_progress(world, 0);
	}
	CloseWindow();
	return 0;
}