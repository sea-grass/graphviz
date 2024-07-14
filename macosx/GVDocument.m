/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

#import "GVDocument.h"
#import "GVExportViewController.h"
#import "GVFileNotificationCenter.h"
#import "GVZGraph.h"
#import "GVWindowController.h"
#import "GVGraphArguments.h"

@implementation GVDocument

- (instancetype)init
{
	if (self = [super init]) {
	}
    return self;
}

- (BOOL)readFromURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError
{
	_graph = [[GVZGraph alloc] initWithURL:url error:outError];
	if (*outError) {
		return NO;
	}
	_graph.arguments[@"layout"] = @"dot";

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(graphDidChange:) name:@"GVGraphDidChange" object:_graph];
	[[GVFileNotificationCenter defaultCenter] addObserver:self selector:@selector(fileDidChange:) path:url.path];

	return YES;
}

- (BOOL)writeToURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError **)outError
{
	return [_graph writeToURL:url error:outError];
}

- (void)makeWindowControllers
{
	[self addWindowController:[[GVWindowController alloc] init]];
}

- (void)setPrintInfo:(NSPrintInfo *)printInfo
{
	/* after Page Setup is run, change the page size and margins of the graph to fit the page setup parameters */
	super.printInfo = printInfo;
	NSSize paperSize = printInfo.paperSize;
	NSRect imageablePageBounds = printInfo.imageablePageBounds;
	double scalingFactor = 72.0 * [[printInfo dictionary][NSPrintScalingFactor] doubleValue];

	_graph.graphAttributes[@"page"] = [NSString stringWithFormat:@"%f,%f", paperSize.width / scalingFactor, paperSize.height / scalingFactor];
	_graph.graphAttributes[@"margin"] = [NSString stringWithFormat:@"%f,%f", fmax(imageablePageBounds.origin.x, paperSize.width - imageablePageBounds.size.width - imageablePageBounds.origin.x) / scalingFactor, fmax(imageablePageBounds.origin.y, paperSize.height - imageablePageBounds.size.height - imageablePageBounds.origin.y) / scalingFactor];
}

- (IBAction)exportDocument:(id)sender
{
	if (!_exporter) {
		_exporter = [[GVExportViewController alloc] init];
		_exporter.URL = self.fileURL.URLByDeletingPathExtension;
	}
	[_exporter beginSheetModalForWindow:self.windowForSheet modalDelegate:self didEndSelector:@selector(exporterDidEnd)];
}

- (void)exporterDidEnd
{
	[_graph renderWithFormat:_exporter.device toURL:_exporter.URL];
}

- (void)fileDidChange:(NSString *)path
{
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter removeObserver:self name:@"GVGraphDidChange" object:_graph];

	/* reparse the graph fresh from the file */
	NSError *error;
	_graph = [[GVZGraph alloc] initWithURL:self.fileURL error:&error];
	if (error) {
		return;
	}
	_graph.arguments[@"layout"] = @"dot";

	[defaultCenter addObserver:self selector:@selector(graphDidChange:) name:@"GVGraphDidChange" object:_graph];
	[defaultCenter postNotificationName:@"GVGraphDocumentDidChange" object:self];
}

- (void)graphDidChange:(NSNotification *)notification
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"GVGraphDocumentDidChange" object:self];
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"GVGraphDidChange" object:_graph];
	[[GVFileNotificationCenter defaultCenter] removeObserver:self path:self.fileURL.path];
}

@end
