#include "Spatials.h"
#include <math.h>

ECS_COMPONENT_DECLARE(SpatialsPosition2);

ECS_COMPONENT_DECLARE(SpatialsRotation);

ECS_COMPONENT_DECLARE(SpatialsTransform2);
ECS_COMPONENT_DECLARE(SpatialsVector2);
ECS_COMPONENT_DECLARE(SpatialsOmega);
ECS_COMPONENT_DECLARE(SpatialsCrank);
ECS_COMPONENT_DECLARE(SpatialsFourBarLinkage);

ECS_COMPONENT_DECLARE(SpatialsWorldRotation);
ECS_COMPONENT_DECLARE(SpatialsWorldPosition2);

static void Position2_Cascade(ecs_iter_t *it)
{
	SpatialsWorldPosition2 *g = ecs_field(it, SpatialsWorldPosition2, 0);       // self, out
	SpatialsPosition2 const *l = ecs_field(it, SpatialsPosition2, 1);           // self, in
	SpatialsWorldPosition2 const *p = ecs_field(it, SpatialsWorldPosition2, 2); // parent, in
	SpatialsTransform2 const *t = ecs_field(it, SpatialsTransform2, 3);         // parent, in
	for (int i = 0; i < it->count; ++i, ++l, ++g) {
		if (t) {
			g->x = t->c0[0] * l->x + t->c1[0] * l->y;
			g->y = t->c0[1] * l->x + t->c1[1] * l->y;
		} else {
			g->x = l->x;
			g->y = l->y;
		}
		if (p) {
			g->x += p->x;
			g->y += p->y;
		}
	}
}

static void Rotation_Cascade(ecs_iter_t *it)
{
	SpatialsWorldRotation *g = ecs_field(it, SpatialsWorldRotation, 0);       // self, out
	SpatialsRotation const *l = ecs_field(it, SpatialsRotation, 1);           // self, in
	SpatialsWorldRotation const *p = ecs_field(it, SpatialsWorldRotation, 2); // parent, in
	for (int i = 0; i < it->count; ++i, ++l, ++g) {
		g->radians = l->radians;
		if (p) {
			g->radians += p->radians;
		}
	}
}

void SpatialsTransform2_make_rotation(SpatialsTransform2 *r, float radians)
{
	float s = sinf(radians);
	float c = cosf(radians);
	r->c0[0] = c;
	r->c0[1] = s;
	r->c1[0] = -s;
	r->c1[1] = c;
}

static void Transform_Make(ecs_iter_t *it)
{
	SpatialsTransform2 *t = ecs_field(it, SpatialsTransform2, 0);   // self, out
	SpatialsRotation const *r = ecs_field(it, SpatialsRotation, 1); // self, in
	for (int i = 0; i < it->count; ++i, ++r, ++t) {
		SpatialsTransform2_make_rotation(t, r->radians);
	}
}

static void Rotator(ecs_iter_t *it)
{
	SpatialsRotation *r = ecs_field(it, SpatialsRotation, 0); // self, out
	SpatialsOmega const *o = ecs_field(it, SpatialsOmega, 1); // self, in
	for (int i = 0; i < it->count; ++i, ++r, ++o) {
		r->radians += o->radians_per_second;
	}
}

double calculateFollowerAngle(double L1, double L2, double L3, double L4, double theta2)
{
	// L1 = Ground length (fixed, along x-axis)
	// L2 = Crank length
	// L3 = Coupler length
	// L4 = Follower length
	// theta2 = Crank angle (input, in radians)

	double K1, K2, K3;
	double A, B, C, discriminant, theta4;

	// Calculate the K constants (only K1, K2, K3 needed for theta4)
	K1 = L1 / L2;
	K2 = L1 / L4;
	K3 = (L2 * L2 - L3 * L3 + L4 * L4 + L1 * L1) / (2 * L2 * L4);

	// Calculate coefficients A, B, C for the quadratic equation in tan(theta4/2)
	A = cos(theta2) - K1 - K2 * cos(theta2) + K3;
	B = -2 * sin(theta2);
	C = K1 - (K2 + 1) * cos(theta2) + K3;

	// Solve quadratic equation for theta4
	discriminant = B * B - 4 * A * C;

	if (discriminant < 0) {
		// printf("Error: No valid solution exists (discriminant < 0). Check linkage dimensions.\n");
		return 0.0;
	}

	// Two possible solutions for theta4
	double theta4_1 = 2 * atan2((-B + sqrt(discriminant)), (2 * A));
	double theta4_2 = 2 * atan2((-B - sqrt(discriminant)), (2 * A));

	// Return the first solution (open configuration)
	// Use theta4_2 for crossed configuration if needed
	theta4 = theta4_1;

	return theta4;
}

static void SpatialsFourBarLinkage_Calculate(ecs_iter_t *it)
{
	SpatialsFourBarLinkage *l = ecs_field(it, SpatialsFourBarLinkage, 0); // self, inout
	for (int i = 0; i < it->count; ++i, ++l) {
		ecs_entity_t ea = ecs_get_parent(it->world, l->entity_driver);
		ecs_entity_t eb = ecs_get_parent(it->world, l->entity_follower);
		SpatialsWorldPosition2 const *pa = ecs_get(it->world, ea, SpatialsWorldPosition2);
		SpatialsWorldPosition2 const *pb = ecs_get(it->world, eb, SpatialsWorldPosition2);
		// Distance between the two fixed points (ground length)
		l->length_frame = sqrtf((pb->x - pa->x) * (pb->x - pa->x) + (pb->y - pa->y) * (pb->y - pa->y));
		float driver_angle = ecs_get(it->world, l->entity_driver, SpatialsRotation)->radians;
		float driver_length = ecs_get(it->world, l->entity_driver, SpatialsCrank)->l;
		float follower_length = ecs_get(it->world, l->entity_follower, SpatialsCrank)->l;
		float theta4 = calculateFollowerAngle(l->length_frame, driver_length, l->length_coupler, follower_length, driver_angle);
		ecs_set(it->world, l->entity_follower, SpatialsRotation, {.radians = theta4});
	}
}

static void Crank_To_Position(ecs_iter_t *it)
{
	SpatialsCrank const *c = ecs_field(it, SpatialsCrank, 0);       // self, in
	SpatialsRotation const *r = ecs_field(it, SpatialsRotation, 1); // self, in
	SpatialsPosition2 *p = ecs_field(it, SpatialsPosition2, 2);     // self, out
	for (int i = 0; i < it->count; ++i, ++c, ++r, ++p) {
		p->x = c->l * cosf(r->radians);
		p->y = c->l * sinf(r->radians);
	}
}

ECS_CTOR(SpatialsTransform2, t, {
	t->c0[0] = 1;
	t->c0[1] = 0;
	t->c1[0] = 0;
	t->c1[1] = 1;
})

void SpatialsImport(ecs_world_t *world)
{
	ECS_MODULE(world, Spatials);
	ecs_set_name_prefix(world, "Spatials");

	ECS_COMPONENT_DEFINE(world, SpatialsPosition2);
	ECS_COMPONENT_DEFINE(world, SpatialsRotation);
	ECS_COMPONENT_DEFINE(world, SpatialsTransform2);
	ECS_COMPONENT_DEFINE(world, SpatialsOmega);
	ECS_COMPONENT_DEFINE(world, SpatialsCrank);
	ECS_COMPONENT_DEFINE(world, SpatialsFourBarLinkage);

	ECS_COMPONENT_DEFINE(world, SpatialsWorldPosition2);
	ECS_COMPONENT_DEFINE(world, SpatialsWorldRotation);

	ecs_set_hooks(world, SpatialsTransform2, {.ctor = ecs_ctor(SpatialsTransform2)});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsPosition2),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsWorldPosition2),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsRotation),
	.members = {
	{.name = "radians", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsWorldRotation),
	.members = {
	{.name = "radians", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsTransform2),
	.members = {
	{.name = "c0", .type = ecs_id(ecs_f32_t), .count = 2},
	{.name = "c1", .type = ecs_id(ecs_f32_t), .count = 2},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsVector2),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsOmega),
	.members = {
	{.name = "radians_per_second", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsCrank),
	.members = {
	{.name = "l", .type = ecs_id(ecs_f32_t)},
	{.name = "t", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(SpatialsFourBarLinkage),
	.members = {
	{.name = "entity_driver", .type = ecs_id(ecs_entity_t)},
	{.name = "entity_follower", .type = ecs_id(ecs_entity_t)},
	{.name = "length_coupler", .type = ecs_id(ecs_f32_t)},
	{.name = "length_frame", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Rotator", .add = ecs_ids(ecs_dependson(EcsPreUpdate))}),
	.callback = Rotator,
	.query.terms = {
	{.id = ecs_id(SpatialsRotation), .inout = EcsOut},
	{.id = ecs_id(SpatialsOmega), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Transform_Make", .add = ecs_ids(ecs_dependson(EcsPreUpdate))}),
	.callback = Transform_Make,
	.query.terms = {
	{.id = ecs_id(SpatialsTransform2), .inout = EcsOut},
	{.id = ecs_id(SpatialsWorldRotation), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Position2_Cascade", .add = ecs_ids(ecs_dependson(EcsPreUpdate))}),
	.callback = Position2_Cascade,
	.query.terms = {
	{.id = ecs_id(SpatialsWorldPosition2), .inout = EcsOut},
	{.id = ecs_id(SpatialsPosition2), .inout = EcsIn},
	{.id = ecs_id(SpatialsWorldPosition2), .src.id = EcsCascade, .inout = EcsIn, .oper = EcsOptional},
	{.id = ecs_id(SpatialsTransform2), .src.id = EcsUp, .inout = EcsIn, .oper = EcsOptional},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Rotation_Cascade", .add = ecs_ids(ecs_dependson(EcsPreUpdate))}),
	.callback = Rotation_Cascade,
	.query.terms = {
	{.id = ecs_id(SpatialsWorldRotation), .inout = EcsOut},
	{.id = ecs_id(SpatialsRotation), .inout = EcsIn},
	{.id = ecs_id(SpatialsWorldRotation), .src.id = EcsCascade, .inout = EcsIn, .oper = EcsOptional},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Crank_To_Position", .add = ecs_ids(ecs_dependson(EcsPreUpdate))}),
	.callback = Crank_To_Position,
	.query.terms = {
	{.id = ecs_id(SpatialsCrank), .inout = EcsIn},
	{.id = ecs_id(SpatialsRotation), .inout = EcsIn},
	{.id = ecs_id(SpatialsPosition2), .inout = EcsOut},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "SpatialsFourBarLinkage_Calculate", .add = ecs_ids(ecs_dependson(EcsPreUpdate))}),
	.callback = SpatialsFourBarLinkage_Calculate,
	.query.terms = {
	{.id = ecs_id(SpatialsFourBarLinkage), .inout = EcsInOut},
	}});
}