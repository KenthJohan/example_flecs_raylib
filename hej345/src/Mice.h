#pragma once

#include <flecs.h>

typedef struct {
	float x;
	float y;
	uint32_t pressed;
	uint32_t down;
	float wheel;
	float dx;
	float dy;
} MicePosition;

typedef struct {
	float x;
	float y;
} MicePositionLocal;

extern ECS_TAG_DECLARE(MiceCollide);
extern ECS_TAG_DECLARE(MiceToggle);
extern ECS_COMPONENT_DECLARE(MicePosition);
extern ECS_COMPONENT_DECLARE(MicePositionLocal);


void MiceImport(ecs_world_t *world);