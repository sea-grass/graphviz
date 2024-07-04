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
 * include style search support
 */

#include <ast/ast.h>
#include <cgraph/agxbuf.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * return path to name
 * if lib!=0 then pathpath() attempted after include search
 * if type!=0 and name has no '.' then file.type also attempted
 * any *: prefix in lib is ignored (discipline library dictionary support)
 */

char *pathfind(const char *name, const char *lib, const char *type) {
  char *s;
  agxbuf tmp = {0};
  char *buf;

  if (access(name, R_OK) >= 0)
    return strdup(name);
  if (type) {
    agxbprint(&tmp, "%s.%s", name, type);
    char *tmp_path = agxbdisown(&tmp);
    if (access(tmp_path, R_OK) >= 0)
      return tmp_path;
    free(tmp_path);
  }
  if (*name != '/') {
    if (strchr(name, '.'))
      type = 0;
    if (lib) {
      if ((s = strrchr(lib, ':')))
        lib = s + 1;
      agxbprint(&tmp, "lib/%s/%s", lib, name);
      if ((buf = pathpath(agxbuse(&tmp)))) {
        agxbfree(&tmp);
        return buf;
      }
      if (type) {
        agxbprint(&tmp, "lib/%s/%s.%s", lib, name, type);
        if ((buf = pathpath(agxbuse(&tmp)))) {
          agxbfree(&tmp);
          return buf;
        }
      }
    }
  }
  agxbfree(&tmp);
  return 0;
}
