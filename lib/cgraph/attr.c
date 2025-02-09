/**
 * @file
 * @brief implementation of dynamic attributes
 * @ingroup cgraph_attr
 * @ingroup cgraph_core
 */
 /*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include	<cgraph/cghdr.h>
#include	<stddef.h>
#include	<stdbool.h>
#include	<util/streq.h>
#include	<util/unreachable.h>

/*
 * dynamic attributes
 */

/* to create a graph's data dictionary */

#define MINATTR	4		/* minimum allocation */

static void freesym(void *obj);

Dtdisc_t AgDataDictDisc = {
    (int) offsetof(Agsym_t, name),	/* use symbol name as key */
    -1,
    (int) offsetof(Agsym_t, link),
    NULL,
    freesym,
    NULL,
};

static char DataDictName[] = "_AG_datadict";
static void init_all_attrs(Agraph_t * g);
static Agdesc_t ProtoDesc = {.directed = true, .no_loop = true,
                             .no_write = true};
static Agraph_t *ProtoGraph;

Agdatadict_t *agdatadict(Agraph_t *g, bool cflag) {
    Agdatadict_t *rv = (Agdatadict_t *) aggetrec(g, DataDictName, 0);
    if (rv || !cflag)
	return rv;
    init_all_attrs(g);
    rv = (Agdatadict_t *) aggetrec(g, DataDictName, 0);
    return rv;
}

static Dict_t *agdictof(Agraph_t * g, int kind)
{
    Agdatadict_t *dd;
    Dict_t *dict;

    dd = agdatadict(g, false);
    if (dd)
	switch (kind) {
	case AGRAPH:
	    dict = dd->dict.g;
	    break;
	case AGNODE:
	    dict = dd->dict.n;
	    break;
	case AGINEDGE:
	case AGOUTEDGE:
	    dict = dd->dict.e;
	    break;
	default:
	    agerrorf("agdictof: unknown kind %d\n", kind);
	    dict = NULL;
	    break;
    } else
	dict = NULL;
    return dict;
}

static Agsym_t *agnewsym(Agraph_t * g, const char *name, const char *value,
                         int id, int kind) {
    Agsym_t *sym;
    sym = agalloc(g, sizeof(Agsym_t));
    sym->kind = (unsigned char) kind;
    sym->name = agstrdup(g, name);
    sym->defval = agstrdup(g, value);
    sym->id = id;
    return sym;
}

static void agcopydict(Dict_t * src, Dict_t * dest, Agraph_t * g, int kind)
{
    Agsym_t *sym, *newsym;

    assert(dtsize(dest) == 0);
    for (sym = dtfirst(src); sym; sym = dtnext(src, sym)) {
	newsym = agnewsym(g, sym->name, sym->defval, sym->id, kind);
	newsym->print = sym->print;
	newsym->fixed = sym->fixed;
	dtinsert(dest, newsym);
    }
}

static Agdatadict_t *agmakedatadict(Agraph_t * g)
{
    Agraph_t *par;
    Agdatadict_t *parent_dd, *dd;

    dd = agbindrec(g, DataDictName, sizeof(Agdatadict_t), false);
    dd->dict.n = agdtopen(g, &AgDataDictDisc, Dttree);
    dd->dict.e = agdtopen(g, &AgDataDictDisc, Dttree);
    dd->dict.g = agdtopen(g, &AgDataDictDisc, Dttree);
    if ((par = agparent(g))) {
	parent_dd = agdatadict(par, false);
	assert(dd != parent_dd);
	dtview(dd->dict.n, parent_dd->dict.n);
	dtview(dd->dict.e, parent_dd->dict.e);
	dtview(dd->dict.g, parent_dd->dict.g);
    } else {
	if (ProtoGraph && g != ProtoGraph) {
	    /* it's not ok to dtview here for several reasons. the proto
	       graph could change, and the sym indices don't match */
	    parent_dd = agdatadict(ProtoGraph, false);
	    agcopydict(parent_dd->dict.n, dd->dict.n, g, AGNODE);
	    agcopydict(parent_dd->dict.e, dd->dict.e, g, AGEDGE);
	    agcopydict(parent_dd->dict.g, dd->dict.g, g, AGRAPH);
	}
    }
    return dd;
}

/* look up an attribute with possible viewpathing */
static Agsym_t *agdictsym(Dict_t * dict, char *name)
{
    Agsym_t key;
    key.name = name;
    return dtsearch(dict, &key);
}

/* look up attribute in local dictionary with no view pathing */
static Agsym_t *aglocaldictsym(Dict_t * dict, char *name)
{
    Agsym_t *rv;
    Dict_t *view;

    view = dtview(dict, NULL);
    rv = agdictsym(dict, name);
    dtview(dict, view);
    return rv;
}

Agsym_t *agattrsym(void *obj, char *name)
{
    Agattr_t *data;
    Agsym_t *rv;

    data = agattrrec(obj);
    if (data)
	rv = agdictsym(data->dict, name);
    else
	rv = NULL;
    return rv;
}

/* to create a graph's, node's edge's string attributes */

char *AgDataRecName = "_AG_strdata";

static int topdictsize(Agobj_t * obj)
{
    Dict_t *d;

    d = agdictof(agroot(agraphof(obj)), AGTYPE(obj));
    return d ? dtsize(d) : 0;
}

/* g can be either the enclosing graph, or ProtoGraph */
static Agrec_t *agmakeattrs(Agraph_t * context, void *obj)
{
    int sz;
    Agattr_t *rec;
    Agsym_t *sym;
    Dict_t *datadict;

    rec = agbindrec(obj, AgDataRecName, sizeof(Agattr_t), false);
    datadict = agdictof(context, AGTYPE(obj));
    assert(datadict);
    if (rec->dict == NULL) {
	rec->dict = agdictof(agroot(context), AGTYPE(obj));
	/* don't malloc(0) */
	sz = topdictsize(obj);
	if (sz < MINATTR)
	    sz = MINATTR;
	rec->str = agalloc(agraphof(obj), (size_t) sz * sizeof(char *));
	/* doesn't call agxset() so no obj-modified callbacks occur */
	for (sym = dtfirst(datadict); sym; sym = dtnext(datadict, sym))
	    rec->str[sym->id] = agstrdup(agraphof(obj), sym->defval);
    } else {
	assert(rec->dict == datadict);
    }
    return (Agrec_t *) rec;
}

static void freeattr(Agobj_t * obj, Agattr_t * attr)
{
    int i, sz;
    Agraph_t *g;

    g = agraphof(obj);
    sz = topdictsize(obj);
    for (i = 0; i < sz; i++)
	agstrfree(g, attr->str[i]);
    agfree(g, attr->str);
}

static void freesym(void *obj) {
    Agsym_t *sym;

    sym = obj;
    agstrfree(Ag_G_global, sym->name);
    agstrfree(Ag_G_global, sym->defval);
    agfree(Ag_G_global, sym);
}

Agattr_t *agattrrec(void *obj)
{
  return (Agattr_t *)aggetrec(obj, AgDataRecName, 0);
}


static void addattr(Agraph_t * g, Agobj_t * obj, Agsym_t * sym)
{
    Agattr_t *attr = agattrrec(obj);
    assert(attr != NULL);
    if (sym->id >= MINATTR)
	attr->str = agrealloc(g, attr->str,
						     (size_t) sym->id *
						     sizeof(char *),
						     ((size_t) sym->id +
						      1) * sizeof(char *));
    attr->str[sym->id] = agstrdup(g, sym->defval);
}

static Agsym_t *getattr(Agraph_t *g, int kind, char *name) {
  Agsym_t *rv = 0;
  Dict_t *dict;
  dict = agdictof(g, kind);
  if (dict) {
    rv = agdictsym(dict, name); // viewpath up to root
  }
  return rv;
}

static void unviewsubgraphsattr(Agraph_t *parent, char *name) {
  Agraph_t *subg;
  Agsym_t *psym, *lsym;
  Dict_t *ldict;

  psym = getattr(parent, AGRAPH, name);
  if (!psym) {
    return; // supposedly can't happen, see setattr()
  }
  for (subg = agfstsubg(parent); subg; subg = agnxtsubg(subg)) {
    ldict = agdatadict(subg, true)->dict.g;
    lsym = aglocaldictsym(ldict, name);
    if (lsym) {
      continue;
    }
    lsym = agnewsym(agroot(subg), name, agxget(subg, psym), psym->id, AGRAPH);
    dtinsert(ldict, lsym);
  }
}

static Agsym_t *setattr(Agraph_t * g, int kind, char *name, const char *value) {
    Dict_t *ldict, *rdict;
    Agsym_t *lsym, *psym, *rsym, *rv;
    Agraph_t *root;
    Agnode_t *n;
    Agedge_t *e;

    assert(value);
    root = agroot(g);
    agdatadict(g, true);	/* force initialization of string attributes */
    ldict = agdictof(g, kind);
    lsym = aglocaldictsym(ldict, name);
    if (lsym) {			/* update old local definition */
	if (g != root && streq(name, "layout"))
	    agwarningf("layout attribute is invalid except on the root graph\n");
        if (kind == AGRAPH) {
	    unviewsubgraphsattr(g,name);
        }
	agstrfree(g, lsym->defval);
	lsym->defval = agstrdup(g, value);
	rv = lsym;
    } else {
	psym = agdictsym(ldict, name);	/* search with viewpath up to root */
	if (psym) {		/* new local definition */
	    lsym = agnewsym(g, name, value, psym->id, kind);
	    dtinsert(ldict, lsym);
	    rv = lsym;
	} else {		/* new global definition */
	    rdict = agdictof(root, kind);
	    rsym = agnewsym(g, name, value, dtsize(rdict), kind);
	    dtinsert(rdict, rsym);
	    switch (kind) {
	    case AGRAPH:
		agapply(root, (Agobj_t *)root, (agobjfn_t)addattr, rsym, true);
		break;
	    case AGNODE:
		for (n = agfstnode(root); n; n = agnxtnode(root, n))
		    addattr(g, (Agobj_t *) n, rsym);
		break;
	    case AGINEDGE:
	    case AGOUTEDGE:
		for (n = agfstnode(root); n; n = agnxtnode(root, n))
		    for (e = agfstout(root, n); e; e = agnxtout(root, e))
			addattr(g, (Agobj_t *) e, rsym);
		break;
	    default:
		UNREACHABLE();
	    }
	    rv = rsym;
	}
    }
    if (rv && kind == AGRAPH)
	agxset(g, rv, value);
    agmethod_upd(g, g, rv);
    return rv;
}

/*
 * create or update an existing attribute and return its descriptor.
 * if the new value is NULL, this is only a search, no update.
 * when a new attribute is created, existing graphs/nodes/edges
 * receive its default value.
 */
Agsym_t *agattr(Agraph_t * g, int kind, char *name, const char *value) {
    Agsym_t *rv;

    if (g == 0) {
	if (ProtoGraph == 0)
	    ProtoGraph = agopen(0, ProtoDesc, 0);
	g = ProtoGraph;
    }
    if (value)
	rv = setattr(g, kind, name, value);
    else
	rv = getattr(g, kind, name);
    return rv;
}

Agsym_t *agnxtattr(Agraph_t * g, int kind, Agsym_t * attr)
{
    Dict_t *d;
    Agsym_t *rv;

    if ((d = agdictof(g, kind))) {
	if (attr)
	    rv = dtnext(d, attr);
	else
	    rv = dtfirst(d);
    } else
	rv = 0;
    return rv;
}

/* Create or delete attributes associated with an object */

void agraphattr_init(Agraph_t * g)
{
    Agraph_t *context;

    g->desc.has_attrs = true;
    agmakedatadict(g);
    if (!(context = agparent(g)))
	context = g;
    agmakeattrs(context, g);
}

int agraphattr_delete(Agraph_t * g)
{
    Agdatadict_t *dd;
    Agattr_t *attr;

    Ag_G_global = g;
    if ((attr = agattrrec(g))) {
	freeattr((Agobj_t *) g, attr);
	agdelrec(g, attr->h.name);
    }

    if ((dd = agdatadict(g, false))) {
	if (agdtclose(g, dd->dict.n)) return 1;
	if (agdtclose(g, dd->dict.e)) return 1;
	if (agdtclose(g, dd->dict.g)) return 1;
	agdelrec(g, dd->h.name);
    }
    return 0;
}

void agnodeattr_init(Agraph_t * g, Agnode_t * n)
{
    Agattr_t *data;

    data = agattrrec(n);
    if (!data || !data->dict)
	(void) agmakeattrs(g, n);
}

void agnodeattr_delete(Agnode_t * n)
{
    Agattr_t *rec;

    if ((rec = agattrrec(n))) {
	freeattr((Agobj_t *) n, rec);
	agdelrec(n, AgDataRecName);
    }
}

void agedgeattr_init(Agraph_t * g, Agedge_t * e)
{
    Agattr_t *data;

    data = agattrrec(e);
    if (!data || !data->dict)
	(void) agmakeattrs(g, e);
}

void agedgeattr_delete(Agedge_t * e)
{
    Agattr_t *rec;

    if ((rec = agattrrec(e))) {
	freeattr((Agobj_t *) e, rec);
	agdelrec(e, AgDataRecName);
    }
}

char *agget(void *obj, char *name)
{
    Agsym_t *sym;
    Agattr_t *data;
    char *rv;

    sym = agattrsym(obj, name);
    if (sym == NULL)
	rv = 0;			/* note was "", but this provides more info */
    else {
	data = agattrrec(obj);
	rv = data->str[sym->id];
    }
    return rv;
}

char *agxget(void *obj, Agsym_t * sym)
{
    Agattr_t *data;
    char *rv;

    data = agattrrec(obj);
    assert(sym->id >= 0 && sym->id < topdictsize(obj));
    rv = data->str[sym->id];
    return rv;
}

int agset(void *obj, char *name, const char *value) {
    Agsym_t *sym;
    int rv;

    sym = agattrsym(obj, name);
    if (sym == NULL)
	rv = FAILURE;
    else
	rv = agxset(obj, sym, value);
    return rv;
}

int agxset(void *obj, Agsym_t * sym, const char *value)
{
    Agraph_t *g;
    Agobj_t *hdr;
    Agattr_t *data;
    Agsym_t *lsym;

    g = agraphof(obj);
    hdr = obj;
    data = agattrrec(hdr);
    assert(sym->id >= 0 && sym->id < topdictsize(obj));
    agstrfree(g, data->str[sym->id]);
    data->str[sym->id] = agstrdup(g, value);
    if (hdr->tag.objtype == AGRAPH) {
	/* also update dict default */
	Dict_t *dict;
	dict = agdatadict(g, false)->dict.g;
	if ((lsym = aglocaldictsym(dict, sym->name))) {
	    agstrfree(g, lsym->defval);
	    lsym->defval = agstrdup(g, value);
	} else {
	    lsym = agnewsym(g, sym->name, value, sym->id, AGTYPE(hdr));
	    dtinsert(dict, lsym);
	}
    }
    agmethod_upd(g, obj, sym);
    return SUCCESS;
}

int agsafeset(void *obj, char *name, const char *value, const char *def) {
    Agsym_t *a;

    a = agattr(agraphof(obj), AGTYPE(obj), name, 0);
    if (!a)
	a = agattr(agraphof(obj), AGTYPE(obj), name, def);
    return agxset(obj, a, value);
}

static void agraphattr_init_wrapper(Agraph_t *g, Agobj_t *ignored1,
                                    void *ignored2) {
  (void)ignored1;
  (void)ignored2;

  agraphattr_init(g);
}

/*
 * attach attributes to the already created graph objs.
 * presumably they were already initialized, so we don't invoke
 * any of the old methods.
 */
static void init_all_attrs(Agraph_t * g)
{
    Agraph_t *root;
    Agnode_t *n;
    Agedge_t *e;

    root = agroot(g);
    agapply(root, (Agobj_t*)root, agraphattr_init_wrapper, NULL, true);
    for (n = agfstnode(root); n; n = agnxtnode(root, n)) {
	agnodeattr_init(g, n);
	for (e = agfstout(root, n); e; e = agnxtout(root, e)) {
	    agedgeattr_init(g, e);
	}
    }
}

/* Assumes attributes have already been declared.
 * Do not copy key attribute for edges, as this must be distinct.
 * Returns non-zero on failure or if objects have different type.
 */
int agcopyattr(void *oldobj, void *newobj)
{
    Agraph_t *g;
    Agsym_t *sym;
    Agsym_t *newsym;
    char* val;
    char* nval;
    int r = 1;

    g = agraphof(oldobj);
    if (AGTYPE(oldobj) != AGTYPE(newobj))
	return 1;
    sym = 0;
    while ((sym = agnxtattr(g, AGTYPE(oldobj), sym))) {
	newsym = agattrsym(newobj, sym->name);
	if (!newsym)
	    return 1;
	val = agxget(oldobj, sym);
	r = agxset(newobj, newsym, val);
	/* FIX(?): Each graph has its own string cache, so a whole new refstr is possibly
	 * allocated. If the original was an html string, make sure the new one is as well.
	 * If cgraph goes to single string table, this can be removed.
	 */
	if (aghtmlstr (val)) {
	    nval = agxget (newobj, newsym);
	    agmarkhtmlstr (nval);
	}
    }
    return r;
}
