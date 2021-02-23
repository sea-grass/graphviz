/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include "config.h"

#include <label/index.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <common/logic.h>
#include <common/arith.h>
#include <label/rectangle.h>
#include <cgraph/cgraph.h>

#define Undefined(x) ((x)->boundary[0] > (x)->boundary[NUMDIMS])

extern Rect_t CoverAll;

/*-----------------------------------------------------------------------------
| Initialize a rectangle to have all 0 coordinates.
-----------------------------------------------------------------------------*/
void InitRect(Rect_t * r)
{
    int i;
    for (i = 0; i < NUMSIDES; i++)
	r->boundary[i] = 0;
}

/*-----------------------------------------------------------------------------
| Return a rect whose first low side is higher than its opposite side -
| interpreted as an undefined rect.
-----------------------------------------------------------------------------*/
Rect_t NullRect()
{
    Rect_t r;
    int i;

    r.boundary[0] = 1;
    r.boundary[NUMDIMS] = -1;
    for (i = 1; i < NUMDIMS; i++)
	r.boundary[i] = r.boundary[i + NUMDIMS] = 0;
    return r;
}

#ifdef UNUSED
/*-----------------------------------------------------------------------------
| Fills in random coordinates in a rectangle.
| The low side is guaranteed to be less than the high side.
-----------------------------------------------------------------------------*/
RandomRect(Rect_t * r)
{
    int i, width;
    for (i = 0; i < NUMDIMS; i++) {
	/* width from 1 to 1000 / 4, more small ones */
	width = rand() % (1000 / 4) + 1;

	/* sprinkle a given size evenly but so they stay in [0,100] */
	r->boundary[i] = rand() % (1000 - width);	/* low side */
	r->boundary[i + NUMDIMS] = r->boundary[i] + width;	/* high side */
    }
}

/*-----------------------------------------------------------------------------
| Fill in the boundaries for a random search rectangle.
| Pass in a pointer to a rect that contains all the data,
| and a pointer to the rect to be filled in.
| Generated rect is centered randomly anywhere in the data area,
| and has size from 0 to the size of the data area in each dimension,
| i.e. search rect can stick out beyond data area.
-----------------------------------------------------------------------------*/
SearchRect(Rect_t * search, Rect_t * data)
{
    int i, j, size, center;

    assert(search);
    assert(data);

    for (i = 0; i < NUMDIMS; i++) {
	j = i + NUMDIMS;	/* index for high side boundary */
	if (data->boundary[i] > INT_MIN && data->boundary[j] < INT_MAX) {
	    size =
		(rand() % (data->boundary[j] - data->boundary[i] + 1)) / 2;
	    center = data->boundary[i]
		+ rand() % (data->boundary[j] - data->boundary[i] + 1);
	    search->boundary[i] = center - size / 2;
	    search->boundary[j] = center + size / 2;
	} else {		/* some open boundary, search entire dimension */
	    search->boundary[i] = INT_MIN;
	    search->boundary[j] = INT_MAX;
	}
    }
}
#endif

#ifdef RTDEBUG
/*-----------------------------------------------------------------------------
| Print rectangle lower upper bounds by dimension
-----------------------------------------------------------------------------*/
void PrintRect(Rect_t * r)
{
    int i;
    assert(r);
    fprintf(stderr, "rect:");
    for (i = 0; i < NUMDIMS; i++)
	fprintf(stderr, "\t%d\t%d\n", r->boundary[i],
		r->boundary[i + NUMDIMS]);
}
#endif

/*-----------------------------------------------------------------------------
| Calculate the n-dimensional area of a rectangle
-----------------------------------------------------------------------------*/

unsigned int RectArea(Rect_t * r)
{
  int i;
  unsigned int area;
  assert(r);

    if (Undefined(r))
	return 0;

    area = 1;
    for (i = 0; i < NUMDIMS; i++) {
      unsigned int dim = r->boundary[i + NUMDIMS] - r->boundary[i];
      if (dim == 0) return 0;
      if (UINT_MAX / dim < area) {
	agerr (AGERR, "label: area too large for rtree\n");
	return UINT_MAX;
      }
      area *= dim;
    }
    return area;
}

/*-----------------------------------------------------------------------------
| Combine two rectangles, make one that includes both.
-----------------------------------------------------------------------------*/
Rect_t CombineRect(Rect_t * r, Rect_t * rr)
{
    int i, j;
    Rect_t new;
    assert(r && rr);

    if (Undefined(r))
	return *rr;
    if (Undefined(rr))
	return *r;

    for (i = 0; i < NUMDIMS; i++) {
	new.boundary[i] = MIN(r->boundary[i], rr->boundary[i]);
	j = i + NUMDIMS;
	new.boundary[j] = MAX(r->boundary[j], rr->boundary[j]);
    }
    return new;
}

/*-----------------------------------------------------------------------------
| Decide whether two rectangles overlap.
-----------------------------------------------------------------------------*/
int Overlap(Rect_t * r, Rect_t * s)
{
    int i, j;
    assert(r && s);

    for (i = 0; i < NUMDIMS; i++) {
	j = i + NUMDIMS;	/* index for high sides */
	if (r->boundary[i] > s->boundary[j]
	    || s->boundary[i] > r->boundary[j])
	    return FALSE;
    }
    return TRUE;
}

/*-----------------------------------------------------------------------------
| Decide whether rectangle r is contained in rectangle s.
-----------------------------------------------------------------------------*/
int Contained(Rect_t * r, Rect_t * s)
{
    int i, j, result;
    assert(r && s);

    /* undefined rect is contained in any other */
    if (Undefined(r))
	return TRUE;
    /* no rect (except an undefined one) is contained in an undef rect */
    if (Undefined(s))
	return FALSE;

    result = TRUE;
    for (i = 0; i < NUMDIMS; i++) {
	j = i + NUMDIMS;	/* index for high sides */
	result = result && r->boundary[i] >= s->boundary[i]
	    && r->boundary[j] <= s->boundary[j];
    }
    return result;
}
