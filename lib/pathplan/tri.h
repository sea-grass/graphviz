/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <pathgeom.h>

#ifdef GVDLL
#ifdef PATHPLAN_EXPORTS
#define TRI_API __declspec(dllexport)
#else
#define TRI_API __declspec(dllimport)
#endif
#endif

#ifndef TRI_API
#define TRI_API /* nothing */
#endif

/* Points in polygon must be in CCW order */
TRI_API int Ptriangulate(Ppoly_t *polygon,
                         void (*fn)(void *closure, Ppoint_t tri[]), void *vc);

#undef TRI_API

/// return value from `ccw`
enum {
  ISCCW = 1, ///< counter-clockwise
  ISCW = 2,  ///< clockwise
  ISON = 3,  ///< co-linear
};

/// are the given points counter-clockwise, clockwise, or co-linear?
int ccw(Ppoint_t p1, Ppoint_t p2, Ppoint_t p3);

/// is pb between pa and pc?
bool between(Ppoint_t pa, Ppoint_t pb, Ppoint_t pc);

/// line to line intersection
bool intersects(Ppoint_t pa, Ppoint_t pb, Ppoint_t pc, Ppoint_t pd);

typedef Ppoint_t (*indexer_t)(void *base, int index);

/// is (i, i + 2) a diagonal?
bool isdiagonal(int i, int ip2, void *pointp, int pointn, indexer_t indexer);

#ifdef __cplusplus
}
#endif
