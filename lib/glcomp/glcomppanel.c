/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <cgraph/alloc.h>
#include <glcomp/glcomppanel.h>
#include <glcomp/glcompfont.h>
#include <glcomp/glcompset.h>
#include <glcomp/glcomptexture.h>
#include <glcomp/glutils.h>
#include <stdbool.h>

static void glCompPanelDraw(void *o) {
  glCompPanel *p = o;
  glCompRect r;
  glCompCommon ref = p->base.common;
  glCompCalcWidget(p->base.common.parent, &p->base.common, &ref);
  p->base.objType = glPanelObj;

  if (!p->base.common.visible)
    return;
  /*draw shadow */
  glColor4f(p->shadowcolor.R, p->shadowcolor.G, p->shadowcolor.B,
            p->shadowcolor.A);
  r.h = p->shadowwidth;
  r.w = ref.width;
  r.pos.x = ref.pos.x + p->shadowwidth;
  r.pos.y = ref.pos.y - p->shadowwidth;
  r.pos.z = -0.001f;
  glCompDrawRectangle(&r);
  r.h = ref.height;
  r.w = p->shadowwidth;
  r.pos.x = ref.pos.x + ref.width;
  r.pos.y = ref.pos.y - p->shadowwidth;
  r.pos.z = -0.001f;
  glCompDrawRectangle(&r);
  /*draw panel */
  glCompDrawRectPrism(&ref.pos, ref.width, ref.height,
                      p->base.common.borderWidth, 0.01f, &ref.color, true);
  /*draw image if there is */
  if (p->image) {
    p->image->base.common.callbacks.draw(p->image);
  }
}

glCompPanel *glCompPanelNew(void *parentObj, float x, float y, float w,
                            float h) {
    glCompPanel *p = gv_alloc(sizeof(glCompPanel));
    glCompInitCommon(&p->base, parentObj, x, y);

    p->shadowcolor.R = GLCOMPSET_PANEL_SHADOW_COLOR_R;
    p->shadowcolor.G = GLCOMPSET_PANEL_SHADOW_COLOR_G;
    p->shadowcolor.B = GLCOMPSET_PANEL_SHADOW_COLOR_B;
    p->shadowcolor.A = GLCOMPSET_PANEL_SHADOW_COLOR_A;
    p->shadowwidth = GLCOMPSET_PANEL_SHADOW_WIDTH;
    p->base.common.borderWidth = GLCOMPSET_PANEL_BORDERWIDTH;

    p->base.common.width = w;
    p->base.common.height = h;

    p->base.common.font = glNewFontFromParent(&p->base, NULL);
    p->text = NULL;
    p->base.common.functions.draw = glCompPanelDraw;
    p->image = NULL;
    return p;
}
