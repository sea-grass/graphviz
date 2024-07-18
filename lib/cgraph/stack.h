/// \file
/// \brief Implementation of a dynamically expanding stack data structure
/// \ingroup cgraph_utils

#pragma once

#include <assert.h>
#include <cgraph/list.h>
#include <stdbool.h>

DEFINE_LIST(gv_stack, void *)

static inline size_t stack_size(const gv_stack_t *stack) {
  return gv_stack_size(stack);
}

static inline bool stack_is_empty(const gv_stack_t *stack) {
  return gv_stack_is_empty(stack);
}

static inline void stack_push(gv_stack_t *stack, void *item) {
  gv_stack_push_back(stack, item);
}

static inline void *stack_top(gv_stack_t *stack) {
  return *gv_stack_back(stack);
}

static inline void *stack_pop(gv_stack_t *stack) {
  return gv_stack_pop_back(stack);
}

static inline void stack_reset(gv_stack_t *stack) { gv_stack_free(stack); }
