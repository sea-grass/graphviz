/// @file
/// @ingroup common_utils
/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

%require "3.0"

  /* By default, Bison emits a parser using symbols prefixed with "yy". Graphviz
   * contains multiple Bison-generated parsers, so we alter this prefix to avoid
   * symbol clashes.
   */
%define api.prefix {html}

%code requires {
#include <common/htmltable.h>
#include <common/textspan.h>
}

%{

#include <cgraph/list.h>
#include <common/render.h>
#include <common/htmltable.h>
#include <common/htmllex.h>
#include <stdbool.h>
#include <util/alloc.h>

extern int htmlparse(void);

DEFINE_LIST(sfont, textfont_t *)

static void free_ti(textspan_t item) {
  free(item.str);
}

DEFINE_LIST_WITH_DTOR(textspans, textspan_t, free_ti)

static void free_hi(htextspan_t item) {
  for (size_t i = 0; i < item.nitems; i++) {
    free(item.items[i].str);
  }
  free(item.items);
}

DEFINE_LIST_WITH_DTOR(htextspans, htextspan_t, free_hi)

/// Clean up cell if error in parsing.
static void cleanCell(htmlcell_t *cp);

/// Clean up table if error in parsing.
static void cleanTbl(htmltbl_t *tp) {
  rows_t *rows = &tp->u.p.rows;
  for (size_t r = 0; r < rows_size(rows); ++r) {
    row_t *rp = rows_get(rows, r);
    for (size_t c = 0; c < cells_size(&rp->rp); ++c) {
      cleanCell(cells_get(&rp->rp, c));
    }
  }
  rows_free(rows);
  free_html_data(&tp->data);
  free(tp);
}

static struct {
  htmllabel_t* lbl;       /* Generated label */
  htmltbl_t*   tblstack;  /* Stack of tables maintained during parsing */
  textspans_t  fitemList;
  htextspans_t fspanList;
  agxbuf*      str;       /* Buffer for text */
  sfont_t      fontstack;
  GVC_t*       gvc;
} HTMLstate;

/// Clean up cell if error in parsing.
static void
cleanCell (htmlcell_t* cp)
{
  if (cp->child.kind == HTML_TBL) cleanTbl (cp->child.u.tbl);
  else if (cp->child.kind == HTML_TEXT) free_html_text (cp->child.u.txt);
  free_html_data (&cp->data);
  free (cp);
}

/// Append a new text span to the list.
static void
appendFItemList (agxbuf *ag)
{
    const textspan_t ti = {.str = agxbdisown(ag),
                           .font = *sfont_back(&HTMLstate.fontstack)};
    textspans_append(&HTMLstate.fitemList, ti);
}	

static void
appendFLineList (int v)
{
    htextspan_t lp = {0};
    textspans_t *ilist = &HTMLstate.fitemList;

    size_t cnt = textspans_size(ilist);
    lp.just = v;
    if (cnt) {
	lp.nitems = cnt;
	lp.items = gv_calloc(cnt, sizeof(textspan_t));

	for (size_t i = 0; i < textspans_size(ilist); ++i) {
	    // move this text span into the new list
	    textspan_t *ti = textspans_at(ilist, i);
	    lp.items[i] = *ti;
	    *ti = (textspan_t){0};
	}
    }
    else {
	lp.items = gv_alloc(sizeof(textspan_t));
	lp.nitems = 1;
	lp.items[0].str = gv_strdup("");
	lp.items[0].font = *sfont_back(&HTMLstate.fontstack);
    }

    textspans_clear(ilist);

    htextspans_append(&HTMLstate.fspanList, lp);
}

static htmltxt_t*
mkText(void)
{
    htextspans_t *ispan = &HTMLstate.fspanList;
    htmltxt_t *hft = gv_alloc(sizeof(htmltxt_t));

    if (!textspans_is_empty(&HTMLstate.fitemList))
	appendFLineList (UNSET_ALIGN);

    size_t cnt = htextspans_size(ispan);
    hft->nspans = cnt;
    	
    hft->spans = gv_calloc(cnt, sizeof(htextspan_t));
    for (size_t i = 0; i < htextspans_size(ispan); ++i) {
    	// move this HTML text span into the new list
    	htextspan_t *hi = htextspans_at(ispan, i);
    	hft->spans[i] = *hi;
    	*hi = (htextspan_t){0};
    }

    htextspans_clear(ispan);

    return hft;
}

static row_t *lastRow(void) {
  htmltbl_t* tbl = HTMLstate.tblstack;
  row_t *sp = *rows_back(&tbl->u.p.rows);
  return sp;
}

/// Add new cell row to current table.
static void addRow(void) {
  htmltbl_t* tbl = HTMLstate.tblstack;
  row_t *sp = gv_alloc(sizeof(row_t));
  if (tbl->hrule)
    sp->ruled = true;
  rows_append(&tbl->u.p.rows, sp);
}

/// Set cell body and type and attach to row
static void setCell(htmlcell_t *cp, void *obj, label_type_t kind) {
  htmltbl_t* tbl = HTMLstate.tblstack;
  row_t *rp = *rows_back(&tbl->u.p.rows);
  cells_t *row = &rp->rp;
  cells_append(row, cp);
  cp->child.kind = kind;
  if (tbl->vrule) {
    cp->vruled = true;
    cp->hruled = false;
  }

  if(kind == HTML_TEXT)
  	cp->child.u.txt = obj;
  else if (kind == HTML_IMAGE)
    cp->child.u.img = obj;
  else
    cp->child.u.tbl = obj;
}

/// Create label, given body and type.
static htmllabel_t *mkLabel(void *obj, label_type_t kind) {
  htmllabel_t* lp = gv_alloc(sizeof(htmllabel_t));

  lp->kind = kind;
  if (kind == HTML_TEXT)
    lp->u.txt = obj;
  else
    lp->u.tbl = obj;
  return lp;
}

/* Called on error. Frees resources allocated during parsing.
 * This includes a label, plus a walk down the stack of
 * tables. Note that `cleanTbl` frees the contained cells.
 */
static void cleanup (void)
{
  htmltbl_t* tp = HTMLstate.tblstack;
  htmltbl_t* next;

  if (HTMLstate.lbl) {
    free_html_label (HTMLstate.lbl,1);
    HTMLstate.lbl = NULL;
  }
  while (tp) {
    next = tp->u.p.prev;
    cleanTbl (tp);
    tp = next;
  }

  textspans_clear(&HTMLstate.fitemList);
  htextspans_clear(&HTMLstate.fspanList);

  sfont_free(&HTMLstate.fontstack);
}

/// Return 1 if s contains a non-space character.
static bool nonSpace(const char *s) {
  char   c;

  while ((c = *s++)) {
    if (c != ' ') return true;
  }
  return false;
}

/// Fonts are allocated in the lexer.
static void
pushFont (textfont_t *fp)
{
    textfont_t* curfont = *sfont_back(&HTMLstate.fontstack);
    textfont_t  f = *fp;

    if (curfont) {
	if (!f.color && curfont->color)
	    f.color = curfont->color;
	if ((f.size < 0.0) && (curfont->size >= 0.0))
	    f.size = curfont->size;
	if (!f.name && curfont->name)
	    f.name = curfont->name;
	if (curfont->flags)
	    f.flags |= curfont->flags;
    }

    textfont_t *const ft = dtinsert(HTMLstate.gvc->textfont_dt, &f);
    sfont_push_back(&HTMLstate.fontstack, ft);
}

static void
popFont (void)
{
    (void)sfont_pop_back(&HTMLstate.fontstack);
}

%}

%union  {
  int    i;
  htmltxt_t*  txt;
  htmlcell_t*  cell;
  htmltbl_t*   tbl;
  textfont_t*  font;
  htmlimg_t*   img;
  row_t *p;
}

%token T_end_br T_end_img T_row T_end_row T_html T_end_html
%token T_end_table T_end_cell T_end_font T_string T_error
%token T_n_italic T_n_bold T_n_underline  T_n_overline T_n_sup T_n_sub T_n_s
%token T_HR T_hr T_end_hr
%token T_VR T_vr T_end_vr
%token <i> T_BR T_br
%token <img> T_IMG T_img
%token <tbl> T_table
%token <cell> T_cell
%token <font> T_font T_italic T_bold T_underline T_overline T_sup T_sub T_s

%type <txt> fonttext
%type <cell> cell cells
%type <i> br
%type <tbl> table fonttable
%type <img> image
%type <p> row rows

%start html

%%

html  : T_html fonttext T_end_html { HTMLstate.lbl = mkLabel($2,HTML_TEXT); }
      | T_html fonttable T_end_html { HTMLstate.lbl = mkLabel($2,HTML_TBL); }
      | error { cleanup(); YYABORT; }
      ;

fonttext : text { $$ = mkText(); }
      ;

text : text textitem
     | textitem
     ;

textitem : string { appendFItemList(HTMLstate.str);}
         | br {appendFLineList($1);}
         | font text n_font
         | italic text n_italic
         | underline text n_underline
         | overline text n_overline
         | bold text n_bold
         | sup text n_sup
         | sub text n_sub
         | strike text n_strike
         ;

font : T_font { pushFont ($1); }
      ;

n_font : T_end_font { popFont (); }
      ;

italic : T_italic {pushFont($1);}
          ;

n_italic : T_n_italic {popFont();}
            ;

bold : T_bold {pushFont($1);}
          ;

n_bold : T_n_bold {popFont();}
            ;

strike : T_s {pushFont($1);}
          ;

n_strike : T_n_s {popFont();}
            ;

underline : T_underline {pushFont($1);}
          ;

n_underline : T_n_underline {popFont();}
            ;

overline : T_overline {pushFont($1);}
          ;

n_overline : T_n_overline {popFont();}
            ;

sup : T_sup {pushFont($1);}
          ;

n_sup : T_n_sup {popFont();}
            ;

sub : T_sub {pushFont($1);}
          ;

n_sub : T_n_sub {popFont();}
            ;

br     : T_br T_end_br { $$ = $1; }
       | T_BR { $$ = $1; }
       ;

string : T_string
       | string T_string
       ;

table : opt_space T_table {
          if (nonSpace(agxbuse(HTMLstate.str))) {
            htmlerror ("Syntax error: non-space string used before <TABLE>");
            cleanup(); YYABORT;
          }
          $2->u.p.prev = HTMLstate.tblstack;
          $2->u.p.rows = (rows_t){0};
          HTMLstate.tblstack = $2;
          $2->font = *sfont_back(&HTMLstate.fontstack);
          $<tbl>$ = $2;
        }
        rows T_end_table opt_space {
          if (nonSpace(agxbuse(HTMLstate.str))) {
            htmlerror ("Syntax error: non-space string used after </TABLE>");
            cleanup(); YYABORT;
          }
          $$ = HTMLstate.tblstack;
          HTMLstate.tblstack = HTMLstate.tblstack->u.p.prev;
        }
      ;

fonttable : table { $$ = $1; }
          | font table n_font { $$=$2; }
          | italic table n_italic { $$=$2; }
          | underline table n_underline { $$=$2; }
          | overline table n_overline { $$=$2; }
          | bold table n_bold { $$=$2; }
          ;

opt_space : string
          | /* empty*/
          ;

rows : row { $$ = $1; }
     | rows row { $$ = $2; }
     | rows HR row { $1->ruled = true; $$ = $3; }
     ;

row : T_row { addRow (); } cells T_end_row { $$ = lastRow(); }
      ;

cells : cell { $$ = $1; }
      | cells cell { $$ = $2; }
      | cells VR cell { $1->vruled = true; $$ = $3; }
      ;

cell : T_cell fonttable { setCell($1,$2,HTML_TBL); } T_end_cell { $$ = $1; }
     | T_cell fonttext { setCell($1,$2,HTML_TEXT); } T_end_cell { $$ = $1; }
     | T_cell image { setCell($1,$2,HTML_IMAGE); } T_end_cell { $$ = $1; }
     | T_cell { setCell($1,mkText(),HTML_TEXT); } T_end_cell { $$ = $1; }
     ;

image  : T_img T_end_img { $$ = $1; }
       | T_IMG { $$ = $1; }
       ;

HR  : T_hr T_end_hr
    | T_HR
    ;

VR  : T_vr T_end_vr
    | T_VR
    ;


%%

/* Return parsed label or NULL if failure.
 * Set warn to 0 on success; 1 for warning message; 2 if no expat; 3 for error
 * message.
 */
htmllabel_t*
parseHTML (char* txt, int* warn, htmlenv_t *env)
{
  agxbuf        str = {0};
  htmllabel_t*  l;

  sfont_push_back(&HTMLstate.fontstack, NULL);
  HTMLstate.tblstack = 0;
  HTMLstate.lbl = 0;
  HTMLstate.gvc = GD_gvc(env->g);
  HTMLstate.fitemList = (textspans_t){0};
  HTMLstate.fspanList = (htextspans_t){0};

  HTMLstate.str = &str;

  if (initHTMLlexer (txt, &str, env)) {/* failed: no libexpat - give up */
    *warn = 2;
    l = NULL;
  }
  else {
    htmlparse();
    *warn = clearHTMLlexer ();
    l = HTMLstate.lbl;
  }

  textspans_free(&HTMLstate.fitemList);
  htextspans_free(&HTMLstate.fspanList);

  sfont_free(&HTMLstate.fontstack);

  agxbfree (&str);

  return l;
}

