/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/


#include <string.h>
#include	<vmalloc/vmhdr.h>
/*
 * return a copy of s using vmalloc
 */

char *vmstrdup(Vmalloc_t * v, const char *s)
{
    char *t;
    int n;

    return ((t =
	     vmalloc(v, n =
		     strlen(s) + 1)) ? (char *) memcpy(t, s,
						       n) : (char *) 0);
}
