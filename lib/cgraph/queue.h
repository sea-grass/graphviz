/**
 * @file
 * @brief generic first-in-first-out buffer (queue)
 *
 * The header-only implementation below defines a queue with an optional initial
 * capacity that expands on-demand if necessary. Expansions always succeed or
 * exit on out-of-memory. A queue item is a generic pointer, `void *`.
 */

#pragma once

#include <assert.h>
#include <cgraph/alloc.h>
#include <stddef.h>
#include <string.h>

/** queue object
 *
 * A valid queue can be constructed with either C99 zero initialization,
 * `queue_t q = {0}`, or by calling `queue_new`.
 */
typedef struct {
  void **base;     ///< start of the backing memory
  size_t head;     ///< index of the first element
  size_t size;     ///< number of elements currently stored
  size_t capacity; ///< number of allocated slots in the backing memory
} queue_t;

/** create a new queue
 *
 * The hint should be a guess of the intended occupancy ceiling. If the guess is
 * inaccurate, the queue expands on-demand anyway, so there are few consequences
 * for getting it wrong. It is possible to pass 0 as the hint, but in this case
 * you are better of using zero initialization for brevity.
 *
 * @param hint Number of initial slots to allocate
 * @return An empty queue
 */
static inline queue_t queue_new(size_t hint) {
  queue_t q = {.base = gv_calloc(hint, sizeof(void *)), .capacity = hint};
  return q;
}

/** insert a new item at the tail of a queue
 *
 * It is possible to push `NULL` entries. But then you will be unable to
 * distinguish popping this entry from the queue being empty.
 *
 * @param q Queue to operate on
 * @param item Entry to insert
 */
static inline void queue_push(queue_t *q, void *item) {
  assert(q != NULL);

  // do we need to expand the backing memory?
  if (q->size == q->capacity) {
    const size_t c = q->capacity == 0 ? 1 : 2 * q->capacity;
    q->base = gv_recalloc(q->base, q->capacity, c, sizeof(void *));

    // Do we need to shuffle the prefix upwards? E.g.
    //
    //        ┌───┬───┬───┬───┐
    //   old: │ 3 │ 4 │ 1 │ 2 │
    //        └───┴───┴─┼─┴─┼─┘
    //                  │   └───────────────┐
    //                  └───────────────┐   │
    //                                  ▼   ▼
    //        ┌───┬───┬───┬───┬───┬───┬───┬───┐
    //   new: │ 3 │ 4 │   │   │   │   │ 1 │ 2 │
    //        └───┴───┴───┴───┴───┴───┴───┴───┘
    if (q->head + q->size > q->capacity) {
      const size_t prefix = q->capacity - q->head;
      const size_t new_head = c - prefix;
      memmove(&q->base[new_head], &q->base[q->head], prefix * sizeof(void *));
      q->head = new_head;
    }

    q->capacity = c;
  }

  q->base[(q->head + q->size) % q->capacity] = item;
  ++q->size;
}

/** remove an item from the head of a queue
 *
 * @param q Queue to operate on
 * @return The popped entry on success or `NULL` if the queue was empty
 */
static inline void *queue_pop(queue_t *q) {
  assert(q != NULL);

  // return null if the queue is empty
  if (q->size == 0) {
    return NULL;
  }

  void *item = q->base[q->head];
  q->head = (q->head + 1) % q->capacity;
  --q->size;
  return item;
}

/** deallocate backing memory and re-initialize a queue
 *
 * @param q Queue to operate on
 */
static inline void queue_free(queue_t *q) {
  assert(q != NULL);
  free(q->base);
  *q = (queue_t){0};
}
