#pragma once

#include <flecs.h>

typedef struct {
    int32_t dummy;
} PlatformRaylibState;

extern ECS_COMPONENT_DECLARE(PlatformRaylibState);

void PlatformRaylibImport(ecs_world_t *world);