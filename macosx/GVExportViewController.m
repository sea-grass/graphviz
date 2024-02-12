/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

#import "GVExportViewController.h"
#import "GVZGraph.h"

static NSMutableArray *_formatRenders;

@implementation GVExportViewController

@synthesize URL = _url;

+ (void)initialize
{
	if (!_formatRenders) {
		_formatRenders = [[NSMutableArray alloc] init];
		
		NSString *lastFormat;
		NSMutableArray *lastRenders;
		for (NSString *device in [GVZGraph pluginsWithAPI:API_device]) {
			NSArray *deviceComponents = [device componentsSeparatedByString:@":"];
			NSUInteger componentCount = deviceComponents.count;
			
			if (componentCount > 0) {
				NSString *format = deviceComponents[0];
				if (![lastFormat isEqualToString:format]) {
					lastFormat = format;
					lastRenders = [NSMutableArray array];
					[_formatRenders addObject:@{@"format": lastFormat, @"renders": lastRenders}];
				}
			}
			
			if (componentCount > 1)
				[lastRenders addObject:deviceComponents[1]];
		}
	}
}

- (instancetype)init
{
	if (self = [super initWithNibName:@"Export" bundle:nil]) {
		for (NSDictionary *formatRender in _formatRenders)
			if ([formatRender[@"format"] isEqualToString:@"pdf"]) {
				_formatRender = formatRender;
				if ([formatRender[@"renders"] containsObject:@"quartz"])
					_render = @"quartz";
				break;
			}
	}
	return self;
}

- (NSArray *)formatRenders
{
	return _formatRenders;
}

- (NSString *)device
{
	NSString *format = _formatRender[@"format"];
	return _render ? [NSString stringWithFormat:@"%@:%@", format, _render] : format;
}

- (NSDictionary *)formatRender
{
	return _formatRender;
}

- (void)setFormatRender:(NSDictionary *)formatRender
{
	if (_formatRender != formatRender) {
		_formatRender = formatRender;
		
		/* force save panel to use this file type */
		UTType *type = [UTType typeWithFilenameExtension:_formatRender[@"format"]];
		_panel.allowedContentTypes = @[type];

		/* remove existing render if it's not compatible with format */
		if (![_formatRender[@"renders"] containsObject:_render])
			_render = nil;
	}
}

- (void)beginSheetModalForWindow:(NSWindow *)window modalDelegate:(id)modalDelegate didEndSelector:(SEL)selector
{
	/* remember to invoke end selector on the modal delegate */
	NSInvocation *endInvocation = [NSInvocation invocationWithMethodSignature:[modalDelegate methodSignatureForSelector:selector]];
	endInvocation.target = modalDelegate;
	endInvocation.selector = selector;

	_panel = [NSSavePanel savePanel];
	_panel.accessoryView = self.view;
	UTType *type = [UTType typeWithFilenameExtension:_formatRender[@"format"]];
	_panel.allowedContentTypes = @[type];
	_panel.directoryURL = _url.URLByDeletingLastPathComponent;
	_panel.nameFieldStringValue = _url.lastPathComponent;
	__weak typeof(self) weakSelf = self;
	[_panel beginSheetModalForWindow:window completionHandler:^(NSInteger result) {
		if (result == NSModalResponseOK) {
			weakSelf.URL = weakSelf.panel.URL;
			/* invoke the end selector on the modal delegate */
			[endInvocation invoke];
		}
	}];
}

@end
