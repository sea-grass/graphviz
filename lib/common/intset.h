/// @file
/// @ingroup common_utils
/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#pragma once

#include <cdt.h>
#include <stddef.h>

typedef struct {
    size_t       id;
    Dtlink_t  link;
} intitem;

extern Dt_t* openIntSet (void);
void addIntSet(Dt_t *, size_t);
int inIntSet(Dt_t *, size_t);
