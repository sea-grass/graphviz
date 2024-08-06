/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <neatogen/voronoi.h>
#include <neatogen/poly.h>

    typedef struct {		/* Info concerning site */
	Agnode_t *node;		/* libgraph node */
	Site site;		/* site used by voronoi code */
	bool overlaps; ///< true if node overlaps other nodes
	Poly poly;		/* polygon at node */
	Point *verts; ///< sorted list of vertices of voronoi polygon
	size_t n_verts; ///< number of elements in `verts`
    } Info_t;

    extern Info_t *nodeInfo;	/* Array of node info */

    /* Insert vertex into sorted list */
    extern void addVertex(Site *, double, double);

#ifdef __cplusplus
}
#endif
