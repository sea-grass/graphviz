/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <assert.h>
#include <cgraph/alloc.h>
#include <cgraph/bitarray.h>
#include <dotgen/dot.h>
#include <stddef.h>

/*
 * Author: Mohammad T. Irfan
 *   Summer, 2008
 */

/* TODO:
 *   - Support clusters
 *   - Support disconnected graphs
 *   - Provide algorithms for aspect ratios < 1
 */

#define DEF_PASSES 5

/* countDummyNodes:
 *  Count the number of dummy nodes
 */
int countDummyNodes(graph_t * g)
{
    int count = 0;
    node_t *n;
    edge_t *e;

    /* Count dummy nodes */
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		/* flat edges do not have dummy nodes */
	    if (ND_rank(aghead(e)) != ND_rank(agtail(e)))	
		count += abs(ND_rank(aghead(e)) - ND_rank(agtail(e))) - 1;
	}
    }
    return count;
}

void setAspect(Agraph_t *g, aspect_t *adata) {
    double rv;
    char *p;
    int r, passes = DEF_PASSES;

    p = agget (g, "aspect");

    if (!p || ((r = sscanf (p, "%lf,%d", &rv, &passes)) <= 0)) {
	adata->nextIter = 0;
	adata->nPasses = 0;
	return;
    }
    agerr (AGWARN, "the aspect attribute has been disabled due to implementation flaws - attribute ignored.\n");
    adata->nextIter = 0;
    adata->nPasses = 0;
    return;
}
