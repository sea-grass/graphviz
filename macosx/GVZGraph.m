/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at http://www.graphviz.org/
 *************************************************************************/

#import "GVZGraph.h"
#import "GVGraphArguments.h"
#import "GVGraphDefaultAttributes.h"

NSString *const GVGraphvizErrorDomain = @"GVGraphvizErrorDomain";

extern char *gvplugin_list(GVC_t * gvc, api_t api, const char *str);

extern double PSinputscale;

static GVC_t *_graphContext = nil; // initialize non-object type

@implementation GVZGraph

+ (void)initialize
{
	_graphContext = gvContext();
}

+ (NSArray *)pluginsWithAPI:(api_t)api
{
	NSMutableSet *plugins = [NSMutableSet set];

	/* go through each non-empty plugin in the list, ignoring the package part */
	char *pluginList = gvplugin_list(_graphContext, api, ":");
	char *restOfPlugins;
	char *nextPlugin;
	for (restOfPlugins = pluginList; (nextPlugin = strsep(&restOfPlugins, " "));) {
		if (*nextPlugin) {
			char *lastColon = strrchr(nextPlugin, ':');
			if (lastColon) {
				*lastColon = '\0';
				[plugins addObject:@(nextPlugin)];
			}
		}
	}
	free(pluginList);

	return [plugins.allObjects sortedArrayUsingSelector:@selector(compare:)];
}

- (instancetype)initWithURL:(NSURL *)url error:(NSError **)outError
{
	if (self = [super init]) {
		if (url.fileURL) {
			/* open a FILE* on the file URL */
			FILE *file = fopen(url.path.fileSystemRepresentation, "r");
			if (!file) {
				if (outError)
					*outError = [NSError errorWithDomain:NSPOSIXErrorDomain code:errno userInfo:nil];
				return nil;
			}
			
			_graph = agread(file, 0);
			if (!_graph) {
				if (outError)
					*outError = [NSError errorWithDomain:GVGraphvizErrorDomain code:GVFileParseError userInfo:nil];
				return nil;
			}
			if(!agget(_graph, "imagepath")) {
					const char *parentDir = url.path.fileSystemRepresentation;
					char *ptr = strrchr(parentDir,'/');
					*ptr = 0;
					agattr(_graph, AGRAPH, "imagepath", parentDir);
			}
			fclose(file);
		}
		else {
			/* read the URL into memory */
			NSMutableData *memory = [NSMutableData dataWithContentsOfURL:url options:0 error:outError];
			if (!memory) {
				return nil;
			}
			
			/* null terminate the data */
			char nullByte = '\0';
			[memory appendBytes:&nullByte length:1];
			
			_graph = agmemread((char *)memory.bytes);
			if (!_graph) {
				if (outError)
					*outError = [NSError errorWithDomain:GVGraphvizErrorDomain code:GVFileParseError userInfo:nil];
				return nil;
			}
		}

		_freeLastLayout = NO;
		_arguments = [[GVGraphArguments alloc] initWithGraph:self];
		_graphAttributes = [[GVGraphDefaultAttributes alloc] initWithGraph:self prototype:AGRAPH];
		_defaultNodeAttributes = [[GVGraphDefaultAttributes alloc] initWithGraph:self prototype:AGNODE];
		_defaultEdgeAttributes = [[GVGraphDefaultAttributes alloc] initWithGraph:self prototype:AGEDGE];
	}

	return self;
}

- (BOOL)writeToURL:(NSURL *)url error:(NSError **)outError
{
	if (url.fileURL) {
		/* open a FILE* on the file URL */
		FILE *file = fopen(url.path.fileSystemRepresentation, "w");
		if (!file) {
			if (outError)
				*outError = [NSError errorWithDomain:NSPOSIXErrorDomain code:errno userInfo:nil];
			return NO;
		}
		
		/* write it out */
		if (agwrite(_graph, file) != 0) {
			if (outError)
				*outError = [NSError errorWithDomain:NSPOSIXErrorDomain code:errno userInfo:nil];
			return NO;
		}
		
		fclose(file);
		return YES;
	}
	else
		/* can't write out to non-file URL */
		return NO;
}

- (void)noteChanged:(BOOL)relayout
{
	/* if we need to layout, apply globals and then relayout */
	if (relayout) {
		NSString *layout = _arguments[@"layout"];
		if (layout) {
			if (_freeLastLayout)
				gvFreeLayout(_graphContext, _graph);
				
			/* apply scale */
			NSString *scale = _arguments[@"scale"];
			PSinputscale = scale ? scale.doubleValue : 0.0;
			if (PSinputscale == 0.0)
				PSinputscale = 72.0;
		
			if (gvLayout(_graphContext, _graph, (char *)layout.UTF8String) != 0)
				@throw [NSException exceptionWithName:@"GVException" reason:@"bad layout" userInfo:nil];
			_freeLastLayout = YES;
		}
	}

	[[NSNotificationCenter defaultCenter] postNotificationName: @"GVGraphDidChange" object:self];
}

- (NSData *)renderWithFormat:(NSString *)format
{
	char *renderedData = NULL;
	unsigned int renderedLength = 0;
	if (gvRenderData(_graphContext, _graph, (char *)format.UTF8String, &renderedData, &renderedLength) != 0)
		@throw [NSException exceptionWithName:@"GVException" reason:@"bad render" userInfo:nil];
	return [NSData dataWithBytesNoCopy:renderedData length:renderedLength freeWhenDone:YES];
}

- (void)renderWithFormat:(NSString *)format toURL:(NSURL *)url
{
	if (url.fileURL) {
		if (gvRenderFilename(_graphContext, _graph, (char *)format.UTF8String, (char *)url.path.UTF8String) != 0)
			@throw [NSException exceptionWithName:@"GVException" reason:@"bad render" userInfo:nil];
	}
	else
		[[self renderWithFormat:format] writeToURL:url atomically:NO];
}

- (void)dealloc
{
	if (_graph)
		agclose(_graph);
	_graph = nil; // reinitialize non-object type
}

@end
