/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

/*Open GL basic component set
  includes glPanel,glCompButton,glCompCustomButton,clCompLabel,glCompStyle
*/
#ifndef GLCOMPSET_H
#define GLCOMPSET_H

#include <glcomp/glcompfont.h>
#include <glcomp/glcomptextpng.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern void glCompInitCommon(glCompObj * childObj,
				 glCompObj * parentObj, GLfloat x,
				 GLfloat y);
    void glCompEmptyCommon(glCompCommon * c);
    extern glCompSet *glCompSetNew(int w, int h);
    extern int glCompSetDraw(glCompSet * s);
    extern void glcompsetUpdateBorder(glCompSet * s, int w, int h);
    extern void glCompDrawBegin(void);
    extern void glCompDrawEnd(void);
    extern void glCompSetAddObj(glCompSet * s, glCompObj * obj);
    glCompObj *glCompGetObjByMouse(glCompSet * s, glCompMouse * m,
				   int onlyClickable);

#ifdef __cplusplus
}
#endif
#endif
