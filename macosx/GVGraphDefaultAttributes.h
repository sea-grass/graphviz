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

@class GVZGraph;

@interface GVGraphDefaultAttributes : NSMutableDictionary<NSString *, NSString *>
{
	__weak GVZGraph *_graph;
	int _kind;
}

- (instancetype)initWithGraph:(GVZGraph *)graph prototype:(int)kind;

/* dictionary primitive methods */
- (NSUInteger)count;
- (NSEnumerator *)keyEnumerator;
- (NSString *)objectForKey:(NSString *)key;

/* mutable dictionary primitive methods */
- (void)setObject:(NSString *)object forKey:(NSString *)key;
- (void)removeObjectForKey:(NSString *)key;

@end
