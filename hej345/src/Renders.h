#pragma once

#include <flecs.h>

typedef struct {
	int32_t dummy;
} RendersCanvas2;

extern ECS_COMPONENT_DECLARE(RendersCanvas2);

extern ECS_TAG_DECLARE(RendersGroup);

void RendersImport(ecs_world_t *world);