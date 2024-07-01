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
 * return string with expanded escape chars
 */

#include <ast/ast.h>
#include <cgraph/gv_ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/// quote string as with qb...qe
char *fmtquote(const char *as, const char *qb, const char *qe) {
    const size_t n = strlen(as);
    const unsigned char *s = (const unsigned char *) as;
    const unsigned char *e = s + n;
    char *b;
    bool escaped = false;
    bool spaced = false;
    bool shell = false;
    char *f;
    char *buf;

    size_t len = 4 * (n + 1);
    if (qb)
	len += strlen(qb);
    if (qe)
	len += strlen(qe);
    b = buf = fmtbuf(len);
    if (qb) {
	if (qb[0] == '$' && qb[1] == '\'' && qb[2] == 0)
	    shell = true;
	while ((*b = *qb++))
	    b++;
    }
    f = b;
    while (s < e) {
	    int c = *s++;
	    if (gv_iscntrl(c) || !gv_isprint(c) || c == '\\') {
		escaped = true;
		*b++ = '\\';
		switch (c) {
		case CC_bel:
		    c = 'a';
		    break;
		case '\b':
		    c = 'b';
		    break;
		case '\f':
		    c = 'f';
		    break;
		case '\n':
		    c = 'n';
		    break;
		case '\r':
		    c = 'r';
		    break;
		case '\t':
		    c = 't';
		    break;
		case CC_vt:
		    c = 'v';
		    break;
		case CC_esc:
		    c = 'E';
		    break;
		case '\\':
		    break;
		default:
		    *b++ = (char)('0' + ((c >> 6) & 07));
		    *b++ = (char)('0' + ((c >> 3) & 07));
		    c = '0' + (c & 07);
		    break;
		}
	    } else if (qe && strchr(qe, c)) {
		escaped = true;
		*b++ = '\\';
	    } else if (!spaced &&
		       !escaped &&
		       (gv_isspace(c) ||
			(shell &&
			 (strchr("\";~&|()<>[]*?", c) ||
			  (c == '#' && (b == f || gv_isspace(*(b - 1)))
			  )
			 )
			)
		       )
		)
		spaced = true;
	    *b++ = (char)c;
    }
    if (qb) {
	if (!escaped)
	    buf += shell + !spaced;
	if (qe && (escaped || spaced))
	    while ((*b = *qe++))
		b++;
    }
    *b = 0;
    return buf;
}

/*
 * escape the usual suspects and quote chars in qs
 */

char *fmtesq(const char *as, const char *qs)
{
  return fmtquote(as, NULL, qs);
}

/*
 * escape the usual suspects
 */

char *fmtesc(const char *as)
{
  return fmtquote(as, NULL, NULL);
}
