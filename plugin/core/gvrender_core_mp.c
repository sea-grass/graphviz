/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

/* FIXME - incomplete replacement for codegen */

#include "config.h"
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#endif

#include <cgraph/prisize_t.h>
#include <cgraph/streq.h>
#include <cgraph/unreachable.h>
#include <common/macros.h>
#include <common/const.h>

#include <gvc/gvplugin_render.h>
#include <gvc/gvplugin_device.h>
#include <gvc/gvio.h>
#include <cgraph/agxbuf.h>
#include <common/utils.h>
#include <common/color.h>

/* Number of points to split splines into */
#define BEZIERSUBDIVISION 6

typedef enum { FORMAT_MP, } format_type;

static int Depth;

static void mpptarray(GVJ_t *job, pointf *A, size_t n, int close) {
    for (size_t i = 0; i < n; i++) {
        gvprintf(job, " %.0f %.0f", A[i].x, A[i].y);
    }
    if (close) {
        gvprintf(job, " %.0f %.0f", A[0].x, A[0].y);
    }
    gvputs(job, "\n");
}

static int mpColorResolve(int *new, unsigned char r, unsigned char g,
  unsigned char b)
{
#define maxColors 256
    static int top = 0;
    static short red[maxColors], green[maxColors], blue[maxColors];
    int c;
    int ct = -1;
    long rd, gd, bd, dist;
    long mindist = 3 * 255 * 255;       /* init to max poss dist */

    *new = 0;                   /* in case it is not a new color */
    for (c = 0; c < top; c++) {
        rd = (long) (red[c] - r);
        gd = (long) (green[c] - g);
        bd = (long) (blue[c] - b);
        dist = rd * rd + gd * gd + bd * bd;
        if (dist < mindist) {
            if (dist == 0)
                return c;       /* Return exact match color */
            mindist = dist;
            ct = c;
        }
    }
    /* no exact match.  We now know closest, but first try to allocate exact */
    if (top++ == maxColors)
        return ct;              /* Return closest available color */
    red[c] = r;
    green[c] = g;
    blue[c] = b;
    *new = 1;                   /* flag new color */
    return c;                   /* Return newly allocated color */
}

/* this table is in xfig color index order */
static const char *mpcolor[] = {"black", "blue",    "green",  "cyan",
                                "red",   "magenta", "yellow", "white"};

static void mp_resolve_color(GVJ_t *job, gvcolor_t * color)
{
    int object_code = 0;        /* always 0 for color */
    int i, new;

    switch (color->type) {
	case COLOR_STRING:
	    for (i = 0; i < (int)(sizeof(mpcolor) / sizeof(mpcolor[0])); i++) {
		if (streq(mpcolor[i], color->u.string)) {
		    color->u.index = i;
		    break;
		}
	    }
	    break;
	case RGBA_BYTE:
	    i = 32 + mpColorResolve(&new,
			color->u.rgba[0],
			color->u.rgba[1],
			color->u.rgba[2]);
	    if (new)
		gvprintf(job, "%d %d #%02x%02x%02x\n",
			object_code, i,
			color->u.rgba[0],
			color->u.rgba[1],
			color->u.rgba[2]);
	    color->u.index = i;
	    break;
        case HSVA_DOUBLE: /* TODO: implement color conversion */
	    color->u.index = 0;
            break;
	default:
	    UNREACHABLE(); // internal error
    }

    color->type = COLOR_INDEX;
}

static void mp_line_style(obj_state_t *obj, int *line_style, double *style_val)
{
    switch (obj->pen) {
	case PEN_DASHED: 
	    *line_style = 1;
	    *style_val = 10.;
	    break;
	case PEN_DOTTED:
	    *line_style = 2;
	    *style_val = 10.;
	    break;
	case PEN_SOLID:
	default:
	    *line_style = 0;
	    *style_val = 0.;
	    break;
    }
}

static void mp_comment(GVJ_t *job, char *str)
{
    gvprintf(job, "# %s\n", str);
}

static void mp_begin_graph(GVJ_t * job)
{
    obj_state_t *obj = job->obj;

    gvputs(job, "#FIG 3.2\n");
    gvprintf(job, "# Generated by %s version %s (%s)\n",
	job->common->info[0], job->common->info[1], job->common->info[2]);
    gvprintf(job, "# Title: %s\n", agnameof(obj->u.g));
    gvprintf(job, "# Pages: %d\n", job->pagesArraySize.x * job->pagesArraySize.y);
    gvputs(job, "Portrait\n"); /* orientation */
    gvputs(job, "Center\n");   /* justification */
    gvputs(job, "Inches\n");   /* units */
    gvputs(job, "Letter\n");   /* papersize */
    gvputs(job, "100.00\n");   /* magnification % */
    gvputs(job, "Single\n");   /* multiple-page */
    gvputs(job, "-2\n");       /* transparent color (none) */
    gvputs(job, "1200");	     /* resolution */
    gvputs(job, " 2\n");       /* coordinate system (upper left) */
}

static void mp_end_graph(GVJ_t * job)
{
    gvputs(job, "# end of FIG file\n");
}

static void mp_begin_page(GVJ_t * job)
{
    (void)job;

    Depth = 2;
}

static void mp_begin_node(GVJ_t * job)
{
    (void)job;

    Depth = 1;
}

static void mp_end_node(GVJ_t * job)
{
    (void)job;

    Depth = 2;
}

static void mp_begin_edge(GVJ_t * job)
{
    (void)job;

    Depth = 0;
}

static void mp_end_edge(GVJ_t * job)
{
    (void)job;

    Depth = 2;
}

static void mp_textspan(GVJ_t * job, pointf p, textspan_t * span)
{
    obj_state_t *obj = job->obj;

    int object_code = 4;        /* always 4 for text */
    int sub_type = 0;           /* text justification */
    int color = obj->pencolor.u.index;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int font = -1;		/* init to xfig's default font */
    double font_size = span->font->size * job->zoom;
    double angle = job->rotation ? (M_PI / 2.0) : 0.0;
    int font_flags = 4;		/* PostScript font */
    double height = 0.0;
    double length = 0.0;

    if (span->font->postscript_alias) /* if it is a standard postscript font */
	font = span->font->postscript_alias->xfig_code; 

    switch (span->just) {
    case 'l':
        sub_type = 0;
        break;
    case 'r':
        sub_type = 2;
        break;
    default:
    case 'n':
        sub_type = 1;
        break;
    }

    gvprintf(job,
            "%d %d %d %d %d %d %.1f %.4f %d %.1f %.1f %.0f %.0f",
            object_code, sub_type, color, depth, pen_style, font,
            font_size, angle, font_flags, height, length, round(p.x),
            round(p.y));
    gvputs_nonascii(job, span->str);
    gvputs(job, "\\001\n");
}

static void mp_ellipse(GVJ_t * job, pointf * A, int filled)
{
    obj_state_t *obj = job->obj;

    int object_code = 1;        /* always 1 for ellipse */
    int sub_type = 1;           /* ellipse defined by radii */
    int line_style;		/* solid, dotted, dashed */
    double thickness = round(obj->penwidth);
    int pen_color = obj->pencolor.u.index;
    int fill_color = obj->fillcolor.u.index;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int area_fill = filled ? 20 : -1;
    double style_val;
    int direction = 0;
    double angle = 0.0;
    double center_x, center_y;

    mp_line_style(obj, &line_style, &style_val);

    const double start_x = center_x = round(A[0].x);
    const double start_y = center_y = round(A[0].y);
    const double radius_x = round(A[1].x - A[0].x);
    const double radius_y = round(A[1].y - A[0].y);
    const double end_x = round(A[1].x);
    const double end_y = round(A[1].y);

    gvprintf(job,
            "%d %d %d %.0f %d %d %d %d %d %.3f %d %.4f %.0f %.0f %.0f %.0f "
            "%.0f %.0f %.0f %.0f\n",
            object_code, sub_type, line_style, thickness, pen_color,
            fill_color, depth, pen_style, area_fill, style_val, direction,
            angle, center_x, center_y, radius_x, radius_y, start_x,
            start_y, end_x, end_y);
}

static void mp_bezier(GVJ_t *job, pointf *A, size_t n, int filled) {
    obj_state_t *obj = job->obj;

    int object_code = 3;        /* always 3 for spline */
    int sub_type;
    int line_style;		/* solid, dotted, dashed */
    double thickness = round(obj->penwidth);
    int pen_color = obj->pencolor.u.index;
    int fill_color = obj->fillcolor.u.index;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int area_fill;
    double style_val;
    int cap_style = 0;
    int forward_arrow = 0;
    int backward_arrow = 0;

    pointf pf, V[4];
    int step;
    int count = 0;

    agxbuf buf = {0};

    mp_line_style(obj, &line_style, &style_val);

    if (filled) {
        sub_type = 5;     /* closed X-spline */
        area_fill = 20;   /* fully saturated color */
        fill_color = job->obj->fillcolor.u.index;
    }
    else {
        sub_type = 4;     /* opened X-spline */
        area_fill = -1;
        fill_color = 0;
    }
    V[3].x = A[0].x;
    V[3].y = A[0].y;
    /* Write first point in line */
    count++;
    agxbprint(&buf, " %.0f %.0f", A[0].x, A[0].y);
    /* write subsequent points */
    for (size_t i = 0; i + 3 < n; i += 3) {
        V[0] = V[3];
        for (size_t j = 1; j <= 3; j++) {
            V[j].x = A[i + j].x;
            V[j].y = A[i + j].y;
        }
        for (step = 1; step <= BEZIERSUBDIVISION; step++) {
            count++;
            pf = Bezier(V, (double)step / BEZIERSUBDIVISION, NULL, NULL);
            agxbprint(&buf, " %.0f %.0f", pf.x, pf.y);
        }
    }

    gvprintf(job, "%d %d %d %.0f %d %d %d %d %d %.1f %d %d %d %d\n",
            object_code,
            sub_type,
            line_style,
            thickness,
            pen_color,
            fill_color,
            depth,
            pen_style,
            area_fill,
            style_val, cap_style, forward_arrow, backward_arrow, count);

    gvprintf(job, " %s\n", agxbuse(&buf));      /* print points */
    agxbfree(&buf);
    for (int i = 0; i < count; i++) {
        gvprintf(job, " %d", i % (count + 1) ? 1 : 0);   /* -1 on all */
    }
    gvputs(job, "\n");
}

static void mp_polygon(GVJ_t *job, pointf *A, size_t n, int filled) {
    obj_state_t *obj = job->obj;

    int object_code = 2;        /* always 2 for polyline */
    int sub_type = 3;           /* always 3 for polygon */
    int line_style;		/* solid, dotted, dashed */
    double thickness = round(obj->penwidth);
    int pen_color = obj->pencolor.u.index;
    int fill_color = obj->fillcolor.u.index;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int area_fill = filled ? 20 : -1;
    double style_val;
    int join_style = 0;
    int cap_style = 0;
    int radius = 0;
    int forward_arrow = 0;
    int backward_arrow = 0;
    const size_t npoints = n + 1;

    mp_line_style(obj, &line_style, &style_val);

    gvprintf(job,
            "%d %d %d %.0f %d %d %d %d %d %.1f %d %d %d %d %d %" PRISIZE_T "\n",
            object_code, sub_type, line_style, thickness, pen_color,
            fill_color, depth, pen_style, area_fill, style_val, join_style,
            cap_style, radius, forward_arrow, backward_arrow, npoints);
    mpptarray(job, A, n, 1); // closed shape
}

static void mp_polyline(GVJ_t *job, pointf *A, size_t n) {
    obj_state_t *obj = job->obj;

    int object_code = 2;        /* always 2 for polyline */
    int sub_type = 1;           /* always 1 for polyline */
    int line_style;		/* solid, dotted, dashed */
    double thickness = round(obj->penwidth);
    int pen_color = obj->pencolor.u.index;
    int fill_color = 0;
    int depth = Depth;
    int pen_style = 0;          /* not used */
    int area_fill = 0;
    double style_val;
    int join_style = 0;
    int cap_style = 0;
    int radius = 0;
    int forward_arrow = 0;
    int backward_arrow = 0;
    const size_t npoints = n;

    mp_line_style(obj, &line_style, &style_val);

    gvprintf(job,
            "%d %d %d %.0f %d %d %d %d %d %.1f %d %d %d %d %d %" PRISIZE_T "\n",
            object_code, sub_type, line_style, thickness, pen_color,
            fill_color, depth, pen_style, area_fill, style_val, join_style,
            cap_style, radius, forward_arrow, backward_arrow, npoints);
    mpptarray(job, A, n, 0); // open shape
}

gvrender_engine_t mp_engine = {
    0,				/* mp_begin_job */
    0,				/* mp_end_job */
    mp_begin_graph,
    mp_end_graph,
    0,				/* mp_begin_layer */
    0,				/* mp_end_layer */
    mp_begin_page,
    0,				/* mp_end_page */
    0,				/* mp_begin_cluster */
    0,				/* mp_end_cluster */
    0,				/* mp_begin_nodes */
    0,				/* mp_end_nodes */
    0,				/* mp_begin_edges */
    0,				/* mp_end_edges */
    mp_begin_node,
    mp_end_node,
    mp_begin_edge,
    mp_end_edge,
    0,				/* mp_begin_anchor */
    0,				/* mp_end_anchor */
    0,				/* mp_begin_label */
    0,				/* mp_end_label */
    mp_textspan,
    mp_resolve_color,
    mp_ellipse,
    mp_polygon,
    mp_bezier,
    mp_polyline,
    mp_comment,
    0,				/* mp_library_shape */
};

static gvrender_features_t render_features_mp = {
    0,                          /* flags */
    4.,                         /* default pad - graph units */
    NULL,                       /* knowncolors */
    0,                          /* sizeof knowncolors */
    HSVA_DOUBLE,                /* color_type */
};

static gvdevice_features_t device_features_mp = {
    0,                          /* flags */
    {0.,0.},                    /* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {72.,72.},                  /* default dpi */
};

gvplugin_installed_t gvrender_mp_types[] = {
    {FORMAT_MP, "mp", -1, &mp_engine, &render_features_mp},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_mp_types[] = {
    {FORMAT_MP, "mp:mp", -1, NULL, &device_features_mp},
    {0, NULL, 0, NULL, NULL}
};

