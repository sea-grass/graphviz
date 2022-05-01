/// \file
/// \brief Implementation of a dynamically expanding stack data structure

#pragma once

#include <assert.h>
#include <cgraph/list.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  list_t data;
} gv_stack_t;

static inline size_t stack_size(const gv_stack_t *stack) {
  assert(stack != NULL);
  return list_size(&stack->data);
}

static inline bool stack_is_empty(const gv_stack_t *stack) {
  assert(stack != NULL);
  return list_is_empty(&stack->data);
}

static inline int stack_try_push(gv_stack_t *stack, void *item) {

  assert(stack != NULL);

  return list_try_push_back(&stack->data, item);
}

static inline void stack_push(gv_stack_t *stack, void *item) {

  assert(stack != NULL);

  return list_push_back(&stack->data, item);
}

static inline void *stack_top(gv_stack_t *stack) {

  assert(stack != NULL);
  assert(!stack_is_empty(stack) && "access to top of an empty stack");

  return list_at(&stack->data, stack_size(stack) - 1);
}

static inline void *stack_pop(gv_stack_t *stack) {

  assert(stack != NULL);
  assert(!stack_is_empty(stack) && "access to top of an empty stack");

  return list_pop_back(&stack->data);
}

static inline void stack_reset(gv_stack_t *stack) {

  assert(stack != NULL);

  list_reset(&stack->data);
}
