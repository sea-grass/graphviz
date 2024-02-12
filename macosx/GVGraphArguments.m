/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

#import "GVGraphArguments.h"
#import "GVZGraph.h"

@implementation GVGraphArguments

- (instancetype)initWithGraph:(GVZGraph *)graph
{
	if (self = [super init]) {
		_graph = graph;
		_arguments = [[NSMutableDictionary alloc] init];
	}
	return self;
}

- (NSUInteger)count
{
	return _arguments.count;
}

- (NSEnumerator *)keyEnumerator
{
	return [_arguments keyEnumerator];
}

- (id)objectForKey:(id)key
{
	return _arguments[key];
}

/* mutable dictionary primitive methods */
- (void)setObject:(id)object forKey:(id)key
{
	_arguments[key] = object;
	[_graph noteChanged:YES];
}

- (void)removeObjectForKey:(id)key
{
	[_arguments removeObjectForKey:key];
	[_graph noteChanged:YES];
}

@end
