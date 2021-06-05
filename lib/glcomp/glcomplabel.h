/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/
#ifndef GLCOMPLABEL_H
#define GLCOMPLABEL_H

#include <glcomp/glcompdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern glCompLabel *glCompLabelNew(glCompObj * par, GLfloat x,
				       GLfloat y, char *text);

/*events*/
    extern int glCompLabelDraw(glCompLabel * p);

#ifdef __cplusplus
}
#endif
#endif
