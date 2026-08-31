#ifndef NODE_H
#define NODE_H

#include <stdint.h>

// Forward-declared, not #included: NodeContext only ever holds pointers to
// these, and both Context (context.h) and Enemy (enemy.h) transitively pull
// in this header (Context via Enemy enemies[MAX_ENEMIES], Enemy via
// Enemy.behavior_tree) — #including either one back here would be a cycle.
// A .c file that dereferences game_context/enemy needs to #include the real
// headers itself.
struct Context;
struct Enemy;

typedef enum NodeStatus {
    NODE_RUNNING,
    NODE_SUCCESS,
    NODE_FAILURE,
} NodeStatus;

typedef struct NodeContext {
    struct Context *game_context;
    struct Enemy *enemy;
} NodeContext;

typedef struct Node Node;

typedef NodeStatus (*NodeTickFn)(const Node *self, NodeContext *context);

struct Node {
    NodeTickFn tick;
    const Node *const *children; // NULL for leaves
    uint8_t child_count;
};

// Try each child in order; the first that doesn't fail wins.
NodeStatus selector_tick(const Node *self, NodeContext *context);
// Run each child in order; the first that doesn't succeed wins.
NodeStatus sequence_tick(const Node *self, NodeContext *context);

#endif