/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include <sparse/general.h>
#include <sparse/vector.h>


/*---------------- base vector class ----------- */
Vector Vector_new(int maxlen, size_t size_of_elem, void (*deallocator)(void *v)){
  Vector v;
  v = malloc(sizeof(struct vector_struct));
  if (v == NULL)
    return NULL;
  if (maxlen <= 0) maxlen = 1;
  v->maxlen = maxlen;
  v->len = 0;
  v->size_of_elem = size_of_elem;
  v->deallocator = deallocator;
  v->v = malloc(size_of_elem*maxlen);
  if (!v->v){
    free(v);
    return NULL;
  }
  return v;
}

static Vector Vector_assign(Vector v, void *stuff, int i){
  memcpy(((char*) v->v)+(v->size_of_elem)*i/sizeof(char), stuff, v->size_of_elem);
  return v;
}

Vector Vector_reset(Vector v, void *stuff, int i){
  if (i >= v->len) return NULL;
  if (v->deallocator)(v->deallocator)((char*)v->v + (v->size_of_elem)*i/sizeof(char)); 
  return Vector_assign(v, stuff, i);
}


Vector Vector_add(Vector v, void *stuff){
  if (v->len + 1 >= v->maxlen){
    v->maxlen = v->maxlen + MAX((int) .2*(v->maxlen), 10);
    v->v = realloc(v->v, (v->maxlen)*(v->size_of_elem));
    if (!(v->v)) return NULL;
  }

  return Vector_assign(v, stuff, (v->len)++);
}

void Vector_delete(Vector v){
  int i;
  if (!v) return;
  for (i = 0; i < v->len; i++){
    if (v->deallocator)(v->deallocator)((char*)v->v + (v->size_of_elem)*i/sizeof(char));
  }
  free(v->v);
  v->v = NULL;
  free(v);
};

void* Vector_get(Vector v, int i){
  if (i >= v->len) return NULL;
  return ((char*)v->v + i*(v->size_of_elem)/sizeof(char));
}

int Vector_get_length(Vector v){
  return v->len;
}



/*---------------- integer vector --------------- */

Vector IntegerVector_new(int len){
  return Vector_new(len, sizeof(int), NULL);

}
Vector IntegerVector_add(Vector v, int i){
  return Vector_add(v, (void*) &i);
}

void IntegerVector_delete(Vector v){
  return Vector_delete(v);
}

int* IntegerVector_get(Vector v, int i){
  int *p;
  p = (int*) Vector_get(v, i);
  if (!p) return NULL;
  return (int*) p;
}

int IntegerVector_get_length(Vector v){
  return Vector_get_length(v);
}

Vector IntegerVector_reset(Vector v, int content, int pos){
  return Vector_reset(v, (void*) (&content), pos);
}




/*---------------- string vector --------------- */

static void strdealloactor(void *v){
  char **s;
  s = (char**) v;
  free(*s);
}

Vector StringVector_new(int len, int delete_element_strings){
  /* delete_element_strings decides whether we need to delete each string in the vector or leave it to be cleaned by other handles */
  if (!delete_element_strings){
    return Vector_new(len, sizeof(char*), NULL);
  } else {
    return Vector_new(len, sizeof(char*), strdealloactor);
  }

}
Vector StringVector_add(Vector v, char *s){
  return Vector_add(v, (void*) &s);
}

void StringVector_delete(Vector v){
  return Vector_delete(v);
}

char** StringVector_get(Vector v, int i){
  char **p;
  p = (char**) Vector_get(v, i);
  if (!p) return NULL;
  return p;
}

int StringVector_get_length(Vector v){
  return Vector_get_length(v);
}

Vector StringVector_reset(Vector v, char *content, int pos){
  return Vector_reset(v, (void*) (&content), pos);
}

void StringVector_fprint1(FILE *fp, StringVector v){
  int i;
  if (!v) return;
  for (i = 0; i < StringVector_get_length(v); i++){
    fprintf(fp,"%s\n", *(StringVector_get(v, i)));
  }
}

void StringVector_fprint(FILE *fp, StringVector v){
  int i;
  if (!v) return;
  for (i = 0; i < StringVector_get_length(v); i++){
    fprintf(fp,"%d %s\n", i+1,*(StringVector_get(v, i)));
  }
}

StringVector StringVector_part(StringVector v, int n, int *selected_list){
  /* select a list of n elements from vector v and form a new vector */
  StringVector u;
  char *s, *s2;
  int i;
  u = StringVector_new(1, TRUE);
  for (i = 0; i < n; i++){
    s = *(StringVector_get(v, selected_list[i]));
    s2 = MALLOC(sizeof(char)*(strlen(s)+1));
    strcpy(s2, s);
    StringVector_add(u, s2);
  }
  return u;
}
