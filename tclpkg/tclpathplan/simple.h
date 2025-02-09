/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <stddef.h>

#define MAXINTS  10000		/* modify this line to reflect the max no. of 
				   intersections you want reported -- 50000 seems to break the program */

#define SLOPE(p,q) ( ( ( p.y ) - ( q.y ) ) / ( ( p.x ) - ( q.x ) ) )
#define MAX(a,b) ( ( a ) > ( b ) ? ( a ) : ( b ) )

#define after(v) (((v)==((v)->poly->finish))?((v)->poly->start):((v)+1))
#define prior(v) (((v)==((v)->poly->start))?((v)->poly->finish):((v)-1))

    struct position {
	float x, y;
    };


    struct vertex {
	struct position pos;
	struct polygon *poly;
	struct active_edge *active;
    };

    struct polygon {
	struct vertex *start, *finish;
    };

    struct intersection {
	struct vertex *firstv, *secondv;
	struct polygon *firstp, *secondp;
	float x, y;
    };

    struct active_edge {
	struct vertex *name;
	struct active_edge *next, *last;
    };
    struct active_edge_list {
	struct active_edge *first, *final;
	int number;
    };
    struct data {
	size_t nvertices;
	int ninters;
    };

void find_ints(struct vertex vertex_list[], struct data *input,
               struct intersection ilist[]);

/// detect whether lines `l` and `m` intersect
void find_intersection(struct vertex *l, struct vertex *m,
                       struct intersection ilist[], struct data *input);
