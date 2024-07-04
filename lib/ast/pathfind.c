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
#include <string.h>
#include <unistd.h>

/*
 * return path to name
 */

char *pathfind(const char *name) {
  if (access(name, R_OK) >= 0)
    return strdup(name);
  return 0;
}
