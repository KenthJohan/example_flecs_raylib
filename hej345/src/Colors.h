#pragma once

#include <flecs.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ColorsRgb;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ColorsWorldRgb;

extern ECS_COMPONENT_DECLARE(ColorsRgb);
extern ECS_COMPONENT_DECLARE(ColorsWorldRgb);

void ColorsImport(ecs_world_t *world);