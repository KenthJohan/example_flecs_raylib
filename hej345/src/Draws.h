#pragma once

#include <flecs.h>


typedef struct {
    int32_t dummy;
} DrawsCanvasCreate;

extern ECS_COMPONENT_DECLARE(DrawsCanvasCreate);


extern ECS_TAG_DECLARE(DrawsGroup);

void DrawsImport(ecs_world_t *world);