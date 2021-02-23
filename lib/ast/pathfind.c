/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

/*
 * Glenn Fowler
 * AT&T Research
 *
 * include style search support
 */

#include "config.h"
#include <ast/ast.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include <ast/compat_unistd.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <string.h>

typedef struct Dir_s {		/* directory list element */
    struct Dir_s *next;		/* next in list                 */
    char dir[1];		/* directory path               */
} Dir_t;

static struct {			/* directory list state           */
    Dir_t *head;		/* directory list head          */
    Dir_t *tail;		/* directory list tail          */
} state;

/*
 * return path to name using pathinclude() list
 * path placed in <buf,size>
 * if lib!=0 then pathpath() attempted after include search
 * if type!=0 and name has no '.' then file.type also attempted
 * any *: prefix in lib is ignored (discipline library dictionary support)
 */

char *pathfind(const char *name, const char *lib, const char *type,
	       char *buf, size_t size)
{
    Dir_t *dp;
    char *s;
    char tmp[PATH_MAX];

    if (access(name, R_OK) >= 0)
	return strncpy(buf, name, size);
    if (type) {
	sfsprintf(buf, size, "%s.%s", name, type);
	if (access(buf, R_OK) >= 0)
	    return buf;
    }
    if (*name != '/') {
	if (strchr(name, '.'))
	    type = 0;
	for (dp = state.head; dp; dp = dp->next) {
	    sfsprintf(tmp, sizeof(tmp), "%s/%s", dp->dir, name);
	    if (pathpath(buf, tmp, "", PATH_REGULAR))
		return buf;
	    if (type) {
		sfsprintf(tmp, sizeof(tmp), "%s/%s.%s", dp->dir, name,
			  type);
		if (pathpath(buf, tmp, "", PATH_REGULAR))
		    return buf;
	    }
	}
	if (lib) {
	    if ((s = strrchr(lib, ':')))
		lib = (const char *) s + 1;
	    sfsprintf(tmp, sizeof(tmp), "lib/%s/%s", lib, name);
	    if (pathpath(buf, tmp, "", PATH_REGULAR))
		return buf;
	    if (type) {
		sfsprintf(tmp, sizeof(tmp), "lib/%s/%s.%s", lib, name,
			  type);
		if (pathpath(buf, tmp, "", PATH_REGULAR))
		    return buf;
	    }
	}
    }
    return 0;
}
