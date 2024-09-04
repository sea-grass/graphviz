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

#include "config.h"
#include <stddef.h>
#include <common/intset.h>
#include <util/alloc.h>

static void *mkIntItem(void *p, Dtdisc_t *disc) {
    (void)disc;
    intitem *obj = p;

    intitem* np = gv_alloc(sizeof(intitem));
    np->id = obj->id;
    return np;
}

static int cmpid(void *k1, void *k2) {
  const size_t *key1 = k1;
  const size_t *key2 = k2;
  if (*key1 > *key2) return 1;
  else if (*key1 < *key2) return -1;
  else return 0;
}   

static Dtdisc_t intSetDisc = {
    offsetof(intitem,id),
    sizeof(int),
    offsetof(intitem,link),
    mkIntItem,
    free,
    cmpid,
};

Dt_t* 
openIntSet (void)
{
    return dtopen(&intSetDisc,Dtoset);
}

void addIntSet(Dt_t *is, size_t v) {
    intitem obj;

    obj.id = v;
    dtinsert(is, &obj);
}

int inIntSet(Dt_t *is, size_t v) {
    return (dtmatch (is, &v) != 0);
}

