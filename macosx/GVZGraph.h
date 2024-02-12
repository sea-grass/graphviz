/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

#include "gvc.h"

@import Foundation;

#import "GVGraphDefaultAttributes.h"

@class GVGraphArguments;

@interface GVZGraph : NSObject
{
	BOOL _freeLastLayout;
}

@property(readonly) graph_t *graph;
@property(readonly) GVGraphArguments *arguments;
@property(readonly) GVGraphDefaultAttributes *graphAttributes;
@property(readonly) GVGraphDefaultAttributes *defaultNodeAttributes;
@property(readonly) GVGraphDefaultAttributes *defaultEdgeAttributes;

+ (void)initialize;
+ (NSArray *)pluginsWithAPI:(api_t)api;

- (instancetype)initWithURL:(NSURL *)url error:(NSError **)outError;

- (NSData *)renderWithFormat:(NSString *)format;
- (void)renderWithFormat:(NSString *)format toURL:(NSURL *)url;
- (void)noteChanged:(BOOL)relayout;

- (BOOL)writeToURL:(NSURL *)url error:(NSError **)outError;

- (void)dealloc;

@end

extern NSString *const GVGraphvizErrorDomain;

enum {
	GVNoError,
	GVFileParseError
};
