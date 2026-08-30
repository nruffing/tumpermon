#ifndef NODE_H
#define NODE_H

#include <stdint.h>

#include "../context.h"
#include "../enemy.h"

typedef enum NodeStatus {
    NODE_RUNNING,
    NODE_SUCCESS,
    NODE_FAILURE,
} NodeStatus;

typedef struct NodeContext {
    Context *game_context;
    Enemy *enemy;
} NodeContext;

typedef struct Node Node;

typedef NodeStatus (*NodeTickFn)(const Node *self, NodeContext *context);

struct Node {
    NodeTickFn tick;
    const Node *const *children; // NULL for leaves
    uint8_t child_count;
};

NodeStatus selector_tick(const Node *self, NodeContext *context);
NodeStatus sequence_tick(const Node *self, NodeContext *context);

#endif