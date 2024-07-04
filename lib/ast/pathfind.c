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
#include <string.h>
#include <unistd.h>

/*
 * return path to name
 * if type!=0 and name has no '.' then file.type also attempted
 */

char *pathfind(const char *name, const char *type) {
  agxbuf tmp = {0};

  if (access(name, R_OK) >= 0)
    return strdup(name);
  if (type) {
    agxbprint(&tmp, "%s.%s", name, type);
    char *tmp_path = agxbdisown(&tmp);
    if (access(tmp_path, R_OK) >= 0)
      return tmp_path;
    free(tmp_path);
  }
  agxbfree(&tmp);
  return 0;
}
