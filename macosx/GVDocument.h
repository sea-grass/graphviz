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

@class GVExportViewController;
@class GVZGraph;

@interface GVDocument : NSDocument
{
	GVExportViewController *_exporter;
}

@property(readonly) GVZGraph *graph;

- (instancetype)init;

- (BOOL)readFromURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError;
- (BOOL)writeToURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError;

- (void)makeWindowControllers;

- (void)setPrintInfo:(NSPrintInfo *)printInfo;

- (IBAction)exportDocument:(id)sender;
- (void)exporterDidEnd;

- (void)fileDidChange:(NSString *)path;
- (void)graphDidChange:(NSNotification *)notification;

- (void)dealloc;

@end
