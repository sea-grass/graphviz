/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

/* 
 * in_poly
 * 
 * Test if a point is inside a polygon.
 * The polygon must be convex with vertices in CW order.
 */

#include <stdlib.h>
#include <pathplan/vispath.h>
#include <pathplan/pathutil.h>
#include <stdbool.h>
#include <stddef.h>

bool in_poly(Ppoly_t poly, Ppoint_t q) {
    Ppoint_t *P;

    P = poly.ps;
    const size_t n = poly.pn;
    for (size_t i = 0; i < n; i++) {
	const size_t i1 = (i + n - 1) % n; // point index; i1 = i-1 mod n
	if (wind(P[i1], P[i], q) == 1) return false;
    }
    return true;
}
