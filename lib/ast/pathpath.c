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
 * return full path to p using $PATH
 * if a!=0 then it and $0 and $_ with $PWD are used for
 * related root searching
 * the related root must have a bin subdir
 */

#include <assert.h>
#include <ast/ast.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *getenv_path(void) {
  const char *path = getenv("PATH");
  if (path != NULL) {
    return path;
  }
  return "";
}

char **opt_info_argv;

char *pathpath(const char *p) {
    const char *a = "";
    char *s;
    const char *x;
    char path[PATH_MAX];

    static char *cmd;

    assert(p != NULL);
    struct stat st;
    if (stat(p, &st) == 0 && !S_ISDIR(st.st_mode))
	return strdup(p);
    if (*p == '/')
	a = 0;
    else if ((s = (char *) a)) {
	x = s;
	if (strchr(p, '/')) {
	    a = p;
	    p = "..";
	} else
	    a = 0;
	if ((!cmd || *cmd) &&
	    (strchr(s, '/') ||
	     (((s = cmd) || (opt_info_argv && (s = *opt_info_argv))) &&
	      strchr(s, '/') && !strchr(s, '\n') && !access(s, F_OK)) ||
	     ((s = getenv("_")) &&
	      strchr(s, '/') && !strneq(s, "/bin/", 5) &&
	      !strneq(s, "/usr/bin/", 9)) ||
	     (*x && !access(x, F_OK) && (s = getenv("PWD")) && *s == '/')
	    )
	    ) {
	    if (!cmd)
		cmd = strdup(s);
	    if (strlen(s) < sizeof(path) - 6) {
		s = strcopy(path, s);
		for (;;) {
		    do
			if (s <= path)
			    goto normal;
		    while (*--s == '/');
		    do
			if (s <= path)
			    goto normal;
		    while (*--s != '/');
		    strcpy(s + 1, "bin");
		    if (access(path, X_OK) == 0) {
			if ((s = pathaccess(path, path, p, a)))
			    return strdup(s);
			goto normal;
		    }
		}
	      normal:;
	    }
	}
    }
    x = !a && strchr(p, '/') ? "" : getenv_path();
    if (!(s = pathaccess(path, x, p, a)) && !*x
	&& (x = getenv("FPATH")))
	s = pathaccess(path, x, p, a);
    return s ? strdup(s) : s;
}
