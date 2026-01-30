#pragma once

#include <flecs.h>

typedef struct {
	float x;
	float y;
	uint32_t pressed;
} MicePosition;

extern ECS_TAG_DECLARE(MiceCollide);
extern ECS_TAG_DECLARE(MiceToggle);
extern ECS_COMPONENT_DECLARE(MicePosition);


void MiceImport(ecs_world_t *world);