/**
 * @file
 * @brief  <a href=https://en.wikipedia.org/wiki/GXL>GXL</a> converting program
 */

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
 * Written by Emden R. Gansner and Krishnam Pericherla 
 */

#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "convert.h"
#include "openFile.h"
#include <cgraph/gv_ctype.h>
#include <cgraph/ingraphs.h>
#include <util/exit.h>

typedef enum { Unset, ToGV, ToGXL } mode;

static FILE *outFile;
static char *CmdName;
static char **Files;
static mode act = Unset;

#ifdef HAVE_EXPAT
static FILE *getFile(void)
{
    FILE *rv = NULL;
    static FILE *savef = NULL;
    static int cnt = 0;

    if (Files == NULL) {
	if (cnt++ == 0) {
	    rv = stdin;
	}
    } else {
	if (savef)
	    fclose(savef);
	while (Files[cnt]) {
	    if ((rv = fopen(Files[cnt++], "r")) != 0)
		break;
	    else
		fprintf(stderr, "Can't open %s\n", Files[cnt - 1]);
	}
    }
    savef = rv;
    return rv;
}
#endif

static const char *use = "Usage: %s [-gd?] [-o<file>] [<graphs>]\n\
 -g        : convert to GXL\n\
 -d        : convert to GV\n\
 -o<file>  : output to <file> (stdout)\n\
 -?        : usage\n";

static void usage(int v)
{
    fprintf(stderr, use, CmdName);
    graphviz_exit(v);
}

static char *cmdName(char *path)
{
    char *sp;

    sp = strrchr(path, '/');
    if (sp)
	sp++;
    else
	sp = path;
#ifdef _WIN32
    char *sp2 = strrchr(sp, '\\');
    if (sp2 != NULL)
	sp = sp2 + 1;
#endif
    return sp;
}

static void checkInput(void)
{
    char *ep;

    ep = strrchr(*Files, '.');
    if (!ep)
	return;
    ep++;
    if (strcmp(ep, "gv") == 0)
	act = ToGXL;
    else if (strcmp(ep, "dot") == 0)
	act = ToGXL;
    else if (strcmp(ep, "gxl") == 0)
	act = ToGV;
}

static void setAction(void)
{
    if (gv_tolower(CmdName[0]) == 'd') 
	act = ToGXL;
    else if (gv_tolower(CmdName[0]) == 'g') {
	if (gv_tolower(CmdName[1]) == 'v')
	    act = ToGXL;
	else
	    act = ToGV;
    }
    else if (Files)
	checkInput();

    if (act == Unset) {
	fprintf(stderr, "Cannot determine conversion type\n");
	usage(1);
    }
}

static void initargs(int argc, char **argv)
{
    int c;

    CmdName = cmdName(argv[0]);
    opterr = 0;
    while ((c = getopt(argc, argv, ":gdo:")) != -1) {
	switch (c) {
	case 'd':
	    act = ToGV;
	    break;
	case 'g':
	    act = ToGXL;
	    break;
	case 'o':
	    if (outFile != NULL)
		fclose(outFile);
	    outFile = openFile(CmdName, optarg, "w");
	    break;
	case ':':
	    fprintf(stderr, "%s: option -%c missing argument\n", CmdName, optopt);
	    break;
	case '?':
	    if (optopt == '?')
		usage(0);
	    else {
		fprintf(stderr, "%s: option -%c unrecognized\n", CmdName,
			optopt);
		graphviz_exit(1);
	    }
	    break;
	default:
	    fprintf(stderr, "cvtgxl: unexpected error\n");
	    graphviz_exit(EXIT_FAILURE);
	}
    }

    argv += optind;
    argc -= optind;

    if (argc > 0)
	Files = argv;
    if (!outFile)
	outFile = stdout;
    if (act == Unset)
	setAction();
}

int main(int argc, char **argv)
{
    Agraph_t *G;
    Agraph_t *prev = 0;

    initargs(argc, argv);
    if (act == ToGXL) {
	ingraph_state ig;
	newIngraph(&ig, Files);

	while ((G = nextGraph(&ig))) {
	    if (prev)
		agclose(prev);
	    prev = G;
	    gv_to_gxl(G, outFile);
	    fflush(outFile);
	}
    } else {
#ifdef HAVE_EXPAT
	FILE *inFile;
	while ((inFile = getFile())) {
	    while ((G = gxl_to_gv(inFile))) {
		if (prev)
		    agclose(prev);
		prev = G;
		agwrite(G, outFile);
		fflush(outFile);
	    }
	}
#else
	fputs("cvtgxl: not configured for conversion from GXL to GV\n",
	      stderr);
	graphviz_exit(1);

#endif
    }
    graphviz_exit(0);
}
