// output test for `dot -?V`

#ifdef NDEBUG
#error this is not intended to be compiled with assertions off
#endif

#include <assert.h>
#include <graphviz/gvc.h>
#include <stdio.h>

static const lt_symlist_t lt_preloaded_symbols[] = {{0, 0}};
extern int GvExitOnUsage;

static const char usage_info[] =
    "Usage: dot [-Vv?] [-(GNE)name=val] [-(KTlso)<val>] <dot files>\n"
    "(additional options for neato)    [-x] [-n<v>]\n"
    "(additional options for fdp)      [-L(gO)] [-L(nUCT)<val>]\n"
    "(additional options for memtest)  [-m<v>]\n"
    "(additional options for config)  [-cv]\n"
    "\n"
    " -V          - Print version and exit\n"
    " -v          - Enable verbose mode \n"
    " -Gname=val  - Set graph attribute 'name' to 'val'\n"
    " -Nname=val  - Set node attribute 'name' to 'val'\n"
    " -Ename=val  - Set edge attribute 'name' to 'val'\n"
    " -Tv         - Set output format to 'v'\n"
    " -Kv         - Set layout engine to 'v' (overrides default based on "
    "command name)\n"
    " -lv         - Use external library 'v'\n"
    " -ofile      - Write output to 'file'\n"
    " -O          - Automatically generate an output filename based on the "
    "input filename with a .'format' appended. (Causes all -ofile options to "
    "be ignored.) \n"
    " -P          - Internally generate a graph of the current plugins. \n"
    " -q[l]       - Set level of message suppression (=1)\n"
    " -s[v]       - Scale input by 'v' (=72)\n"
    " -y          - Invert y coordinate in output\n"
    "\n"
    " -n[v]       - No layout mode 'v' (=1)\n"
    " -x          - Reduce graph\n"
    "\n"
    " -Lg         - Don't use grid\n"
    " -LO         - Use old attractive force\n"
    " -Ln<i>      - Set number of iterations to i\n"
    " -LU<i>      - Set unscaled factor to i\n"
    " -LC<v>      - Set overlap expansion factor to v\n"
    " -LT[*]<v>   - Set temperature (temperature factor) to v\n"
    "\n"
    " -m          - Memory test (Observe no growth with top. Kill when done.)\n"
    " -m[v]       - Memory test - v iterations.\n"
    "\n"
    " -c          - Configure plugins (Writes $prefix/lib/graphviz/config \n"
    "               with available plugin information.  Needs write "
    "privilege.)\n"
    " -?          - Print usage and exit\n";

int main(void) {
  GVC_t *Gvc = gvContextPlugins(lt_preloaded_symbols, DEMAND_LOADING);
  GvExitOnUsage = 0;
  int argc = 2;
  char *argv[] = {"dot", "-?V"};

  gvParseArgs(Gvc, argc, argv);

  fflush(stdout);
  printf("expected: %s", usage_info);

  return 0;
}
