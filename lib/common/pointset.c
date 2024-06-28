/// @file
/// @ingroup common_utils
/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <cgraph/alloc.h>
#include <common/render.h>
#include <common/pointset.h>
#include <stddef.h>

typedef struct {
    Dtlink_t link;
    pointf id;
} pair;

static pair *mkPair(pointf p) {
    pair *pp = gv_alloc(sizeof(pair));
    pp->id = p;
    return pp;
}

static void freePair(pair *pp, Dtdisc_t *disc) {
    (void)disc;

    free (pp);
}

static int cmppair(void *k1, void *k2) {
    const pointf *key1 = k1;
    const pointf *key2 = k2;
    if (key1->x > key2->x)
	return 1;
    else if (key1->x < key2->x)
	return -1;
    else if (key1->y > key2->y)
	return 1;
    else if (key1->y < key2->y)
	return -1;
    else
	return 0;
}

static Dtdisc_t intPairDisc = {
    offsetof(pair, id),
    sizeof(pointf),
    offsetof(pair, link),
    0,
    (Dtfree_f) freePair,
    cmppair,
};

PointSet *newPS(void)
{
    return (dtopen(&intPairDisc, Dtoset));
}

void freePS(PointSet * ps)
{
    dtclose(ps);
}

void insertPS(PointSet *ps, pointf pt) {
    pair *pp;

    pp = mkPair(pt);
    if (dtinsert(ps, pp) != pp)
        free(pp);
}

void addPS(PointSet *ps, double x, double y) {
    const pointf pt = {.x = x, .y = y};
    pair *pp = mkPair(pt);
    if (dtinsert(ps, pp) != pp)
        free(pp);
}

int inPS(PointSet *ps, pointf pt) {
    pair p;
    p.id = pt;
    return dtsearch(ps, &p) ? 1 : 0;
}

int isInPS(PointSet *ps, double x, double y) {
  return inPS(ps, (pointf){.x = x, .y = y});
}

int sizeOf(PointSet * ps)
{
    return dtsize(ps);
}

pointf *pointsOf(PointSet *ps) {
    const size_t n = (size_t)dtsize(ps);
    pointf *pts = gv_calloc(n, sizeof(pointf));
    pair *p;
    pointf *pp = pts;

    for (p = (pair *) dtflatten(ps); p;
	 p = (pair *)dtlink(ps, p)) {
	*pp++ = p->id;
    }

    return pts;
}

typedef struct {
    Dtlink_t link;
    point id;
    int v;
} mpair;

typedef struct {
    Dtdisc_t disc;
} MPairDisc;

static mpair *mkMPair(mpair *obj, MPairDisc *disc) {
    (void)disc;
    mpair *ap = gv_alloc(sizeof(mpair));
    ap->id = obj->id;
    ap->v = obj->v;
    return ap;
}

static void freeMPair(mpair *ap, MPairDisc *disc) {
  (void)disc;
  free(ap);
}

static Dtdisc_t intMPairDisc = {
    offsetof(mpair, id),
    sizeof(point),
    offsetof(mpair, link),
    (Dtmake_f) mkMPair,
    (Dtfree_f) freeMPair,
    cmppair,
};

PointMap *newPM(void)
{
  return dtopen(&intMPairDisc, Dtoset);
}

void clearPM(PointMap * ps)
{
    dtclear(ps);
}

void freePM(PointMap * ps)
{
    dtclose(ps);
}

int insertPM(PointMap * pm, int x, int y, int value)
{
    mpair *p;
    mpair dummy;

    dummy.id.x = x;
    dummy.id.y = y;
    dummy.v = value;
    p = dtinsert(pm, &dummy);
    return p->v;
}
