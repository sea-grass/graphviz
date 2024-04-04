/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include	<sfio/sfhdr.h>
#include	<stdio.h>

/*	Read formatted data from a stream
**
**	Written by Kiem-Phong Vo.
*/

int sfscanf(FILE *f, ...) {
    va_list args;
    int rv;
    va_start(args, f);
    rv = f ? sfvscanf(f, args) : -1;
    va_end(args);
    return rv;
}
