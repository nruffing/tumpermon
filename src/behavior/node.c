#include "node.h"

NodeStatus selector_tick(const Node *self, NodeContext *context)
{
    // Try each child in order; the first that doesn't fail wins.
    for (uint8_t i = 0; i < self->child_count; i++) {
        NodeStatus status = self->children[i]->tick(self->children[i], context);
        if (status != NODE_FAILURE) {
            return status;
        }
    }
    return NODE_FAILURE;
}

NodeStatus sequence_tick(const Node *self, NodeContext *context)
{
    // Run each child in order; the first that doesn't succeed wins.
    for (uint8_t i = 0; i < self->child_count; i++) {
        NodeStatus status = self->children[i]->tick(self->children[i], context);
        if (status != NODE_SUCCESS) {
            return status;
        }
    }
    return NODE_SUCCESS;
}