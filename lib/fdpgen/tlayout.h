/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TLAYOUT_H
#define TLAYOUT_H

#include <fdpgen/fdp.h>
#include <fdpgen/xlayout.h>

typedef enum {
  seed_unset, seed_val, seed_time, seed_regular
} seedMode;

    extern void fdp_initParams(graph_t *);
    extern void fdp_tLayout(graph_t *, xparams *);

#endif

#ifdef __cplusplus
}
#endif
