#include "enemy_behavior.h"

#include "../context.h"
#include "../enemy.h"
#include "../kinematics.h"

#define ENEMY_SPEED 6

static const uint16_t is_close_distance = 600;

static NodeStatus is_player_close_tick(const Node *self, NodeContext *context)
{
    (void)self; // make compiler happy and not warn self is unused

    return absolute_distance(context->enemy->position, context->game_context->player->position) <
                   is_close_distance
               ? NODE_SUCCESS
               : NODE_FAILURE;
}
static const Node is_player_close_node = { .tick = is_player_close_tick,
                                           .children = NULL,
                                           .child_count = 0 };

static NodeStatus stop_tick(const Node *self, NodeContext *context)
{
    (void)self; // make compiler happy and not warn self is unused

    context->enemy->velocity = zero_velocity();
    return NODE_RUNNING;
}
static const Node stop_node = { .tick = stop_tick, .children = NULL, .child_count = 0 };

static NodeStatus chase_tick(const Node *self, NodeContext *context)
{
    (void)self; // make compiler happy and not warn self is unused

    PositionDelta delta =
        position_delta(context->game_context->player->position, context->enemy->position);
    Velocity new_velocity = velocity_toward_delta(delta, ENEMY_SPEED);
    context->enemy->velocity = new_velocity;
    return NODE_RUNNING;
}
static const Node chase_node = { .tick = chase_tick, .children = NULL, .child_count = 0 };

static const Node *const is_player_close_fallback_children[] = { &is_player_close_node,
                                                                 &stop_node };
static const Node is_player_close_fallback_node = { .tick = selector_tick,
                                                    .children = is_player_close_fallback_children,
                                                    .child_count = 2 };

static const Node *const chase_sequence_children[] = { &is_player_close_fallback_node,
                                                       &chase_node };
static const Node chase_sequence_node = { .tick = sequence_tick,
                                          .children = chase_sequence_children,
                                          .child_count = 2 };

static const Node *const root_children[] = { &chase_sequence_node };
const Node basic_enemy_behavior_tree = { .tick = selector_tick,
                                         .children = root_children,
                                         .child_count = 1 };