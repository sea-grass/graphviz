/// @file
/// @ingroup common_render
/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <common/render.h>
#include <cgraph/agxbuf.h>
#include <cgraph/prisize_t.h>
#include <gvc/gvc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define YDIR(y) (Y_invert ? (Y_off - (y)) : (y))
#define YFDIR(y) (Y_invert ? (YF_off - (y)) : (y))

static double Y_off;        /* ymin + ymax */
static double YF_off;       /* Y_off in inches */

double yDir (double y)
{
    return YDIR(y);
}

static void agputs(int (*putstr)(void *chan, const char *str), const char* s,
                   FILE* fp) {
    putstr(fp, s);
}

static void agputc(int (*putstr)(void *chan, const char *str), char c,
                   FILE *fp) {
    static char buf[2] = {'\0','\0'};
    buf[0] = c;
    putstr(fp, buf);
}

static void printstring(int (*putstr)(void *chan, const char *str), FILE *f,
                        char *prefix, char *s) {
  if (prefix) agputs(putstr, prefix, f);
  agputs(putstr, s, f);
}

static void printint(int (*putstr)(void *chan, const char *str), FILE *f,
                     char *prefix, size_t i) {
  agxbuf buf = {0};
    
  if (prefix) agputs(putstr, prefix, f);
  agxbprint(&buf, "%" PRISIZE_T, i);
  agputs(putstr, agxbuse(&buf), f);
  agxbfree(&buf);
}

static void printdouble(int (*putstr)(void *chan, const char *str), FILE *f,
                        char *prefix, double v) {
  agxbuf buf = {0};
    
  if (prefix) agputs(putstr, prefix, f);
  agxbprint(&buf, "%.5g", v);
  agputs(putstr, agxbuse(&buf), f);
  agxbfree(&buf);
}

static void printpoint(int (*putstr)(void *chan, const char *str), FILE *f,
                       pointf p) {
  printdouble(putstr, f, " ", PS2INCH(p.x));
  printdouble(putstr, f, " ", PS2INCH(YDIR(p.y)));
}

/* setYInvert:
 * Set parameters used to flip coordinate system (y=0 at top).
 * Values do not need to be unset, since if Y_invert is set, it's
 * set for * all graphs during current run, so each will 
 * reinitialize the values for its bbox.
 */
static void setYInvert(graph_t * g)
{
    if (Y_invert) {
	Y_off = GD_bb(g).UR.y + GD_bb(g).LL.y;
	YF_off = PS2INCH(Y_off);
    }
}

/* canon:
 * Canonicalize a string which may not have been allocated using agstrdup.
 */
static char* canon (graph_t *g, char* s)
{
    char* ns = agstrdup (g, s);
    char* cs = agcanonStr (ns);
    agstrfree (g, ns);
    return cs;
}

static void writenodeandport(int (*putstr)(void *chan, const char *str),
                             FILE *f, node_t *node, char *portname) {
    char *name;
    if (IS_CLUST_NODE(node))
	name = canon (agraphof(node), strchr(agnameof(node), ':') + 1);
    else
	name = agcanonStr (agnameof(node));
    printstring(putstr, f, " ", name); /* slimey i know */
    if (portname && *portname)
	printstring(putstr, f, ":", agcanonStr(portname));
}

void write_plain(GVJ_t *job, graph_t *g, FILE *f, bool extend) {
    char *tport, *hport;
    node_t *n;
    edge_t *e;
    bezier bz;
    pointf pt;
    char *lbl;
    char* fillcolor;

    int (*putstr)(void *chan, const char *str) = g->clos->disc.io->putstr;
    setYInvert(g);
    pt = GD_bb(g).UR;
    printdouble(putstr, f, "graph ", job->zoom);
    printdouble(putstr, f, " ", PS2INCH(pt.x));
    printdouble(putstr, f, " ", PS2INCH(pt.y));
    agputc(putstr, '\n', f);
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (IS_CLUST_NODE(n))
	    continue;
	printstring(putstr, f, "node ", agcanonStr(agnameof(n)));
	printpoint(putstr, f, ND_coord(n));
	if (ND_label(n)->html)   /* if html, get original text */
	    lbl = agcanonStr (agxget(n, N_label));
	else
	    lbl = canon(agraphof(n),ND_label(n)->text);
        printdouble(putstr, f, " ", ND_width(n));
        printdouble(putstr, f, " ", ND_height(n));
        printstring(putstr, f, " ", lbl);
	printstring(putstr, f, " ", late_nnstring(n, N_style, "solid"));
	printstring(putstr, f, " ", ND_shape(n)->name);
	printstring(putstr, f, " ", late_nnstring(n, N_color, DEFAULT_COLOR));
	fillcolor = late_nnstring(n, N_fillcolor, "");
        if (fillcolor[0] == '\0')
	    fillcolor = late_nnstring(n, N_color, DEFAULT_FILL);
	printstring(putstr, f, " ", fillcolor);
	agputc(putstr, '\n', f);
    }
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	for (e = agfstout(g, n); e; e = agnxtout(g, e)) {

	    if (extend) {		//assuming these two attrs have already been created by cgraph
		if (!(tport = agget(e,"tailport")))
		    tport = "";
		if (!(hport = agget(e,"headport")))
		    hport = "";
	    }
	    else
		tport = hport = "";
	    if (ED_spl(e)) {
		size_t splinePoints = 0;
		for (size_t i = 0; i < ED_spl(e)->size; i++) {
		    bz = ED_spl(e)->list[i];
		    splinePoints += bz.size;
		}
		printstring(putstr, f, NULL, "edge");
		writenodeandport(putstr, f, agtail(e), tport);
		writenodeandport(putstr, f, aghead(e), hport);
		printint(putstr, f, " ", splinePoints);
		for (size_t i = 0; i < ED_spl(e)->size; i++) {
		    bz = ED_spl(e)->list[i];
		    for (size_t j = 0; j < bz.size; j++)
			printpoint(putstr, f, bz.list[j]);
		}
	    }
	    if (ED_label(e)) {
		printstring(putstr, f, " ", canon(agraphof(agtail(e)),ED_label(e)->text));
		printpoint(putstr, f, ED_label(e)->pos);
	    }
	    printstring(putstr, f, " ", late_nnstring(e, E_style, "solid"));
	    printstring(putstr, f, " ", late_nnstring(e, E_color, DEFAULT_COLOR));
	    agputc(putstr, '\n', f);
	}
    }
    agputs(putstr, "stop\n", f);
}

static void set_record_rects(node_t * n, field_t * f, agxbuf * xb)
{
    int i;

    if (f->n_flds == 0) {
	agxbprint(xb, "%.5g,%.5g,%.5g,%.5g ",
		f->b.LL.x + ND_coord(n).x,
		YDIR(f->b.LL.y + ND_coord(n).y),
		f->b.UR.x + ND_coord(n).x,
		YDIR(f->b.UR.y + ND_coord(n).y));
    }
    for (i = 0; i < f->n_flds; i++)
	set_record_rects(n, f->fld[i], xb);
}

static void rec_attach_bb(graph_t * g, Agsym_t* bbsym, Agsym_t* lpsym, Agsym_t* lwsym, Agsym_t* lhsym)
{
    int c;
    agxbuf buf = {0};
    pointf pt;

    agxbprint(&buf, "%.5g,%.5g,%.5g,%.5g", GD_bb(g).LL.x, YDIR(GD_bb(g).LL.y),
              GD_bb(g).UR.x, YDIR(GD_bb(g).UR.y));
    agxset(g, bbsym, agxbuse(&buf));
    if (GD_label(g) && GD_label(g)->text[0]) {
	pt = GD_label(g)->pos;
	agxbprint(&buf, "%.5g,%.5g", pt.x, YDIR(pt.y));
	agxset(g, lpsym, agxbuse(&buf));
	pt = GD_label(g)->dimen;
	agxbprint(&buf, "%.2f", PS2INCH(pt.x));
	agxset(g, lwsym, agxbuse(&buf));
	agxbprint(&buf, "%.2f", PS2INCH(pt.y));
	agxset(g, lhsym, agxbuse(&buf));
    }
    for (c = 1; c <= GD_n_cluster(g); c++)
	rec_attach_bb(GD_clust(g)[c], bbsym, lpsym, lwsym, lhsym);
    agxbfree(&buf);
}

void attach_attrs_and_arrows(graph_t *g, bool *sp, bool *ep) {
    node_t *n;
    edge_t *e;
    pointf ptf;
    int dim3 = (GD_odim(g) >= 3);
    Agsym_t* bbsym = NULL;
    Agsym_t* lpsym = NULL;
    Agsym_t* lwsym = NULL;
    Agsym_t* lhsym = NULL;

    gv_fixLocale (1);
    bool e_arrows = false; // graph has edges with end arrows
    bool s_arrows = false; // graph has edges with start arrows
    setYInvert(g);
    agxbuf xb = {0};
    safe_dcl(g, AGNODE, "pos", "");
    safe_dcl(g, AGNODE, "rects", "");
    N_width = safe_dcl(g, AGNODE, "width", "");
    N_height = safe_dcl(g, AGNODE, "height", "");
    safe_dcl(g, AGEDGE, "pos", "");
    if (GD_has_labels(g) & NODE_XLABEL)
	safe_dcl(g, AGNODE, "xlp", "");
    if (GD_has_labels(g) & EDGE_LABEL)
	safe_dcl(g, AGEDGE, "lp", "");
    if (GD_has_labels(g) & EDGE_XLABEL)
	safe_dcl(g, AGEDGE, "xlp", "");
    if (GD_has_labels(g) & HEAD_LABEL)
	safe_dcl(g, AGEDGE, "head_lp", "");
    if (GD_has_labels(g) & TAIL_LABEL)
	safe_dcl(g, AGEDGE, "tail_lp", "");
    if (GD_has_labels(g) & GRAPH_LABEL) {
	lpsym = safe_dcl(g, AGRAPH, "lp", "");
	lwsym = safe_dcl(g, AGRAPH, "lwidth", "");
	lhsym = safe_dcl(g, AGRAPH, "lheight", "");
    }
    bbsym = safe_dcl(g, AGRAPH, "bb", "");
    for (n = agfstnode(g); n; n = agnxtnode(g, n)) {
	if (dim3) {
	    int k;

	    agxbprint(&xb, "%.5g,%.5g,%.5g", ND_coord(n).x, YDIR(ND_coord(n).y), POINTS_PER_INCH*(ND_pos(n)[2]));
	    for (k = 3; k < GD_odim(g); k++) {
		agxbprint(&xb, ",%.5g", POINTS_PER_INCH*(ND_pos(n)[k]));
	    }
	    agset(n, "pos", agxbuse(&xb));
	} else {
	    agxbprint(&xb, "%.5g,%.5g", ND_coord(n).x, YDIR(ND_coord(n).y));
	    agset(n, "pos", agxbuse(&xb));
	}
	agxbprint(&xb, "%.5g", PS2INCH(ND_ht(n)));
	agxset(n, N_height, agxbuse(&xb));
	agxbprint(&xb, "%.5g", PS2INCH(ND_lw(n) + ND_rw(n)));
	agxset(n, N_width, agxbuse(&xb));
	if (ND_xlabel(n) && ND_xlabel(n)->set) {
	    ptf = ND_xlabel(n)->pos;
	    agxbprint(&xb, "%.5g,%.5g", ptf.x, YDIR(ptf.y));
	    agset(n, "xlp", agxbuse(&xb));
	}
	if (strcmp(ND_shape(n)->name, "record") == 0) {
	    set_record_rects(n, ND_shape_info(n), &xb);
	    agxbpop(&xb);	/* get rid of last space */
	    agset(n, "rects", agxbuse(&xb));
	} else {
	    polygon_t *poly;
	    if (N_vertices && isPolygon(n)) {
		poly = ND_shape_info(n);
		size_t sides = poly->sides;
		if (sides < 3) {
		    char *p = agget(n, "samplepoints");
		    if (p)
			sides = strtoul(p, NULL, 0);
		    else
			sides = 8;
		    if (sides < 3)
			sides = 8;
		}
		for (size_t i = 0; i < sides; i++) {
		    if (i > 0)
			agxbputc(&xb, ' ');
		    if (poly->sides >= 3)
			agxbprint(&xb, "%.5g %.5g",
				PS2INCH(poly->vertices[i].x),
				YFDIR(PS2INCH(poly->vertices[i].y)));
		    else
			agxbprint(&xb, "%.5g %.5g",
				ND_width(n) / 2.0 * cos((double)i / (double)sides * M_PI * 2.0),
				YFDIR(ND_height(n) / 2.0 * sin((double)i / (double)sides * M_PI * 2.0)));
		}
		agxset(n, N_vertices, agxbuse(&xb));
	    }
	}
	if (State >= GVSPLINES) {
	    for (e = agfstout(g, n); e; e = agnxtout(g, e)) {
		if (ED_edge_type(e) == IGNORED)
		    continue;
		if (ED_spl(e) == NULL)
		    continue;	/* reported in postproc */
		for (size_t i = 0; i < ED_spl(e)->size; i++) {
		    if (i > 0)
			agxbputc(&xb, ';');
		    if (ED_spl(e)->list[i].sflag) {
			s_arrows = true;
			agxbprint(&xb, "s,%.5g,%.5g ",
				ED_spl(e)->list[i].sp.x,
				YDIR(ED_spl(e)->list[i].sp.y));
		    }
		    if (ED_spl(e)->list[i].eflag) {
			e_arrows = true;
			agxbprint(&xb, "e,%.5g,%.5g ",
				ED_spl(e)->list[i].ep.x,
				YDIR(ED_spl(e)->list[i].ep.y));
		    }
		    for (size_t j = 0; j < ED_spl(e)->list[i].size; j++) {
			if (j > 0)
			    agxbputc(&xb, ' ');
			ptf = ED_spl(e)->list[i].list[j];
			agxbprint(&xb, "%.5g,%.5g", ptf.x, YDIR(ptf.y));
		    }
		}
		agset(e, "pos", agxbuse(&xb));
		if (ED_label(e)) {
		    ptf = ED_label(e)->pos;
		    agxbprint(&xb, "%.5g,%.5g", ptf.x, YDIR(ptf.y));
		    agset(e, "lp", agxbuse(&xb));
		}
		if (ED_xlabel(e) && ED_xlabel(e)->set) {
		    ptf = ED_xlabel(e)->pos;
		    agxbprint(&xb, "%.5g,%.5g", ptf.x, YDIR(ptf.y));
		    agset(e, "xlp", agxbuse(&xb));
		}
		if (ED_head_label(e)) {
		    ptf = ED_head_label(e)->pos;
		    agxbprint(&xb, "%.5g,%.5g", ptf.x, YDIR(ptf.y));
		    agset(e, "head_lp", agxbuse(&xb));
		}
		if (ED_tail_label(e)) {
		    ptf = ED_tail_label(e)->pos;
		    agxbprint(&xb, "%.5g,%.5g", ptf.x, YDIR(ptf.y));
		    agset(e, "tail_lp", agxbuse(&xb));
		}
	    }
	}
    }
    rec_attach_bb(g, bbsym, lpsym, lwsym, lhsym);
    agxbfree(&xb);

    if (HAS_CLUST_EDGE(g))
	undoClusterEdges(g);
    
    *sp = s_arrows;
    *ep = e_arrows;
    gv_fixLocale (0);
}

void attach_attrs(graph_t * g)
{
    bool e, s;
    attach_attrs_and_arrows (g, &s, &e);
}

