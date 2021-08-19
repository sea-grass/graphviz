/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/


/*
 * Written by Stephen North
 * Updated by Emden Gansner
 */

/*
 * reads a sequence of graphs on stdin, and writes their
 * transitive reduction on stdout
 */

#include "config.h"

#include <gvc/gvc.h>
#include <cgraph/cgraph.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <ingraphs/ingraphs.h>

#include <getopt.h>

char **Files;
char *CmdName;

#ifdef _WIN32 //*dependencies
    #pragma comment( lib, "cgraph.lib" )
    #pragma comment( lib, "ingraphs.lib" )
    #pragma comment( lib, "gvc.lib" )
#endif

static char *useString = "Usage: %s [-?] <files>\n\
  -? - print usage\n\
If no files are specified, stdin is used\n";

static void usage(int v)
{
    printf(useString, CmdName);
    exit(v);
}

static void init(int argc, char *argv[])
{
    int c;

    CmdName = argv[0];
    opterr = 0;
    while ((c = getopt(argc, argv, ":?")) != -1) {
	switch (c) {
	case '?':
	    if (optopt == '\0')
		usage(0);
	    else {
		fprintf(stderr, "%s: option -%c unrecognized\n",
			CmdName, optopt);
		usage(1);
	    }
	    break;
	}
    }
    argv += optind;
    argc -= optind;

    if (argc)
	Files = argv;
}

static Agraph_t *gread(FILE * fp)
{
    return agread(fp, NULL);
}

int main(int argc, char **argv)
{
    Agraph_t *g;
    ingraph_state ig;
    int rc = 0;

    init(argc, argv);
    newIngraph(&ig, Files, gread);

    while ((g = nextGraph(&ig)) != 0) {
	if (agisdirected(g)) {
	    if ((rc = gvToolTred(g)))
                break;
            agwrite(g, stdout);
            fflush(stdout);
        }
	agclose(g);
    }

    return rc;
}

