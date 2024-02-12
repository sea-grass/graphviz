/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

@import Foundation;

@class GVZGraph;

@interface GVGraphArguments : NSMutableDictionary
{
	__weak GVZGraph *_graph;
	NSMutableDictionary *_arguments;
}

- (instancetype)initWithGraph:(GVZGraph *)graph;

/* dictionary primitive methods */
- (NSUInteger)count;
- (NSEnumerator *)keyEnumerator;
- (id)objectForKey:(id)key;

/* mutable dictionary primitive methods */
- (void)setObject:(id)object forKey:(id)key;
- (void)removeObjectForKey:(id)key;

@end
