#pragma once

#include <flecs.h>

typedef struct {
	float x;
	float y;
} MicePosition;

extern ECS_TAG_DECLARE(MiceCollide);
extern ECS_COMPONENT_DECLARE(MicePosition);


void MiceImport(ecs_world_t *world);