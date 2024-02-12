/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

@import WebKit;

#import "GVGraphDefaultAttributes.h"

@class GVAttributeSchema;
@class GVDocument;

@interface GVAttributeInspectorController : NSWindowController
{
	NSDictionary<NSToolbarItemIdentifier, NSArray<GVAttributeSchema *> *> *_allSchemas;
	NSMutableDictionary<NSToolbarItemIdentifier, GVGraphDefaultAttributes *> *_allAttributes;
	GVDocument *_inspectedDocument;
	BOOL _otherChangedGraph;
}

@property(readwrite) IBOutlet NSToolbar *componentToolbar;
@property(readwrite) IBOutlet NSToolbarItem *graphToolbarItem;
@property(readwrite) IBOutlet NSToolbarItem *nodeDefaultToolbarItem;
@property(readwrite) IBOutlet NSToolbarItem *edgeDefaultToolbarItem;

@property(readwrite) IBOutlet NSTableView *attributeTable;
@property(readwrite) IBOutlet WKWebView *documentationWeb;

- (instancetype)init;

- (void)awakeFromNib;

/* notifications */
- (IBAction)toolbarItemDidSelect:(id)sender;
- (void)graphWindowDidBecomeMain:(NSNotification *)notification;
- (void)graphDocumentDidChange:(NSNotification *)notification;
- (void)reloadAttributes;

/* toolbar delegate methods */
- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar;

/* table delegate methods */
- (NSCell *)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row;
- (void)tableViewSelectionDidChange:(NSNotification *)notification;

/* table data source methods */
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView;
- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row;
- (void)tableView:(NSTableView *)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row;

- (void)dealloc;

@end
