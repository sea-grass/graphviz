/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

#import "GVGraphDefaultAttributes.h"
#import "GVZGraph.h"

@interface GVGraphDefaultAttributeKeyEnumerator : NSEnumerator
{
	graph_t *_graph;
	int _kind;
	Agsym_t *_nextSymbol;
}

@property(readonly, copy) NSArray *allObjects;
@property(readonly) id nextObject;

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithGraphLoc:(graph_t *)graph prototype:(int)kind NS_DESIGNATED_INITIALIZER;

@end

@implementation GVGraphDefaultAttributeKeyEnumerator

- (instancetype)initWithGraphLoc:(graph_t *)graph prototype:(int)kind;
{
	if (self = [super init]) {
		_graph = graph;
		_kind = kind;
		_nextSymbol = agnxtattr(_graph, _kind, NULL);
	}
	return self;
}

- (NSArray *)allObjects
{
	NSMutableArray *all = [NSMutableArray array];
	for (; _nextSymbol; _nextSymbol = agnxtattr(_graph, _kind, _nextSymbol)) {
		char *attributeValue = _nextSymbol->defval;
		if (attributeValue && *attributeValue)
			[all addObject:@(attributeValue)];
	}

	return all;
}

- (id)nextObject
{
	for (; _nextSymbol; _nextSymbol = agnxtattr(_graph, _kind, _nextSymbol)) {
		char *attributeValue = _nextSymbol->defval;
		if (attributeValue && *attributeValue)
			return @(attributeValue);
	}
	return nil;
}

@end

@implementation GVGraphDefaultAttributes

- (instancetype)initWithGraph:(GVZGraph *)graph prototype:(int)kind
{
	if (self = [super init]) {
		_graph = graph;
		_kind = kind;
	}
	return self;
}

- (NSUInteger)count
{
	NSUInteger symbolCount = 0;
	Agsym_t *nextSymbol = NULL;
	for (nextSymbol = agnxtattr(_graph.graph, _kind, nextSymbol); nextSymbol; nextSymbol = agnxtattr(_graph.graph, _kind, nextSymbol))
		if (nextSymbol->defval && *(nextSymbol->defval))
			++symbolCount;
	return symbolCount;
}

- (NSEnumerator *)keyEnumerator
{
	return [[GVGraphDefaultAttributeKeyEnumerator alloc] initWithGraphLoc:_graph.graph prototype:_kind];
}

- (NSString *)objectForKey:(NSString *)key
{
	NSString *object;
	Agsym_t *attributeSymbol = agattr(_graph.graph, _kind, (char *)key.UTF8String, 0);
	if (attributeSymbol) {
		char *attributeValue = attributeSymbol->defval;
		if (attributeValue && *attributeValue)
			object = @(attributeValue);
	}
	return object;
}

- (void)setObject:(NSString *)object forKey:(NSString *)key
{
	agattr(_graph.graph, _kind, (char *)key.UTF8String, (char *)object.UTF8String);
	[_graph noteChanged:YES];
}

- (void)removeObjectForKey:(NSString *)key
{
	agattr(_graph.graph, _kind, (char *)key.UTF8String, "");
	[_graph noteChanged:YES];
}
@end
