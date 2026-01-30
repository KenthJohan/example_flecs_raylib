#pragma once

#include <flecs.h>

typedef struct {
	float w;
	float h;
} ShapesRectangle;

typedef struct {
	float r;
} ShapesCircle;

extern ECS_COMPONENT_DECLARE(ShapesRectangle);
extern ECS_COMPONENT_DECLARE(ShapesCircle);

void ShapesImport(ecs_world_t *world);