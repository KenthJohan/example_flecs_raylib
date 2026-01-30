#pragma once

#include <flecs.h>

typedef struct {
    float r;
    float g;
    float b;
} ColorsRgb;

extern ECS_COMPONENT_DECLARE(ColorsRgb);

void ColorsImport(ecs_world_t *world);