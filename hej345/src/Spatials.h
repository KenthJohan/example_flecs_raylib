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
	float angle;
} SpatialsRotationLocal;

typedef struct {
	float angle;
} SpatialsRotationWorld;

extern ECS_COMPONENT_DECLARE(SpatialsPosition2Local);
extern ECS_COMPONENT_DECLARE(SpatialsPosition2World);
extern ECS_COMPONENT_DECLARE(SpatialsRotationLocal);
extern ECS_COMPONENT_DECLARE(SpatialsRotationWorld);

void SpatialsImport(ecs_world_t *world);