#include "enemy_behavior.h"

#include "../kinematics.h"

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

static const Node *const chase_sequence_children[] = { &is_player_close_node };
static const Node chase_sequence = { .tick = sequence_tick,
                                     .children = chase_sequence_children,
                                     .child_count = 1 };

static const Node *const root_children[] = { &chase_sequence };
const Node basic_enemy_behavior_tree = { .tick = selector_tick,
                                         .children = root_children,
                                         .child_count = 1 };