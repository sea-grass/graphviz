// exit test for `dot -?V`

#ifdef NDEBUG
#error this is not intended to be compiled with assertions off
#endif

#include <assert.h>
#include <graphviz/gvc.h>
#include <stdbool.h>

lt_symlist_t lt_preloaded_symbols[] = {{0, 0}};
extern int GvExitOnUsage;

int main(void) {
  GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
  GvExitOnUsage = 1;
  int argc = 2;
  char *argv[] = {"dot", "-?V"};

  gvParseArgs(Gvc, argc, argv);

  // fail this test if the function above does not call exit.
  assert(false);

  return 0;
}
