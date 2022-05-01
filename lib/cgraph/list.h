/// \file
/// \brief Implementation of a dynamically expanding array
///
/// This is analogous to C++’s `std::vector`

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
  size_t size;     ///< number of elements in the list
  size_t capacity; ///< total number of elements that can fit without expansion
} list_t;

static inline size_t list_size(const list_t *list) {
  assert(list != NULL);
  return list->size;
}

static inline bool list_is_empty(const list_t *list) {
  assert(list != NULL);
  return list_size(list) == 0;
}

static inline int list_try_push_back(list_t *list, void *item) {

  assert(list != NULL);

  // do we need to expand the list to make room for this item?
  if (list->size == list->capacity) {

    // Capacity to allocate on the first push to a `list_t`. We pick something
    // that works out to an allocation of 4KB, a common page size on multiple
    // platforms, as a reasonably efficient default.
    enum { FIRST_ALLOCATION = 4096 / sizeof(void *) };

    // will our resize calculation overflow?
    if (UNLIKELY(SIZE_MAX / 2 < list->capacity)) {
      return EOVERFLOW;
    }

    size_t c = list->capacity == 0 ? FIRST_ALLOCATION : (2 * list->capacity);
    void **b = realloc(list->base, sizeof(b[0]) * c);
    if (UNLIKELY(b == NULL)) {
      return ENOMEM;
    }
    list->capacity = c;
    list->base = b;
  }

  assert(list->base != NULL);
  assert(list->capacity > list->size);

  // insert the new item
  list->base[list->size] = item;
  ++list->size;

  return 0;
}

static inline void list_push_back(list_t *list, void *item) {

  assert(list != NULL);

  int r = list_try_push_back(list, item);
  if (UNLIKELY(r != 0)) {
    fprintf(stderr, "list_try_push_back failed: %s\n", strerror(r));
    graphviz_exit(EXIT_FAILURE);
  }
}

static inline void list_set(list_t *list, size_t index, void *value) {

  assert(list != NULL);
  assert(list->size > index && "list access out of range");

  list->base[index] = value;
}

static inline const void *list_get(const list_t *list, size_t index) {

  assert(list != NULL);
  assert(list->size > index && "list access out of range");

  return list->base[index];
}

static inline void *list_at(list_t *list, size_t index) {
  return (void *)list_get(list, index);
}

static inline void *list_pop_back(list_t *list) {

  assert(list != NULL);
  assert(!list_is_empty(list));

  void *back = list_at(list, list->size - 1);
  --list->size;
  return back;
}

static inline void list_reset(list_t *list) {

  assert(list != NULL);

  free(list->base);
  memset(list, 0, sizeof(*list));
}
