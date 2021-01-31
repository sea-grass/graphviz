/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

/* FIXME - incomplete replacement for codegen */

#include "config.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <common/macros.h>
#include <common/const.h>

#include <gvc/gvio.h>
#include <gvc/gvplugin_render.h>
#include <gvc/gvplugin_device.h>
#include <gvc/gvcint.h>
#include <cgraph/strcasecmp.h>

typedef enum { FORMAT_MIF, } format_type;

/* MIF font modifiers */
#define REGULAR 0
#define BOLD	1
#define ITALIC	2

/* MIF patterns */
#define P_SOLID	0
#define P_NONE  15
#define P_DOTTED 4		/* i wasn't sure about this */
#define P_DASHED 11		/* or this */

/* MIF bold line constant */
#define WIDTH_NORMAL 1
#define WIDTH_BOLD 3

static char *FillStr = "<Fill 3>";
static char *NoFillStr = "<Fill 15>";

static void mif_ptarray(GVJ_t * job, pointf * A, int n)
{
    int i;

    gvprintf(job, " <NumPoints %d>\n", n);
    for (i = 0; i < n; i++)
	gvprintf(job, " <Point %.2f %.2f>\n", A[i].x, A[i].y);
}

typedef struct {
    char *name;
    int CMYK[4];
} mif_color_t;

static mif_color_t mif_colors[] = {
    {"Black", {0, 0, 0, 100}},
    {"White", {0, 0, 0, 0}},
    {"Red", {0, 100, 100, 0}},
    {"Green", {100, 0, 100, 0}},
    {"Blue", {100, 100, 0, 0}},
    {"Cyan", {100, 0, 0, 0}},
    {"Magenta", {0, 100, 0, 0}},
    {"Yellow", {0, 0, 100, 0}},
    {"aquamarine", {100, 0, 0, 18}},
    {"plum", {0, 100, 0, 33}},
    {"peru", {0, 24, 100, 32}},
    {"pink", {0, 50, 0, 0}},
    {"mediumpurple", {10, 100, 0, 0}},
    {"grey", {0, 0, 0, 50}},
    {"lightgrey", {0, 0, 0, 25}},
    {"lightskyblue", {38, 33, 0, 0}},
    {"lightcoral", {0, 50, 60, 0}},
    {"yellowgreen", {31, 0, 100, 0}},
};

static void mif_color(GVJ_t * job, int i)
{
    if (isupper(mif_colors[i].name[0]))
	gvprintf(job, "<Separation %d>\n", i);
    else
	gvprintf(job, "<ObColor `%s'>\n", mif_colors[i].name);
}

#if 0
static void mif_style(GVJ_t * job, int filled)
{
    obj_state_t *obj = job->obj;

    gvputs(job, "style=\"fill:");
    if (filled)
	mif_color(job->obj->fillcolor);
    else
        gvputs(job, "none");

    gvputs(job, ";stroke:");
     


    while ((line = *s++)) {
	if (streq(line, "solid"))
	    pen = P_SOLID;
	else if (streq(line, "dashed"))
	    pen = P_DASHED;
	else if (streq(line, "dotted"))
	    pen = P_DOTTED;
	else if (streq(line, "invis"))
	    pen = P_NONE;
	else if (streq(line, "bold"))
	    penwidth = WIDTH_BOLD;
	else if (streq(line, "filled"))
	    fill = P_SOLID;
	else if (streq(line, "unfilled"))
	    fill = P_NONE;
	else {
	    agerr(AGERR,
		  "mif_style: unsupported style %s - ignoring\n",
		  line);
	}
    }

    fw = fa = "Regular";
    switch (cp->fontopt) {
    case BOLD:
	fw = "Bold";
	break;
    case ITALIC:
	fa = "Italic";
	break;
    }
    gvprintf(job, 
	    "<Font <FFamily `%s'> <FSize %.1f pt> <FWeight %s> <FAngle %s>>\n",
	    cp->fontfam, job->scale.x * cp->fontsz, fw, fa);

    gvprintf(job, "<Pen %d> <Fill %d> <PenWidth %d>\n",
	    job->pen, job->fill, job->penwidth);
}
#endif

static void mif_comment(GVJ_t * job, char *str)
{
    gvprintf(job, "# %s\n", str);
}

static void mif_color_declaration(GVJ_t * job, char *str, int CMYK[4])
{
    gvputs(job, " <Color \n");
    gvprintf(job, "  <ColorTag `%s'>\n", str);
    gvprintf(job, "  <ColorCyan  %d.000000>\n", CMYK[0]);
    gvprintf(job, "  <ColorMagenta  %d.000000>\n", CMYK[1]);
    gvprintf(job, "  <ColorYellow  %d.000000>\n", CMYK[2]);
    gvprintf(job, "  <ColorBlack  %d.000000>\n", CMYK[3]);
    if (isupper(str[0])) {
        gvprintf(job, "  <ColorAttribute ColorIs%s>\n", str);
        gvputs(job, "  <ColorAttribute ColorIsReserved>\n");
    }
    gvputs(job, " > # end of Color\n");
}

static void
mif_begin_job(GVJ_t * job)
{
    gvprintf(job,
	"<MIFFile 3.00> # Generated by %s version %s (%s)\n",
	job->common->info[0], job->common->info[1], job->common->info[2]);
}

static void mif_end_job(GVJ_t * job)
{
    gvputs(job, "# end of MIFFile\n");
}

static void mif_begin_graph(GVJ_t * job)
{
    int i;

    gvprintf(job, "# Title: %s\n", job->obj->u.g->name);
    gvprintf(job, "# Pages: %d\n",
	job->pagesArraySize.x * job->pagesArraySize.y);
    gvputs(job, "<Units Upt>\n");
    gvputs(job, "<ColorCatalog \n");
    for (i = 0; i < (sizeof(mif_colors) / sizeof(mif_color_t)); i++)
	mif_color_declaration(job, mif_colors[i].name, mif_colors[i].CMYK);
    gvputs(job, "> # end of ColorCatalog\n");
    gvprintf(job, "<BRect %g %g %g %g>\n",
	job->canvasBox.LL.x,
	job->canvasBox.UR.y,
	job->canvasBox.UR.x - job->canvasBox.LL.x,
	job->canvasBox.UR.y - job->canvasBox.LL.y);
}

static void
mif_begin_page(GVJ_t *job)
{
    gvprintf(job,
	    " <ArrowStyle <TipAngle 15> <BaseAngle 90> <Length %.1f> <HeadType Filled>>\n",
	    14 * job->scale.x);
}

static void mif_set_color(GVJ_t * job, char *name)
{
    int i;

    for (i = 0; i < (sizeof(mif_colors)/sizeof(mif_color_t)); i++) {
	if (strcasecmp(mif_colors[i].name, name) == 0)
	    mif_color(job, i);
    }
    agerr(AGERR, "color %s not supported in MIF\n", name);
}

static char *mif_string(GVJ_t * job, char *s)
{
    static char *buf = NULL;
    static int bufsize = 0;
    int pos = 0;
    char *p, esc;

    if (!buf) {
	bufsize = 64;
	buf = malloc(bufsize);
    }

    p = buf;
    while (*s) {
	if (pos > (bufsize - 8)) {
	    bufsize *= 2;
	    buf = realloc(buf, bufsize);
	    p = buf + pos;
	}
	esc = 0;
	switch (*s) {
	case '\t':
	    esc = 't';
	    break;
	case '>':
	case '\'':
	case '`':
	case '\\':
	    esc = *s;
	    break;
	}
	if (esc) {
	    *p++ = '\\';
	    *p++ = esc;
	    pos += 2;
	} else {
	    *p++ = *s;
	    pos++;
	}
	s++;
    }
    *p = '\0';
    return buf;
}

static void mif_textpara(GVJ_t * job, pointf p, textpara_t * para)
{
    char *anchor;

//    p.y -= para->font.size / 2 + 2;
    switch (para->just) {
    case 'l':
	anchor = "Left";
	break;
    case 'r':
	anchor = "Right";
	break;
    default:
    case 'n':
	anchor = "Center";
	break;
    }
    gvprintf(job,
	    "<TextLine <Angle %d> <TLOrigin %.2f %.2f> <TLAlignment %s>",
	    job->rotation, p.x, p.y, anchor);
    gvprintf(job, " <String `%s'>>\n", mif_string(job, para->str));
}

static void mif_bezier(GVJ_t * job, pointf * A, int n, int arrow_at_start,
		       int arrow_at_end, int filled)
{
    gvprintf(job,
	    "<PolyLine <Fill 15> <Smoothed Yes> <HeadCap Square>\n");
    mif_ptarray(job, A, n);
    gvputs(job, ">\n");
}

static void mif_polygon(GVJ_t * job, pointf * A, int n, int filled)
{
    gvprintf(job, "<Polygon %s\n", (filled ? FillStr : NoFillStr));
    mif_ptarray(job, A, n);
    gvputs(job, ">\n");
}

static void mif_ellipse(GVJ_t * job, pointf * A, int filled)
{
    pointf dia;

    dia.x = (A[1].x - A[0].x) * 2;
    dia.y = (A[1].y - A[0].y) * 2;

    gvprintf(job, "<Ellipse %s <BRect %.2f %.2f %.1f %.1f>>\n",
	    filled ? FillStr : NoFillStr,
            A[1].x - dia.x, A[1].y, dia.x, -dia.y);
}

static void mif_polyline(GVJ_t * job, pointf * A, int n)
{
    gvputs(job, "<PolyLine <HeadCap Square>\n");
    mif_ptarray(job, A, n);
    gvputs(job, ">\n");
}

gvrender_engine_t mif_engine = {
    mif_begin_job,
    mif_end_job,
    mif_begin_graph,
    0,				/* mif_end_graph */
    0,				/* mif_begin_layer */
    0,				/* mif_end_layer */
    mif_begin_page,
    0,				/* mif_end_page */
    0,				/* mif_begin_cluster */
    0,				/* mif_end_cluster */
    0,				/* mif_begin_nodes */
    0,				/* mif_end_nodes */
    0,				/* mif_begin_edges */
    0,				/* mif_end_edges */
    0,				/* mif_begin_node */
    0,				/* mif_end_node */
    0,				/* mif_begin_edge */
    0,				/* mif_end_edge */
    0,				/* mif_begin_anchor */
    0,				/* mif_end_anchor */
    0,				/* mif_begin_label */
    0,				/* mif_end_label */
    mif_textpara,
    0, 				/* mif_resolve_color */
    mif_ellipse,
    mif_polygon,
    mif_bezier,
    mif_polyline,
    mif_comment,
    0,				/* mif_library_shape */
};

gvrender_features_t mif_features = {
    GVRENDER_Y_GOES_DOWN,	/* flags */
    4.,                         /* default pad - graph units */
    NULL,			/* knowncolors */
    0,				/* sizeof knowncolors */
    CMYK_BYTE,			/* color_type */
};

static gvdevice_features_t device_features_mif = {
    0,                          /* flags */
    {0.,0.},                    /* default margin - points */
    {0.,0.},                    /* default page width, height - points */
    {72.,72.},                  /* default dpi */
};

gvplugin_installed_t gvrender_mif_types[] = {
    {FORMAT_MIF, "mif", -1, &mif_engine, &mif_features},
    {0, NULL, 0, NULL, NULL}
};

gvplugin_installed_t gvdevice_mif_types[] = {
    {FORMAT_MIF, "mif:mif", -1, NULL, &device_features_mif},
    {0, NULL, 0, NULL, NULL}
};
