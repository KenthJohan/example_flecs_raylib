#pragma once

#include <flecs.h>

typedef struct {
	float x;
	float y;
} SpatialsPosition2;

typedef struct {
	float x;
	float y;
} SpatialsWorldPosition2;

typedef struct {
	float radians;
} SpatialsRotation;

typedef struct {
	float radians;
} SpatialsWorldRotation;

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

extern ECS_COMPONENT_DECLARE(SpatialsPosition2);
extern ECS_COMPONENT_DECLARE(SpatialsRotation);
extern ECS_COMPONENT_DECLARE(SpatialsVector2);
extern ECS_COMPONENT_DECLARE(SpatialsOmega);

extern ECS_COMPONENT_DECLARE(SpatialsWorldPosition2);
extern ECS_COMPONENT_DECLARE(SpatialsWorldRotation);
extern ECS_COMPONENT_DECLARE(SpatialsTransform2);

void SpatialsImport(ecs_world_t *world);