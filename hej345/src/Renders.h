#pragma once

#include <flecs.h>

typedef struct {
	uint8_t close_requested;
} RendersWindow;

typedef struct {
	int32_t dummy;
} RendersCanvas2;

extern ECS_COMPONENT_DECLARE(RendersWindow);
extern ECS_COMPONENT_DECLARE(RendersCanvas2);

extern ECS_TAG_DECLARE(RendersGroup);

void RendersImport(ecs_world_t *world);