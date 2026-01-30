#include <raylib.h>
#include <flecs.h>

#include "rlgl.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" // Required for GUI controls

#include "Spatials.h"
#include "Shapes.h"
#include "Draws.h"

// include header for getcwd
#include <unistd.h>
#include <limits.h> // For PATH_MAX


void DrawRectangleProRotates(Vector2 position, float w, float h, float rotation, Color color)
{
	rlPushMatrix();
	rlTranslatef(position.x, position.y, 0);
	rlRotatef(rotation, 0, 0, 1);
	DrawRectangleRec((Rectangle){-w / 2, -h / 2, w, h}, color);
	DrawLineEx((Vector2){0, 0}, (Vector2){(w / 2) + 10, 0}, 1.0f, RED);    // Draw X line
	DrawLineEx((Vector2){0, 0}, (Vector2){0, -(h / 2 + 10)}, 1.0f, GREEN); // Draw Y line
	rlPopMatrix();
}

bool CheckCollisionPointRotatedRect(Vector2 point, Vector2 position, float w, float h, float rotation)
{
	// First, rotate the point back inversely relative to the rectangle
	float sinRot = sinf(-rotation * DEG2RAD);
	float cosRot = cosf(-rotation * DEG2RAD);

	// Translate point into rectangle's local space
	Vector2 localPoint = {
	cosRot * (point.x - position.x) - sinRot * (point.y - position.y),
	sinRot * (point.x - position.x) + cosRot * (point.y - position.y)};

	// Check if the local point lies within the bounds of the rectangle
	return (localPoint.x >= -w / 2 && localPoint.x <= w / 2 &&
	        localPoint.y >= -h / 2 && localPoint.y <= h / 2);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
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
	ECS_IMPORT(world, Draws);
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	ecs_script_run_file(world, "./config/world.flecs");
	ecs_log_set_level(-1);



	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera mouse zoom");

	Camera2D camera = {0};
	camera.zoom = 1.0f;

	int zoomMode = 0; // 0-Mouse Wheel, 1-Mouse Move

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second
	                  //--------------------------------------------------------------------------------------
	float angle = 0.0f;
	bool pause = false;

	Rectangle rec01 = {70.0f, 35.0f, 20.0f, 20.0f};




	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{

		// Translate based on mouse right click
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f / camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
		}

		// Zoom based on mouse wheel
		float wheel = GetMouseWheelMove();
		if (wheel != 0) {
			// Get the world point that is under the mouse
			Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
			// Set the offset to where the mouse is
			camera.offset = GetMousePosition();
			// Set the target to match, so that the camera maps the world space point
			// under the cursor to the screen space point under the cursor at any zoom
			camera.target = mouseWorldPos;
			// Zoom increment
			// Uses log scaling to provide consistent zoom speed
			float scale = 0.2f * wheel;
			camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
		}

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode2D(camera);
		// Draw the 3d grid, rotated 90 degrees and centered around 0,0
		// just so we have something in the XY plane
		rlPushMatrix();
		rlTranslatef(0, 25 * 50, 0);
		rlRotatef(90, 1, 0, 0);
		DrawGrid(100, 50);

		rlPopMatrix();

        ecs_progress(world, 0);

		// Draw a reference circle
		//DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 50, MAROON);
		DrawRectangleProRotates((Vector2){rec01.x, rec01.y}, rec01.width, rec01.height, angle, BLUE);


		EndMode2D();

        Vector2 mousePosWorld = GetScreenToWorld2D(GetMousePosition(), camera);
        bool isMouseOver = CheckCollisionPointRotatedRect(mousePosWorld, (Vector2){rec01.x, rec01.y}, rec01.width, rec01.height, angle);
        DrawText("Click on the rectangle", 10, 10, 20, DARKGRAY);
        DrawText(isMouseOver ? "Mouse Over: YES" : "Mouse Over: NO", 10, 40, 20, DARKGRAY);
        
		// Draw mouse reference
		// Vector2 mousePos = GetWorldToScreen2D(GetMousePosition(), camera)
		DrawCircleV(GetMousePosition(), 4, DARKGRAY);
		DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()),
		Vector2Add(GetMousePosition(), (Vector2){-44, -24}), 20, 2, BLACK);

		GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(GRAY));
		GuiToggle((Rectangle){640, 70, 120, 20}, TextFormat("Pause"), &pause);
		GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(LIME));
		GuiSliderBar((Rectangle){640, 40, 120, 20}, "Angle", TextFormat("%.0f°", angle), &angle, 0.0f, 360.0f);

		// Angle values panel
		GuiGroupBox((Rectangle){620, 110, 140, 170}, "Angle Values");

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}