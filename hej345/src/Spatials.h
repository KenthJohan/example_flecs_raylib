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
	float radians;
} SpatialsRotationLocal;

typedef struct {
	float radians;
} SpatialsRotationWorld;

typedef struct {
	float c0[2];
	float c1[2];
} SpatialsTransform2;

typedef struct {
	float x;
	float y;
} SpatialsVector2;

typedef struct {
	float radians_per_second;
} SpatialsOmega;

extern ECS_COMPONENT_DECLARE(SpatialsPosition2Local);
extern ECS_COMPONENT_DECLARE(SpatialsPosition2World);
extern ECS_COMPONENT_DECLARE(SpatialsRotationLocal);
extern ECS_COMPONENT_DECLARE(SpatialsRotationWorld);
extern ECS_COMPONENT_DECLARE(SpatialsTransform2);
extern ECS_COMPONENT_DECLARE(SpatialsVector2);
extern ECS_COMPONENT_DECLARE(SpatialsOmega);

void SpatialsImport(ecs_world_t *world);