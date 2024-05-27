/// @file
/// @brief unordered set of `Agsubnode_t *`

#pragma once

#include <assert.h>
#include <cgraph/cgraph.h>
#include <stdbool.h>
#include <stddef.h>

/// an unordered set
typedef struct graphviz_node_set node_set_t;

/// construct a new set
///
/// Calls `exit` on failure (out-of-memory).
///
/// @return A constructed set
node_set_t *node_set_new(void);

/// add an item to the set
///
/// If the backing store is not large enough, it is expanded on demand. On
/// allocation failure, `exit` is called.
///
/// @param self Set to add to
/// @param item Element to add
void node_set_add(node_set_t *self, Agsubnode_t *item);

/// lookup an existing item in a set
///
/// Only the `key->node` member of the `key` needs to be valid.
///
/// @param self Set to search
/// @param key Subnode to look for
/// @return The found item or `NULL` if it was not in the set
Agsubnode_t *node_set_find(node_set_t *self, const Agsubnode_t *key);

/// remove an item from a set
///
/// If the given item was not in the set, this is a no-op.
///
/// @param self Set to remove from
/// @param item Element to remove
void node_set_remove(node_set_t *self, const Agsubnode_t *item);

/// get the number of items in a set
///
/// @param self Set to query
/// @return Number of elements in the set
size_t node_set_size(const node_set_t *self);

/// is this set empty?
///
/// @param self Set to query
/// @return True if this set contains nothing
static inline bool node_set_is_empty(const node_set_t *self) {
  assert(self != NULL);
  return node_set_size(self) == 0;
}

/// destruct a set
///
/// `*self` is `NULL` on return.
///
/// @param self Set to destroy
void node_set_free(node_set_t **self);
