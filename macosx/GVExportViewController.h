/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

@import AppKit;
@import UniformTypeIdentifiers;

@interface GVExportViewController : NSViewController
{
	NSDictionary *_formatRender;
	NSString *_render;
}

@property(readonly) NSSavePanel *panel;
@property(readonly) NSString *device;
@property(readwrite) NSURL *URL;

- (instancetype)init;

- (void)beginSheetModalForWindow:(NSWindow *)window modalDelegate:(id)modalDelegate didEndSelector:(SEL)selector;

@end
