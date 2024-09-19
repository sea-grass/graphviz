/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/


/* xlayout.c:
 * Written by Emden R. Gansner
 *
 * Layout routine to expand initial layout to accommodate node
 * sizes.
 */

#ifdef FIX
Allow sep to be absolute additive (margin of n points)
Increase less between tries
#endif

/* uses PRIVATE interface */
#define FDP_PRIVATE 1
#include <cgraph/gv_ctype.h>
#include <fdpgen/xlayout.h>
#include <neatogen/adjust.h>
#include <fdpgen/dbg.h>
#include <math.h>

/* Add repulsive force even if nodes don't overlap */
/* #define ORIG      */
#define BOX	/* Use bbox to determine overlap, else use circles */

#define DFLT_overlap   "9:prism"    /* default overlap value */

#define WD2(n) (X_marg.doAdd ? (ND_width(n)/2.0 + X_marg.x): ND_width(n)*X_marg.x/2.0)
#define HT2(n) (X_marg.doAdd ? (ND_height(n)/2.0 + X_marg.y): ND_height(n)*X_marg.y/2.0)

static xparams xParams = {
    60,				/* numIters */
    0.0,			/* T0 */
    0.3,			/* K */
    1.5,			/* C */
    0				/* loopcnt */
};
static double K2;
static expand_t X_marg;
static double X_nonov;
static double X_ov;

#ifdef DEBUG
static void pr2graphs(Agraph_t *g0, Agraph_t *g1) {
	fprintf(stderr,"%s",agnameof(g0));
	fprintf(stderr,"(%s)",agnameof(g1));
}
#endif

static double RAD(Agnode_t * n)
{
    double w = WD2(n);
    double h = HT2(n);
    return hypot(w, h);
}

/* xinit_params:
 * Initialize local parameters
 */
static void xinit_params(graph_t* g, int n, xparams * xpms)
{
    (void)g;

    xParams.K = xpms->K;
    xParams.numIters = xpms->numIters;
    xParams.T0 = xpms->T0;
    xParams.loopcnt = xpms->loopcnt;
    if (xpms->C > 0.0)
	xParams.C = xpms->C;
    K2 = xParams.K * xParams.K;
    if (xParams.T0 == 0.0)
	xParams.T0 = xParams.K * sqrt(n) / 5;
#ifdef DEBUG
    if (Verbose) {
	prIndent();
	fprintf(stderr, "xLayout ");
	pr2graphs(g,GORIG(agroot(g)));
	fprintf(stderr, " : n = %d K = %f T0 = %f loop %d C %f\n", 
		xParams.numIters, xParams.K, xParams.T0, xParams.loopcnt,
		xParams.C);
    }
#endif
}

#define X_T0         xParams.T0
#define X_K          xParams.K
#define X_numIters   xParams.numIters
#define X_loopcnt    xParams.loopcnt
#define X_C          xParams.C


static double cool(int t)
{
    return X_T0 * (X_numIters - t) / X_numIters;
}

/* overlap:
 * Return true if nodes overlap
 */
static int overlap(node_t * p, node_t * q)
{
#if defined(BOX)
    double xdelta, ydelta;
    int    ret;

    xdelta = fabs(ND_pos(q)[0] - ND_pos(p)[0]);
    ydelta = fabs(ND_pos(q)[1] - ND_pos(p)[1]);
    ret = xdelta <= WD2(p) + WD2(q) && ydelta <= HT2(p) + HT2(q);
    return ret;
#else
    double dist2, xdelta, ydelta;
    double din;

    din = RAD(p) + RAD(q);
    xdelta = ND_pos(q)[0] - ND_pos(p)[0];
    ydelta = ND_pos(q)[1] - ND_pos(p)[1];
    dist2 = xdelta * xdelta + ydelta * ydelta;
    return dist2 <= din * din;
#endif
}

/* cntOverlaps:
 * Return number of overlaps.
 */
static int cntOverlaps(graph_t * g)
{
    node_t *p;
    node_t *q;
    int cnt = 0;

    for (p = agfstnode(g); p; p = agnxtnode(g, p)) {
	for (q = agnxtnode(g, p); q; q = agnxtnode(g, q)) {
	    cnt += overlap(p, q);
	}
    }
    return cnt;
}

/* doRep:
 * Return 1 if nodes overlap
 */
static int
doRep(node_t * p, node_t * q, double xdelta, double ydelta, double dist2)
{
    int ov;
    double force;
#if defined(DEBUG)
    double dist;
#endif

    while (dist2 == 0.0) {
	xdelta = 5 - rand() % 10;
	ydelta = 5 - rand() % 10;
	dist2 = xdelta * xdelta + ydelta * ydelta;
    }
#ifdef ORIG
    force = K2 / dist2;
    if ((ov = overlap(p, q)))
	force *= X_C;
#else
    if ((ov = overlap(p, q)))
	force = X_ov / dist2;
    else
	force = X_nonov / dist2;
#endif
#ifdef DEBUG
    if (Verbose == 4) {
	prIndent();
	dist = sqrt(dist2);
	fprintf(stderr, " ov Fr %f dist %f\n", force * dist, dist);
    }
#endif
    DISP(q)[0] += xdelta * force;
    DISP(q)[1] += ydelta * force;
    DISP(p)[0] -= xdelta * force;
    DISP(p)[1] -= ydelta * force;
    return ov;
}

/* applyRep:
 * Repulsive force = (K*K)/d
 * Return 1 if nodes overlap
 */
static int applyRep(Agnode_t * p, Agnode_t * q)
{
    double xdelta, ydelta;

    xdelta = ND_pos(q)[0] - ND_pos(p)[0];
    ydelta = ND_pos(q)[1] - ND_pos(p)[1];
    return doRep(p, q, xdelta, ydelta, xdelta * xdelta + ydelta * ydelta);
}

static void applyAttr(Agnode_t * p, Agnode_t * q)
{
    double xdelta, ydelta;
    double force;
    double dist;
    double dout;
    double din;

    if (overlap(p, q)) {
#ifdef DEBUG
	if (Verbose == 4) {
	    prIndent();
	    fprintf(stderr, "ov 1 Fa 0 din %f\n", RAD(p) + RAD(q));
	}
#endif
	return;
    }
    xdelta = ND_pos(q)[0] - ND_pos(p)[0];
    ydelta = ND_pos(q)[1] - ND_pos(p)[1];
    dist = hypot(xdelta, ydelta);
    din = RAD(p) + RAD(q);
    dout = dist - din;
    force = dout * dout / ((X_K + din) * dist);
#ifdef DEBUG
    if (Verbose == 4) {
	prIndent();
	fprintf(stderr, " ov 0 Fa %f din %f \n", force * dist, din);
    }
#endif
    DISP(q)[0] -= xdelta * force;
    DISP(q)[1] -= ydelta * force;
    DISP(p)[0] += xdelta * force;
    DISP(p)[1] += ydelta * force;
}

/* adjust:
 * Return 0 if definitely no overlaps.
 * Return non-zero if we had overlaps before most recent move.
 */
static int adjust(Agraph_t * g, double temp)
{
    Agnode_t *n;
    Agnode_t *n1;
    Agedge_t *e;
    double temp2;
    double len;
    double len2;
    double disp[NDIM];		/* incremental displacement */
    int overlaps = 0;

#ifdef DEBUG
    if (Verbose == 4)
	fprintf(stderr, "=================\n");
#endif

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	DISP(n)[0] = DISP(n)[1] = 0;
    }

    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	int ov;
	for (n1 = agnxtnode(g, n); n1; n1 = agnxtnode(g, n1)) {
	    ov = applyRep(n, n1);
	    overlaps += ov;
	}
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
	    applyAttr(n,aghead(e));
	}
    }
    if (overlaps == 0)
	return 0;

    temp2 = temp * temp;
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (ND_pinned(n) == P_PIN)
	    continue;
	disp[0] = DISP(n)[0];
	disp[1] = DISP(n)[1];
	len2 = disp[0] * disp[0] + disp[1] * disp[1];

	if (len2 < temp2) {
	    ND_pos(n)[0] += disp[0];
	    ND_pos(n)[1] += disp[1];
	} else {
	    /* to avoid sqrt, consider abs(x) + abs(y) */
	    len = sqrt(len2);
	    ND_pos(n)[0] += disp[0] * temp / len;
	    ND_pos(n)[1] += disp[1] * temp / len;
	}
    }
    return overlaps;
}

/* x_layout:
 * Given graph g with initial layout, adjust g so that nodes
 * do not overlap.
 * Assume g is connected.
 * g may have ports. At present, we do not use ports in the layout
 * at this stage.
 * Returns non-zero if overlaps still exist.
 * TODO (possible):
 *  Allow X_T0 independent of T_TO or percentage of, so the cooling would
 * be piecewise linear. This would allow longer, cooler expansion.
 *  In tries > 1, increase X_T0 and/or lengthen cooling
 */
static int x_layout(graph_t * g, xparams * pxpms, int tries)
{
    int i;
    int try;
    int ov;
    double temp;
    int nnodes = agnnodes(g);
    int nedges = agnedges(g);
    double K;
    xparams xpms;

    X_marg = sepFactor (g);
    if (X_marg.doAdd) {
	X_marg.x = PS2INCH(X_marg.x); /* sepFactor is in points */
	X_marg.y = PS2INCH(X_marg.y);
    }
    ov = cntOverlaps(g);
    if (ov == 0)
	return 0;

    try = 0;
    xpms = *pxpms;
    K = xpms.K;
    while (ov && try < tries) {
	xinit_params(g, nnodes, &xpms);
	X_ov = X_C * K2;
	X_nonov = nedges*X_ov*2.0/(nnodes*(nnodes-1));
#ifdef DEBUG
	if (Verbose) {
	    prIndent();
	    fprintf(stderr, "try %d (%d): %d overlaps on ", try, tries, ov);
		pr2graphs(g,GORIG(agroot(g)));
		fprintf(stderr," \n");
	}
#endif

	for (i = 0; i < X_loopcnt; i++) {
	    temp = cool(i);
	    if (temp <= 0.0)
		break;
	    ov = adjust(g, temp);
	    if (ov == 0)
		break;
	}
	try++;
	xpms.K += K;		/* increase distance */
    }
#ifdef DEBUG
    if (Verbose && ov)
	fprintf(stderr, "Warning: %d overlaps remain on ", ov);
	pr2graphs(g,GORIG(agroot(g)));
	fprintf(stderr,"\n");
#endif

    return ov;
}

/* fdp_xLayout:
 * Use overlap parameter to determine if and how to remove overlaps.
 * In addition to the usual values accepted by removeOverlap, overlap
 * can begin with "n:" to indicate the given number of tries using
 * x_layout to remove overlaps.
 * Thus,
 *  NULL or ""  => dflt overlap
 *  "mode"      => "0:mode", i.e. removeOverlap with mode only
 *  "true"      => "0:true", i.e., no overlap removal
 *  "n:"        => n tries only
 *  "n:mode"    => n tries, then removeOverlap with mode
 *  "0:"        => no overlap removal
 */
void fdp_xLayout(graph_t * g, xparams * xpms)
{
    int   tries;
    char* ovlp = agget (g, "overlap");
    char* cp;
    char* rest;

    if (Verbose) {
#ifdef DEBUG
	prIndent();
#endif
        fprintf (stderr, "xLayout ");
    }
    if (!ovlp || *ovlp == '\0') {
	ovlp = DFLT_overlap;
    }
    /* look for optional ":" or "number:" */
    if ((cp = strchr(ovlp, ':')) && (cp == ovlp || gv_isdigit(*ovlp))) {
      cp++;
      rest = cp;
      tries = atoi (ovlp);
      if (tries < 0) tries = 0;
    }
    else {
      tries = 0;
      rest = ovlp;
    }
    if (Verbose) {
#ifdef DEBUG
	prIndent();
#endif
        fprintf (stderr, "tries = %d, mode = %s\n", tries, rest);
    }
    if (tries && !x_layout(g, xpms, tries))
	return;
    removeOverlapAs(g, rest);

}
