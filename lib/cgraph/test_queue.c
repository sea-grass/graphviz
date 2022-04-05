// basic unit tester for queue.h

#ifdef NDEBUG
#error this is not intended to be compiled with assertions off
#endif

#include <assert.h>
#include <cgraph/queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// a queue should start in a known initial state
static void test_init(void) {
  queue_t q = {0};
  assert(queue_is_empty(&q));
  assert(queue_size(&q) == 0);
}

// reset of an initialized queue should be OK and idempotent
static void test_init_reset(void) {
  queue_t q = {0};
  queue_reset(&q);
  queue_reset(&q);
  queue_reset(&q);
}

// basic push then pop
static void test_push_one(void) {
  queue_t q = {0};
  void *arbitrary = (void *)0x42;
  int r = queue_push(&q, arbitrary);
  assert(r == 0);
  assert(queue_size(&q) == 1);
  void *top = queue_pop(&q);
  assert(top == arbitrary);
  assert(queue_is_empty(&q));
  queue_reset(&q);
}

static void push_then_pop(size_t count) {
  queue_t q = {0};
  for (uintptr_t i = 0; i < (uintptr_t)count; ++i) {
    int r = queue_push(&q, (void *)i);
    assert(r == 0);
    assert(queue_size(&q) == (size_t)i + 1);
  }
  for (uintptr_t i = 0; i < (uintptr_t)count; --i) {
    assert(queue_size(&q) == count - (size_t)i);
    void *p = queue_pop(&q);
    assert((uintptr_t)p == i);
  }
  queue_reset(&q);
}

// push a series of items
static void test_push_then_pop_ten(void) { push_then_pop(10); }

// push enough to cause an expansion
static void test_push_then_pop_many(void) { push_then_pop(4096); }

// interleave some push and pop operations
static void test_push_pop_interleaved(void) {
  queue_t q = {0};
  size_t size = 0;
  uintptr_t j = 0;
  for (uintptr_t i = 0; i < 4096; ++i) {
    if (i % 3 == 1) {
      void *p = queue_pop(&q);
      assert((uintptr_t)p == j);
      ++j;
      if (j % 3 == 1) {
        // this value would not have been pushed
        ++j;
      }
      --size;
    } else {
      int r = queue_push(&q, (void *)i);
      assert(r == 0);
      ++size;
    }
    assert(queue_size(&q) == size);
  }
  queue_reset(&q);
}

int main(void) {

#define RUN(t)                                                                 \
  do {                                                                         \
    printf("running test_%s... ", #t);                                         \
    fflush(stdout);                                                            \
    test_##t();                                                                \
    printf("OK\n");                                                            \
  } while (0)

  RUN(init);
  RUN(init_reset);
  RUN(push_one);
  RUN(push_then_pop_ten);
  RUN(push_then_pop_many);
  RUN(push_pop_interleaved);

#undef RUN

  return EXIT_SUCCESS;
}
