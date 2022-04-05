/// \file
/// \brief Implementation of a dynamically expanding queue data structure
///
/// This implementation maintains a backing store of physical “slots,”
/// an array of pointer elements. On top of this is layered the concept of
/// “virtual indices.” The `head` and `tail` members of the `queue_t` struct
/// hold values that are each a virtual index into a space that is potentially
/// larger than the backing memory in `base`. To find a given element, take its
/// virtual index modulo the size of the backing store. For example,
///
///   q->base[q->head % q->capacity]
///
/// This design allows a queue to be pushed to and popped from without expensive
/// relocation of the queue’s existing contents.
///
/// When the backing memory needs to be enlarged (queue occupancy exceeds its
/// current capacity), the above calculation is invalidated. That is,
/// `q->head % q->capacity` no longer finds you the same element when
/// `q->capacity` has increased. To avoid this complication, the resize
/// operation relocates the existing queue contents and resets virtual indexing
/// to 0. That is, after resizing `q->head` is 0.
///
/// There is currently no way to shrink the backing memory. A queue hits a high
/// watermark of occupancy will retain this reserved memory until it is reset.

#pragma once

#include <assert.h>
#include <cgraph/exit.h>
#include <cgraph/likely.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  void **base;     ///< underlying store of contained elements
  size_t head;     ///< virtual index to first element
  size_t tail;     ///< 1 past virtual index to last element
  size_t capacity; ///< total physical slots available
} queue_t;

static inline size_t queue_size(const queue_t *q) {
  assert(q != NULL);
  assert(q->tail >= q->head);
  assert(q->tail - q->head <= q->capacity &&
         "queue size exceeds backing memory");
  return q->tail - q->head;
}

static inline bool queue_is_empty(const queue_t *q) {
  assert(q != NULL);
  return queue_size(q) == 0;
}

static inline void *queue_pop(queue_t *q) {

  assert(q != NULL);
  assert(!queue_is_empty(q));
  assert(q->capacity != 0);

  void *item = q->base[q->head % q->capacity];
  ++q->head;

  return item;
}

static inline int queue_push(queue_t *q, void *item) {

  assert(q != NULL);

  // do we need to expand the stack to make room for this item?
  if (queue_size(q) == q->capacity) {

    // Capacity to allocate on the first push to a `queue_t`. We pick something
    // that works out to an allocation of 4KB, a common page size on multiple
    // platforms, as a reasonably efficient default.
    enum { FIRST_ALLOCATION = 4096 / sizeof(void *) };

    // will our resize calculation overflow?
    if (UNLIKELY(SIZE_MAX / 2 < q->capacity)) {
      return EOVERFLOW;
    }

    // allocate new space for the expanded queue
    size_t c = q->capacity == 0 ? FIRST_ALLOCATION : (2 * q->capacity);
    void **b = malloc(sizeof(b[0]) * c);
    if (UNLIKELY(b == NULL)) {
      return ENOMEM;
    }

    // copy all entries into the new queue, resetting the offset of the virtual
    // indices to 0
    size_t i = 0;
    while (!queue_is_empty(q)) {
      b[i] = queue_pop(q);
      ++i;
    }

    // discard the old backing memory whose contents have now been copied
    free(q->base);

    // update the queue fields to reflect the new backing memory and state
    q->base = b;
    q->head = 0;
    q->tail = i;
    q->capacity = c;
  }

  assert(q->base != NULL);
  assert(q->capacity > queue_size(q));

  // insert the new item
  q->base[q->tail % q->capacity] = item;
  ++q->tail;

  return 0;
}

static inline void queue_push_or_exit(queue_t *q, void *item) {

  assert(q != NULL);

  int r = queue_push(q, item);
  if (UNLIKELY(r != 0)) {
    fprintf(stderr, "queue_push failed: %s\n", strerror(r));
    graphviz_exit(EXIT_FAILURE);
  }
}

static inline void queue_reset(queue_t *q) {
  assert(q != NULL);
  free(q->base);
  memset(q, 0, sizeof(*q));
}
