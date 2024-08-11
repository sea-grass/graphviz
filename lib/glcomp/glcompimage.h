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

#include <glcomp/glcompdefs.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern glCompImage *glCompImageNewFile(float x, float y, char *imgfile);
    extern glCompImage *glCompImageNew(glCompObj *par, float x, float y);
    extern void glCompImageDelete(glCompImage * p);
    extern int glCompImageLoad(glCompImage *i, unsigned char *data, int width,
                               int height, bool is2D);
int glCompImageLoadPng(glCompImage *i, const char *pngFile);
    extern void glCompImageDraw(void *obj);

#ifdef __cplusplus
}
#endif
