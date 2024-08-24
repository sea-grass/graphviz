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

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#endif
#include <glcomp/opengl.h>

#ifdef __cplusplus
extern "C" {
#endif


#define	GLCOMPSET_PANEL_COLOR_R		0.16f
#define	GLCOMPSET_PANEL_COLOR_G		0.44f
#define	GLCOMPSET_PANEL_COLOR_B		0.87f
#define	GLCOMPSET_PANEL_COLOR_ALPHA	0.5f
#define	GLCOMPSET_PANEL_SHADOW_COLOR_R		0.0f
#define	GLCOMPSET_PANEL_SHADOW_COLOR_G		0.0f
#define	GLCOMPSET_PANEL_SHADOW_COLOR_B		0.0f
#define	GLCOMPSET_PANEL_SHADOW_COLOR_A		0.3f
#define GLCOMPSET_PANEL_SHADOW_WIDTH		4.0f

#define	GLCOMPSET_BUTTON_COLOR_R		0.0f
#define	GLCOMPSET_BUTTON_COLOR_G		1.0f
#define	GLCOMPSET_BUTTON_COLOR_B		0.3f
#define	GLCOMPSET_BUTTON_COLOR_ALPHA	0.6f
#define	GLCOMPSET_BUTTON_THICKNESS		3.0f
#define	GLCOMPSET_BUTTON_BEVEL_BRIGHTNESS		1.7f
#define GLCOMPSET_FONT_SIZE				14.0f

#define	GLCOMPSET_BUTTON_FONT_COLOR_R		0.0f
#define	GLCOMPSET_BUTTON_FONT_COLOR_G		0.0f
#define	GLCOMPSET_BUTTON_FONT_COLOR_B		0.0f
#define	GLCOMPSET_BUTTON_FONT_COLOR_ALPHA	1.0f

#define GLCOMPSET_FONT_SIZE_FACTOR			0.7f

#define	GLCOMPSET_LABEL_COLOR_R		0.0f
#define	GLCOMPSET_LABEL_COLOR_G		0.0f
#define	GLCOMPSET_LABEL_COLOR_B		0.0f
#define	GLCOMPSET_LABEL_COLOR_ALPHA	1.0f

#define	GLCOMPSET_FONT_COLOR_R		0.0f
#define	GLCOMPSET_FONT_COLOR_G		0.0f
#define	GLCOMPSET_FONT_COLOR_B		0.0f
#define	GLCOMPSET_FONT_COLOR_ALPHA	1.0f
#define GLCOMPSET_FONT_DESC  "Times Italic"
#define GL_FONTOPTIMIZE 1


#define GL_FONTVJUSTIFY	0
#define GL_FONTHJUSTIFY	0

#define GLCOMPSET_BORDERWIDTH				2.0f
#define GLCOMPSET_PANEL_BORDERWIDTH				3.0f
#define GLCOMPSET_BUTTON_BEVEL				5.0f
#define	GLCOMPSET_BEVEL_DIFF				0.001f
#define	GLCOMP_DEFAULT_WIDTH	10.0f
#define	GLCOMP_DEFAULT_HEIGHT	10.0f

    typedef enum { glAlignNone, glAlignLeft, glAlignTop, glAlignBottom,
	    glAlignRight, glAlignParent, glAlignCenter } glCompAlignment;

    typedef enum { glFontVJustifyNone, glFontVJustifyTop,
	    glFontVJustifyBottom, glFontVJustifyCenter } glCompVJustify;
    typedef enum { glFontHJustifyNone, glFontHJustifyLeft,
	    glFontHJustifyRight, glFontHJustifyCenter } glCompHJustify;

    typedef enum { glMouseDown, glMouseUp } glCompMouseStatus;
    typedef enum { glMouseLeftButton, glMouseRightButton,
	    glMouseMiddleButton } glMouseButtonType;

    typedef enum { glTexImage, glTexLabel } glCompTexType;
    typedef enum { glPanelObj, glButtonObj, glLabelObj,
	    glImageObj } glObjType;

    typedef struct _glCompButton glCompButton;
    typedef struct _glCompObj glCompObj;

/*call backs for widgets*/
    typedef void (*glcompdrawfunc_t) (void *obj);
    typedef void (*glcompclickfunc_t)(glCompObj *obj, float x,
				       float y, glMouseButtonType t);
    typedef void (*glcompdoubleclickfunc_t) (glCompObj * obj, float x,
					     float y,
					     glMouseButtonType t);
    typedef void (*glcompmouseoverfunc_t) (glCompObj * obj, float x,
					   float y);
    typedef void (*glcompmouseinfunc_t) (glCompObj * obj, float x,
					 float y);
    typedef void (*glcompmouseoutfunc_t) (glCompObj * obj, float x,
					  float y);
    typedef void (*glcompmousedownfunc_t) (glCompObj * obj, float x,
					   float y, glMouseButtonType t);
    typedef void (*glcompmouseupfunc_t) (glCompObj * obj, float x,
					 float y, glMouseButtonType t);
    typedef void (*glcompmousedragfunct_t) (glCompObj * obj, float dx,
					    float dy,
					    glMouseButtonType t);

    typedef struct {
	int topAnchor;		/*anchor booleans */
	int leftAnchor;
	int rightAnchor;
	int bottomAnchor;

	float top;		/*anchor values */
	float left;
	float right;
	float bottom;


    } glCompAnchor;

    typedef struct {
	glCompVJustify VJustify;
	glCompHJustify HJustify;
    } glCompJustify;

    typedef struct {
	float x, y, z;
    } glCompPoint;

    typedef struct {
	int cnt;
	glCompPoint* pts;
    }glCompPoly;

    typedef struct {
	float R;
	float G;
	float B;
	float A;		//Alpha
    } glCompColor;

    typedef struct {
	glCompPoint pos;
	float w;
	float h;
    } glCompRect;

    typedef struct {
	uint32_t id;
	char *def;
	char *text;
	float width;
	float height;
	glCompTexType type;
	int userCount;
	int fontSize;
	unsigned char *data;	/*data */
    } glCompTex;



/*opengl font*/
    typedef struct {
	char *fontdesc;		//font description , only used with pango fonts
	glCompColor color;
	void *glutfont;		/*glut font pointer if used */
	int transparent;
	glCompTex *tex;		/* texture, if type is pangotext */
	int size;
	int reference;		/*if font has references to parent */
	glCompJustify justify;
	bool is2D;
	int optimize;
    } glCompFont;

    typedef struct {
	glcompdrawfunc_t draw;
	glcompclickfunc_t click;
	glcompdoubleclickfunc_t doubleclick;
	glcompmouseoverfunc_t mouseover;
	glcompmouseinfunc_t mousein;
	glcompmouseoutfunc_t mouseout;
	glcompmousedownfunc_t mousedown;
	glcompmouseupfunc_t mouseup;
	glcompmousedragfunct_t mousedrag;

    } glCompCallBacks;


/*
	common widget properties
	also each widget has pointer to its parents common
*/
    typedef struct {
	glCompPoint pos;
	glCompPoint refPos;	/*calculated pos after anchors and aligns */
	float width, height;
	float borderWidth;
	glCompColor color;
	int enabled;
	int visible;
	void *compset;		// compset
	void *parent;		/*parent widget */
	int data;
	glCompFont *font;	//pointer to font to use
	glCompAlignment align;
	glCompAnchor anchor;
	int layer;		/*keep track of object order, what to draw on top */
	glCompCallBacks callbacks;
	glCompCallBacks functions;
	glCompJustify justify;
    } glCompCommon;

/*generic image*/
    typedef struct {
	glObjType objType;	/*always keep this here for each drawable object */
	glCompCommon common;
	glCompTex *texture;
	float width, height;  /* width and height in world coords */
	/* char *pngFile; */
	int stretch;
    } glCompImage;

/*generic panel*/
    typedef struct {
	glObjType objType;	/*always keep this here for each drawable object */
	glCompCommon common;
	float shadowwidth;
	glCompColor shadowcolor;
	char *text;
	glCompImage *image;
    } glCompPanel;

/*label*/
    typedef struct {
	glObjType objType;	/*always keep this here for each drawable object */
	glCompCommon common;
	char *text;
	int transparent;
    } glCompLabel;

/*buttons*/
    struct _glCompButton {
	glObjType objType;	/*always keep this here for each drawable object */
	glCompCommon common;
	float width, height;
	glCompLabel *label;
	bool status; ///< false not pressed, true pressed
	glCompImage *image;	/*glyph */
	int data;

    };

/*texture based image*/

/*object prototype*/
    struct _glCompObj {
	glObjType objType;
	glCompCommon common;
    };

    typedef struct {
	glMouseButtonType t;
	glCompPoint pos; /*current mouse pos,*/
	glCompPoint GLpos;/*3d converted opengl position*/
	glCompPoint GLinitPos;/*mouse button down pos*/
	glCompPoint GLfinalPos;/*mouse button up pos*/

	float dragX, dragY;/*GLpos - GLinitpos*/
	glCompObj *clickedObj;
	glCompCallBacks callbacks;
	glCompCallBacks functions;
	int down;


    } glCompMouse;



/*main widget set manager*/
    typedef struct {
	glObjType objType;	/*always keep this here for each drawable object */
	glCompCommon common;

	glCompObj **obj;
	size_t objcnt;
	size_t textureCount;
	glCompTex **textures;
	glCompMouse mouse;
    } glCompSet;

#ifdef __cplusplus
}
#endif
