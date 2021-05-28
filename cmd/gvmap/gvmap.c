/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define STANDALONE
#include <sparse/general.h>
#include <sparse/QuadTree.h>
#include <time.h>
#include <sparse/SparseMatrix.h>
#include <getopt.h>
#include <string.h>
#include "make_map.h"
#include <sfdpgen/spring_electrical.h>
#include <sfdpgen/post_process.h>
#include <neatogen/overlap.h>
#include <sparse/clustering.h>
#include <ingraphs/ingraphs.h>
#include <sparse/DotIO.h>
#include <sparse/colorutil.h>
#include <sparse/color_palette.h>

typedef struct {
    char* cmd;
    char **infiles; 
    FILE* outfile;
    int dim;
    real shore_depth_tol;
    int nrandom; 
    int show_points; 
    real bbox_margin[2]; 
    int useClusters;
    int clusterMethod;
    bool plotedges;
    int color_scheme;
    real line_width;
    char *color_scheme_str;
    char *opacity;
    char *plot_label;
    real *bg_color;
    int improve_contiguity_n;
    int nart;
    bool color_optimize;
    int maxcluster;
    int nedgep;
    char *line_color;
    int include_OK_points;
    int highlight_cluster;
    int seed;      /* seed used to calculate Fiedler vector */
} params_t;

static const char usestr[] =
"   where graphfile must contain node positions, and widths and heights for each node. No overlap between nodes should be present. Acceptable options are: \n\
    -a k - average number of artificial points added along the bounding box of the labels. If < 0, a suitable value is selected automatically. (-1)\n\
    -b v - polygon line width, with v < 0 for no line. (0)\n\
    -c k - polygon color scheme (1)\n\
       0 : no polygons\n\
       1 : pastel (default)\n\
       2 : blue to yellow\n\
       3 : white to red\n\
       4 : light grey to red\n\
       5 : primary colors\n\
       6 : sequential single hue red \n\
       7 : Adam color scheme\n\
       8 : Adam blend\n\
       9 : sequential single hue lighter red \n\
      10 : light grey\n\
    -c_opacity=xx - 2-character hex string for opacity of polygons\n\
    -C k - generate at most k clusters. (0)\n\
    -d s - seed used to calculate Fiedler vector for optimal coloring\n\
    -D   - use top-level cluster subgraphs to specify clustering\n\
    -e   - show edges\n\
    -g c - bounding box color. If not specified, a bounding box is not drawn.\n\
    -h k - number of artificial points added to maintain bridge between endpoints (0)\n\
    -highlight=k - only draw cluster k\n\
    -k   - increase randomness of boundary\n\
    -l s - specify label\n\
    -m v - bounding box margin. If 0, auto-assigned (0)\n\
    -o <file> - put output in <file> (stdout)\n\
    -O   - do NOT do color assignment optimization that maximizes color difference between neighboring countries\n\
    -p k - show points. (0)\n\
       0 : no points\n\
       1 : all points\n\
       2 : label points\n\
       3 : random/artificial points\n\
    -r k - number of random points k used to define sea and lake boundaries. If 0, auto assigned. (0)\n\
    -s v - depth of the sea and lake shores in points. If < 0, auto assigned. (0)\n\
    -t n - improve contiguity up to n times. (0)\n\
    -v   - verbose\n\
    -z c - polygon line color (black)\n";

/* 

   -q f - output format (3)\n\
       0 : Mathematica\n\
       1 : PostScript\n\
       2 : country map\n\
       3 : dot format\n\
*/
    /* e.g., 
       1 [cluster=10, clustercolor="#ff0000"]
       2 [cluster=10]
       (and no other nodes are in cluster10)

       then since we can only use 1 color for the cluster 10, both 1 and 2 will be colored based on the color of node 2. However if you have

       2 [cluster=10]
       1 [cluster=10, clustercolor="#ff0000"]

       then you get both colored red.
       
    */

static void usage(char* cmd, int eval)
{
    fprintf(stderr, "Usage: %s <options> graphfile\n", cmd);
    fputs (usestr, stderr);
    exit(eval);
}

static FILE *openFile(char *name, char *mode, char* cmd)
{
    FILE *fp;
    char *modestr;

    fp = fopen(name, mode);
    if (!fp) {
	if (*mode == 'r')
	    modestr = "reading";
	else
	    modestr = "writing";
	fprintf(stderr, "%s: could not open file %s for %s\n",
		cmd, name, modestr);
	exit(-1);
    }
    return (fp);
}

#define HLPFX "ighlight="
#define N_HLPFX (sizeof(HLPFX)-1)

static void 
init(int argc, char **argv, params_t* pm)
{
  char* cmd = argv[0];
  int c;
  real s;
  int v, r;
  char stmp[3];  /* two character string plus '\0' */

  pm->outfile = NULL;
  pm->opacity = NULL;
  pm->color_scheme_str = NULL;
  pm->nrandom = -1;
  pm->dim = 2;
  pm->shore_depth_tol = 0;
  pm->highlight_cluster = 0;
  pm->useClusters = 0;
  pm->clusterMethod = CLUSTERING_MODULARITY;
  pm->plotedges = false;
  pm->show_points = 0;
  pm->color_scheme = COLOR_SCHEME_PASTEL; 
  pm->line_width = 0;
  pm->plot_label = NULL;
  pm->bg_color = NULL;
  pm->improve_contiguity_n = 0;
  pm->nart = -1;
  pm->color_optimize = true;
  pm->maxcluster = 0;
  pm->nedgep = 0;

  pm->cmd = cmd;
  pm->infiles = NULL;
  pm->line_color = strdup("#000000");
  pm->include_OK_points = FALSE;
  pm->seed = 123;

  /*  bbox_margin[0] =  bbox_margin[1] = -0.2;*/
  pm->bbox_margin[0] =  pm->bbox_margin[1] = 0;

  opterr = 0;
  while ((c = getopt(argc, argv, ":evODQko:m:s:r:p:c:C:l:b:g:t:a:h:z:d:?")) != -1) {
    switch (c) {
    case 'm':
      if ((sscanf(optarg,"%lf",&s) > 0) && (s != 0)){
	    pm->bbox_margin[0] =  pm->bbox_margin[1] = s;
      } else {
        usage(cmd, 1);
      }
      break;
    case 'Q':
      pm->clusterMethod = CLUSTERING_MQ;
      break;
    case 's':
      if ((sscanf(optarg,"%lf",&s) > 0)){
        pm->shore_depth_tol = s;
      } else {
        usage(cmd,1);
      }
      break;
    case 'h':
      if ((sscanf(optarg,"%d",&v) > 0)){
        pm->nedgep = MAX(0, v);
      } else if (!strncmp(optarg, HLPFX, N_HLPFX) && (sscanf(optarg+N_HLPFX,"%d",&v) > 0)) {
        pm->highlight_cluster = MAX(0, v);
      } else {
        usage(cmd,1);
      }
      break;
     case 'r':
      if ((sscanf(optarg,"%d",&r) > 0)){
        pm->nrandom = r;
      }
      break;
    case 't':
      if ((sscanf(optarg,"%d",&r) > 0) && r > 0){
        pm->improve_contiguity_n = r;
      }
      break;
    case 'p':
      pm->show_points = 1;
      if ((sscanf(optarg,"%d",&r) > 0)){
        pm->show_points = MIN(3, r);
      }
      break;
    case 'k':
      pm->include_OK_points = TRUE;
      break;
    case 'v':
      Verbose = 1;
      break;
    case 'D':
      pm->useClusters = 1;
      break;
    case 'e':
      pm->plotedges = true;
      break;
    case 'o':
	  pm->outfile = openFile(optarg, "w", pm->cmd);
      break;
    case 'O':
      pm->color_optimize = false;
      break;
    case 'a':
      if ((sscanf(optarg,"%d",&r) > 0)){
	    pm->nart = r;
      } else {
	    usage(cmd,1);
      }
      break;
    case 'c':
      if (sscanf(optarg,"_opacity=%2s", stmp) > 0 && strlen(stmp) == 2){
        pm->opacity = strdup(stmp);
      } else if ((sscanf(optarg,"%d",&r) > 0) && r >= COLOR_SCHEME_NONE && r <= COLOR_SCHEME_GREY){
        pm->color_scheme = r;
      } else if (knownColorScheme(optarg)) {
        pm->color_scheme = COLOR_SCHEME_NONE;
        pm->color_scheme_str = optarg;
      } else {
        fprintf(stderr,"-c option %s is invalid, must be a valid integer or string\n", optarg);
        usage(cmd, 1);
      }
      break;
    case 'd':
      if (sscanf(optarg,"%d",&v) <= 0){
        usage(cmd,1);
      }
      else
        pm->seed = v;
      break;
    case 'C':
      if (!((sscanf(optarg,"%d",&v) > 0) && v >= 0)){
        usage(cmd,1);
      }
      else
        pm->maxcluster = v;
      break;
    case 'g': {
      gvcolor_t color;
      if (colorxlate(optarg, &color, RGBA_DOUBLE) == COLOR_OK) {
        if (!pm->bg_color) pm->bg_color = N_NEW(3,real);
        pm->bg_color[0] = color.u.RGBA[0];
        pm->bg_color[1] = color.u.RGBA[1];
        pm->bg_color[2] = color.u.RGBA[2];
      }
      break;
    }
    case 'z': {
      FREE (pm->line_color);
      pm->line_color = strdup (optarg);
      break;
    }
    case 'b':
      if (sscanf(optarg,"%lf",&s) > 0) {
        pm->line_width = s;
      } else {
        fprintf (stderr, "%s: unexpected argument \"%s\" for -b flag\n", cmd, optarg);
      }
      break;
    case 'l':
      free (pm->plot_label);
      pm->plot_label = strdup (optarg);
      break;
    case ':':
      fprintf(stderr, "gvpack: option -%c missing argument - ignored\n", optopt);
      break;
    case '?':
      if (optopt == '\0' || optopt == '?')
        usage(cmd, 0);
      else {
        fprintf(stderr, " option -%c unrecognized\n", optopt);
        usage(cmd, 1);
      }
      break;
    }
  }

  argv += optind;
  argc -= optind;
  if (argc)
    pm->infiles = argv;
  if (!pm->outfile)
    pm->outfile = stdout;
}

static int
validateCluster (int n, int* grouping, int clust_num)
{
  int i;
  for (i = 0; i < n; i++) {
      if (grouping[i] == clust_num) return clust_num;
  }
  fprintf (stderr, "Highlighted cluster %d not found - ignored\n", clust_num);
  return 0;
}

static void 
makeMap (SparseMatrix graph, int n, real* x, real* width, int* grouping, 
  char** labels, float* fsz, float* rgb_r, float* rgb_g, float* rgb_b, params_t* pm, Agraph_t* g )
{
  int dim = pm->dim;
  int i, flag = 0;
  SparseMatrix poly_lines, polys, poly_point_map;
  real edge_bridge_tol = 0.;
  int npolys, nverts, *polys_groups, exclude_random;
  real *x_poly, *xcombined;
  SparseMatrix country_graph;
  int improve_contiguity_n = pm->improve_contiguity_n;
#ifdef TIME
  clock_t  cpu;
#endif
  int nr0, nart0;
  int nart, nrandom;

  exclude_random = TRUE;


#ifdef TIME
  cpu = clock();
#endif
  nr0 = nrandom = pm->nrandom; nart0 = nart = pm->nart;
  if (pm->highlight_cluster) {
    pm->highlight_cluster = validateCluster (n, grouping, pm->highlight_cluster);
  }
  make_map_from_rectangle_groups(exclude_random, pm->include_OK_points,
				 n, dim, x, width, grouping, graph, pm->bbox_margin, &nrandom, &nart, pm->nedgep, 
				 pm->shore_depth_tol, edge_bridge_tol, &xcombined, &nverts, &x_poly, &npolys, &poly_lines, 
				 &polys, &polys_groups, &poly_point_map, &country_graph, pm->highlight_cluster, &flag);

  if (Verbose) fprintf(stderr,"nart = %d\n",nart);
  /* compute a good color permutation */
  if (pm->color_optimize && country_graph && rgb_r && rgb_g && rgb_b) 
    map_optimal_coloring(pm->seed, country_graph, rgb_r,  rgb_g, rgb_b);
  else if (pm->color_scheme_str){
    map_palette_optimal_coloring(pm->color_scheme_str, "0,100", country_graph, 0.01, -10,
               &rgb_r, &rgb_g, &rgb_b);
  }

#ifdef TIME
  fprintf(stderr, "map making time = %f\n",((real) (clock() - cpu)) / CLOCKS_PER_SEC);
#endif


  /* now we check to see if all points in the same group are also in the same polygon, if not, the map is not very
     contiguous so we move point positions to improve contiguity */
  if (graph && improve_contiguity_n) {
    for (i = 0; i < improve_contiguity_n; i++){
      improve_contiguity(n, dim, grouping, poly_point_map, x, graph);
      nart = nart0;
      nrandom = nr0;
      make_map_from_rectangle_groups(exclude_random, pm->include_OK_points,
				     n, dim, x, width, grouping, graph, pm->bbox_margin, &nrandom, &nart, pm->nedgep, 
				     pm->shore_depth_tol, edge_bridge_tol, &xcombined, &nverts, &x_poly, &npolys, &poly_lines, 
				     &polys, &polys_groups, &poly_point_map, &country_graph, pm->highlight_cluster, &flag);
    }
    assert(!flag);    
    {
      SparseMatrix D;
      D = SparseMatrix_get_real_adjacency_matrix_symmetrized(graph);
      remove_overlap(dim, D, x, width, 1000, 5000.,
		     ELSCHEME_NONE, 0, NULL, NULL, TRUE, &flag);
      
      nart = nart0;
      nrandom = nr0;
      make_map_from_rectangle_groups(exclude_random, pm->include_OK_points,
				     n, dim, x, width, grouping, graph, pm->bbox_margin, &nrandom, &nart, pm->nedgep, 
				     pm->shore_depth_tol, edge_bridge_tol, &xcombined, &nverts, &x_poly, &npolys, &poly_lines, 
				     &polys, &polys_groups, &poly_point_map, &country_graph, pm->highlight_cluster, &flag);
    }
    assert(!flag);
    
  }

    Dot_SetClusterColor(g, rgb_r,  rgb_g,  rgb_b, grouping);
    plot_dot_map(g, n, dim, x, polys, poly_lines, pm->line_width, pm->line_color, x_poly, polys_groups, labels, fsz, rgb_r, rgb_g, rgb_b, pm->opacity,
           (pm->plotedges?graph:NULL), pm->outfile);
  SparseMatrix_delete(polys);
  SparseMatrix_delete(poly_lines);
  SparseMatrix_delete(poly_point_map);
  FREE(xcombined);
  FREE(x_poly);
  FREE(polys_groups);
}


static void mapFromGraph (Agraph_t* g, params_t* pm)
{
    SparseMatrix graph;
  int n;
  real* width = NULL;
  real* x;
  char** labels = NULL;
  int* grouping;
  float* rgb_r;
  float* rgb_g;
  float* rgb_b;
  float* fsz;

  initDotIO(g);
  graph = Import_coord_clusters_from_dot(g, pm->maxcluster, pm->dim, &n, &width, &x, &grouping, 
					   &rgb_r,  &rgb_g,  &rgb_b,  &fsz, &labels, pm->color_scheme, pm->clusterMethod, pm->useClusters);
  makeMap (graph, n, x, width, grouping, labels, fsz, rgb_r, rgb_g, rgb_b, pm, g);
}

static Agraph_t *gread(FILE * fp)
{
    return agread(fp, (Agdisc_t *) 0);
}

int main(int argc, char *argv[])
{
  params_t pm;
  Agraph_t* g;
  Agraph_t* prevg = NULL;
  ingraph_state ig;

  init(argc, argv, &pm);

  newIngraph (&ig, pm.infiles, gread);
  while ((g = nextGraph (&ig)) != 0) {
    if (prevg) agclose (prevg);
    mapFromGraph (g, &pm);
    prevg = g;
  }

  return 0; 
}
