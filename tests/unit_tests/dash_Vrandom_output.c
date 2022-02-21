// output test for `dot -Vrandom`

#ifdef NDEBUG
#error this is not intended to be compiled with assertions off
#endif

#include <assert.h>
#include <graphviz/graphviz_version.h>
#include <graphviz/gvc.h>
#include <stdio.h>

lt_symlist_t lt_preloaded_symbols[] = {{0, 0}};
extern int GvExitOnUsage;

int main(void) {
  GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
  GvExitOnUsage = 0;
  int argc = 2;
  char *argv[] = {"dot", "-Vrandom"};

  const char expected_stderr[] =
      "dot - graphviz version " PACKAGE_VERSION " (" GV_VERSION ")\n";
  printf("expected: %s", expected_stderr);

  gvParseArgs(Gvc, argc, argv);

  return 0;
}
