#pragma once

#include <flecs.h>

typedef struct {
	float x;
	float y;
} SpatialsPosition2Local;

typedef struct {
	float x;
	float y;
} SpatialsPosition2World;

typedef struct {
	float w;
	float h;
} SpatialsRectangle;

typedef struct {
	float r;
} SpatialsCircle;

extern ECS_COMPONENT_DECLARE(SpatialsPosition2Local);
extern ECS_COMPONENT_DECLARE(SpatialsPosition2World);
extern ECS_COMPONENT_DECLARE(SpatialsRectangle);
extern ECS_COMPONENT_DECLARE(SpatialsCircle);


void SpatialsImport(ecs_world_t *world);