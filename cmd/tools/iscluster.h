/*
 * This is a verbatim copy of the definitions in lib/common/utils.c
 * and should be kept in sync with it.
 * We can't import lib/common because these tools do not use the graphviz
 * rendering definitions or implementation. We can't put this function in
 *
 */
#pragma once

#include <cgraph/strcasecmp.h>
#include <stdbool.h>
static bool mapBool(const char *p, bool defaultValue) {
    if (!p || *p == '\0')
        return defaultValue;
    if (!strcasecmp(p, "false"))
        return false;
    if (!strcasecmp(p, "no"))
        return false;
    if (!strcasecmp(p, "true"))
        return true;
    if (!strcasecmp(p, "yes"))
        return true;
    if (isdigit((int)*p))
        return atoi(p) != 0;
    else
        return defaultValue;
}
static bool is_a_cluster (Agraph_t* g)
{
  return g == g->root || !strncasecmp(agnameof(g), "cluster", 7) ||
         mapBool(agget(g, "cluster"), false);
}
