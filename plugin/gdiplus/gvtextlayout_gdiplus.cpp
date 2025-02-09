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

#include <memory>
#include <stdlib.h>
#include <string.h>

#include <gvc/gvplugin_textlayout.h>
#include "gvplugin_gdiplus.h"

using namespace Gdiplus;

static int CALLBACK fetch_first_font(const LOGFONTA *logFont,
                                     const TEXTMETRICA *, DWORD, LPARAM lParam)
{
	/* save the first font we see in the font enumeration */
	*((LOGFONTA *)lParam) = *logFont;
	return 0;
}

Layout::Layout(char *fontname, double fontsize, char* string)
{
	/* convert incoming UTF8 string to wide chars */
	/* NOTE: conversion is 1 or more UTF8 chars to 1 wide char */
	int len = strlen(string);
	text.resize(len);
	text.resize(MultiByteToWideChar(CP_UTF8, 0, string, len, &text[0], len));

	/* search for a font with this name. if we can't find it, use the generic serif instead */
	/* NOTE: GDI font search is more comprehensive than GDI+ and will look for variants e.g. Arial Bold */
	DeviceContext reference;
	LOGFONTA font_to_find;
	font_to_find.lfCharSet = ANSI_CHARSET;
	strncpy(font_to_find.lfFaceName, fontname, sizeof(font_to_find.lfFaceName) - 1);
	font_to_find.lfFaceName[sizeof(font_to_find.lfFaceName) - 1] = '\0';
	font_to_find.lfPitchAndFamily = 0;
	LOGFONTA found_font;
	if (EnumFontFamiliesExA(reference.hdc,
		&font_to_find,
		fetch_first_font,
		(LPARAM)&found_font,
		0) == 0) {
		found_font.lfHeight = (LONG)-fontsize;
		found_font.lfWidth = 0;
		font = std::make_unique<Font>(reference.hdc, &found_font);
	}
	else
		font = std::make_unique<Font>(FontFamily::GenericSerif(), fontsize);
}

void gdiplus_free_layout(void *layout)
{
	if (layout)
		delete reinterpret_cast<Layout*>(layout);
};

bool gdiplus_textlayout(textspan_t *span, char **)
{
	/* ensure GDI+ is started up: since we get called outside of a job, we can't rely on GDI+ startup then */
	UseGdiplus();

	Layout* layout = new Layout(span->font->name, span->font->size, span->str);

	/* measure the text */
	/* NOTE: use TextRenderingHintAntiAlias + GetGenericTypographic to get a layout without extra space at beginning and end */
	RectF boundingBox;
	DeviceContext deviceContext;
	Graphics measureGraphics(deviceContext.hdc);
	measureGraphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
	measureGraphics.MeasureString(
		&layout->text[0],
		layout->text.size(),
		layout->font.get(),
		PointF(0.0f, 0.0f),
		GetGenericTypographic(),
		&boundingBox);

	FontFamily fontFamily;
	layout->font->GetFamily(&fontFamily);
	int style = layout->font->GetStyle();

	span->layout = layout;
	span->free_layout = &gdiplus_free_layout;
	span->size.x = boundingBox.Width;
	span->size.y = layout->font->GetHeight(&measureGraphics);
	span->yoffset_layout = fontFamily.GetCellAscent(style) * span->font->size / fontFamily.GetEmHeight(style); /* convert design units to pixels */
	span->yoffset_centerline = 0;
	return true;
};

static gvtextlayout_engine_t gdiplus_textlayout_engine = {
    gdiplus_textlayout
};

gvplugin_installed_t gvtextlayout_gdiplus_types[] = {
    {0, "textlayout", 8, &gdiplus_textlayout_engine, nullptr},
    {0, nullptr, 0, nullptr, nullptr}
};
