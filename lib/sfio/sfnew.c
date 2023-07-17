/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include	<sfio/sfhdr.h>
#include	<stddef.h>

/*	Fundamental function to create a new stream.
**	The argument flags defines the type of stream and the scheme
**	of buffering.
**
**	Written by Kiem-Phong Vo.
*/

/**
 * @param buf a buffer to read/write, if NULL, will be allocated
 * @param size buffer size if buf is given or desired buffer size
 * @param file file descriptor to read/write from
 * @param flags type of file stream
 */
Sfio_t *sfnew(void *buf, size_t size, int file, int flags) {
    Sfio_t *f = NULL;
    int sflags;

    if (!(flags & SF_RDWR))
	return NULL;

    sflags = 0;

    if (!(flags & SF_STRING) && file >= 0 && file <= 2) {
        f = file == 0 ? sfstdin : file == 1 ? sfstdout : sfstderr;
        if (f) {
            if (f->mode & SF_AVAIL) {
                sflags = f->flags;
            } else
                f = NULL;
        }
    }

    if (!f) {
        if (!(f = malloc(sizeof(Sfio_t))))
            return NULL;
        SFCLEAR(f);
    }

    /* stream type */
    f->mode = (flags & SF_READ) ? SF_READ : SF_WRITE;
    f->flags = (unsigned short)((flags & SF_FLAGS) | (sflags & (SF_MALLOC | SF_STATIC)));
    f->bits = (flags & SF_RDWR) == SF_RDWR ? SF_BOTH : 0;
    f->file = file;
    f->here = f->extent = 0;
    f->getr = f->tiny[0] = 0;

    f->mode |= SF_INIT;
    if (size != SF_UNBOUND) {
	f->size = size;
	f->data = size <= 0 ? NULL : (uchar *) buf;
    }
    f->endb = f->endr = f->endw = f->next = f->data;

    if (f->flags & SF_STRING)
	(void) _sfmode(f, f->mode & SF_RDWR, 0);

    return f;
}
