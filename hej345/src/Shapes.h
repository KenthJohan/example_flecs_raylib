#pragma once

#include <flecs.h>

typedef struct {
	float w;
	float h;
} ShapesRectangle;

typedef struct {
	float r;
} ShapesCircle;

typedef struct {
	float t;
} ShapesCrank;

extern ECS_COMPONENT_DECLARE(ShapesRectangle);
extern ECS_COMPONENT_DECLARE(ShapesCircle);
extern ECS_COMPONENT_DECLARE(ShapesCrank);

void ShapesImport(ecs_world_t *world);