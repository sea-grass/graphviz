/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include "viewportcamera.h"
#include "gui.h"
#include <math.h>
#include <common/memory.h>
#include <glcomp/glcompbutton.h>
#include <glcomp/glcomplabel.h>
#include <glcomp/glcomppanel.h>


static viewport_camera *new_viewport_camera(ViewInfo * view)
{
    return NEW(viewport_camera);
}

static void viewport_update_camera_indexes(ViewInfo * view)
{
    int ind = 0;
    for (ind = 0; ind < view->camera_count; ind++) {
	view->cameras[ind]->index = ind;
    }
}

static viewport_camera *add_camera_to_viewport(ViewInfo * view)
{
    view->camera_count++;
    view->cameras =
	RALLOC(view->camera_count, view->cameras, viewport_camera *);
    view->cameras[view->camera_count - 1] = new_viewport_camera(view);
    view->active_camera = view->camera_count - 1;
    viewport_update_camera_indexes(view);
    return view->cameras[view->camera_count - 1];
}

void menu_click_add_camera(void *p)
{
    viewport_camera *c;
    /*add test cameras */
    c = add_camera_to_viewport(view);
    c->targetx = view->panx;
    c->targety = view->pany;
    c->targetz = view->panz;
    c->x = view->panx;
    c->y = view->pany;
    c->z = view->zoom;
    c->camera_vectorx = 0;
    c->camera_vectory = 1;
    c->camera_vectorz = 0;

    c->anglexy = 90;
    c->anglexyz = 0;
    c->anglex = 0;
    c->angley = 0;
    c->anglez = 0;

    c->r = view->zoom * -1;
}
