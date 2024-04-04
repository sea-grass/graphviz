/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
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
#include <cgraph/agxbuf.h>
#include <cgraph/startswith.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const char *getenv_path(void) {
  const char *path = getenv("PATH");
  if (path != NULL) {
    return path;
  }
  return "";
}

char *pathpath(const char *p) {
  const char *a = "";
  char *s;
  const char *x;

  static char *cmd;

  assert(p != NULL);
  struct stat st;
  if (stat(p, &st) == 0 && !S_ISDIR(st.st_mode))
    return strdup(p);
  if (*p == '/')
    a = 0;
  else if ((s = (char *)a)) {
    x = s;
    if (strchr(p, '/')) {
      a = p;
      p = "..";
    } else
      a = 0;
    if ((!cmd || *cmd) &&
        (strchr(s, '/') ||
         ((s = cmd) && strchr(s, '/') && !strchr(s, '\n') &&
          !access(s, F_OK)) ||
         ((s = getenv("_")) && strchr(s, '/') && !startswith(s, "/bin/") &&
          !startswith(s, "/usr/bin/")) ||
         (*x && !access(x, F_OK) && (s = getenv("PWD")) && *s == '/'))) {
      if (!cmd)
        cmd = strdup(s);
      const char *end = s + strlen(s);
      for (;;) {
        do
          if (end <= s)
            goto normal;
        while (*--end == '/');
        do
          if (end <= s)
            goto normal;
        while (*--end != '/');
        agxbuf path = {0};
        agxbprint(&path, "%.*sbin", (int)(end - s + 1), s);
        const char *path_str = agxbuse(&path);
        if (access(path_str, X_OK) == 0) {
          if ((s = pathaccess(path_str, p, a))) {
            agxbfree(&path);
            return s;
          }
          agxbfree(&path);
          goto normal;
        }
        agxbfree(&path);
      }
    normal:;
    }
  }
  x = !a && strchr(p, '/') ? "" : getenv_path();
  if (!(s = pathaccess(x, p, a)) && !*x && (x = getenv("FPATH")))
    s = pathaccess(x, p, a);
  return s;
}
