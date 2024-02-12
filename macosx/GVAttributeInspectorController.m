/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

#import "GVAttributeInspectorController.h"
#import "GVAttributeSchema.h"
#import "GVDocument.h"
#import "GVZGraph.h"
#import "GVWindowController.h"

@implementation GVAttributeInspectorController

- (instancetype)init
{
	if (self = [super initWithWindowNibName: @"Attributes"]) {
		_allAttributes = [[NSMutableDictionary alloc] init];
		_otherChangedGraph = YES;
	}
	return self;
}

- (void)awakeFromNib
{
	/* set component toolbar */
	_allSchemas = [[NSDictionary alloc] initWithObjectsAndKeys:
		[GVAttributeSchema attributeSchemasWithComponent:@"graph"], _graphToolbarItem.itemIdentifier,
		[GVAttributeSchema attributeSchemasWithComponent:@"node"], _nodeDefaultToolbarItem.itemIdentifier,
		[GVAttributeSchema attributeSchemasWithComponent:@"edge"], _edgeDefaultToolbarItem.itemIdentifier,
		nil];
	_componentToolbar.selectedItemIdentifier = _graphToolbarItem.itemIdentifier;
	[self toolbarItemDidSelect:nil];

	/* start observing whenever a window becomes main */
	[self graphWindowDidBecomeMain:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(graphWindowDidBecomeMain:) name:NSWindowDidBecomeMainNotification object:nil];
}

- (IBAction)toolbarItemDidSelect:(id)sender
{
	/* reload the table */
	[_attributeTable reloadData];
}

- (void)graphWindowDidBecomeMain:(NSNotification *)notification
{
	NSWindow *mainWindow = notification ? notification.object : NSApp.mainWindow;
	GVDocument *mainWindowDocument = mainWindow.windowController.document;

	/* update and observe referenced document */
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	if (_inspectedDocument)
		[defaultCenter removeObserver:self name:@"GVGraphDocumentDidChange" object:_inspectedDocument];
	_inspectedDocument = mainWindowDocument;
	[defaultCenter addObserver:self selector:@selector(graphDocumentDidChange:) name:@"GVGraphDocumentDidChange" object:mainWindowDocument];

	[self reloadAttributes];
		
	/* update the UI */
	self.window.title = [NSString stringWithFormat:@"%@ Attributes", mainWindow.title];
	[_attributeTable reloadData];
}

- (void)graphDocumentDidChange:(NSNotification *)notification
{
	/* if we didn't instigate the change, update the UI */
	if (_otherChangedGraph) {
		[self reloadAttributes];
		[_attributeTable reloadData];
	}
}

- (void)reloadAttributes
{
	/* reload the attributes from the inspected document's graph */
	[_allAttributes removeAllObjects];
	if ([_inspectedDocument respondsToSelector:@selector(graph)]) {
		GVZGraph *graph = _inspectedDocument.graph;
		_allAttributes[_graphToolbarItem.itemIdentifier] = graph.graphAttributes;
		_allAttributes[_nodeDefaultToolbarItem.itemIdentifier] = graph.defaultNodeAttributes;
		_allAttributes[_edgeDefaultToolbarItem.itemIdentifier] = graph.defaultEdgeAttributes;
	}
}

- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar
{
	/* which toolbar items are selectable */
	return @[_graphToolbarItem.itemIdentifier,
		_nodeDefaultToolbarItem.itemIdentifier,
		_edgeDefaultToolbarItem.itemIdentifier];
}

- (NSCell *)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	if ([tableColumn.identifier isEqualToString:@"value"]) {
		/* use the row's schema's cell */
		NSCell *cell = _allSchemas[_componentToolbar.selectedItemIdentifier][row].cell;
		cell.enabled = _allAttributes.count > 0;
		return cell;
	}
	else
		/* use the default cell (usually a text field) for other columns */
		return nil;
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	NSInteger selectedRow = [notification.object selectedRow];
	NSString *documentation = selectedRow == -1 ? nil : _allSchemas[_componentToolbar.selectedItemIdentifier][selectedRow].documentation;
	[_documentationWeb loadHTMLString:documentation baseURL:[NSURL URLWithString:@"http://www.graphviz.org/"]];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
	return _allSchemas[_componentToolbar.selectedItemIdentifier].count;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	NSString *selectedComponentIdentifier = _componentToolbar.selectedItemIdentifier;
	NSString *attributeName = _allSchemas[selectedComponentIdentifier][row].name;
	if ([tableColumn.identifier isEqualToString:@"key"])
		return attributeName;
	else if ([tableColumn.identifier isEqualToString:@"value"])
		/* return the inspected graph's attribute value, if any */
		return _allAttributes[selectedComponentIdentifier][attributeName];
	else
		return nil;
}

- (void)tableView:(NSTableView *)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	if ([tableColumn.identifier isEqualToString:@"value"]) {
		NSString *selectedComponentIdentifier = _componentToolbar.selectedItemIdentifier;
		NSString *attributeName = _allSchemas[selectedComponentIdentifier][row].name;

		/* set or remove the key-value on the selected attributes */
		/* NOTE: to avoid needlessly reloading the table in graphDocumentDidChange:, we fence this change with _otherChangedGraph = NO */
		_otherChangedGraph = NO;
		@try {
			_allAttributes[selectedComponentIdentifier][attributeName] = object;
		}
		@finally {
			_otherChangedGraph = YES;
		}
	}
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSWindowDidBecomeMainNotification object:nil];
}

@end
