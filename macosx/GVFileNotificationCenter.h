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

@interface GVFileNotificationCenter : NSObject
{
	int _queuefd;
	NSMutableSet *_records;
}

+ (void)initialize;
+ (id)defaultCenter;

- (instancetype)init;

- (void)addObserver:(id)observer selector:(SEL)selector path:(NSString *)path;
- (void)removeObserver:(id)observer path:(NSString *)path;

@end
