/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*	This header file is for library writers who need to know certain
**	internal info concerning the full Sfio_t structure. Including this
**	file means that you agree to track closely with sfio development
**	in case its internal architecture is changed.
**
**	Written by Kiem-Phong Vo
*/

#include	<sfio/sfio.h>

/* mode bit to indicate that the structure hasn't been initialized */
#define SF_INIT		0000004u

/* short-hand for common stream types */
#define SF_RDWR		(SF_READ|SF_WRITE)
#define SF_RDSTR	(SF_READ|SF_STRING)
#define SF_WRSTR	(SF_WRITE|SF_STRING)
#define SF_RDWRSTR	(SF_RDWR|SF_STRING)

#ifdef __cplusplus
}
#endif
