/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/


/******************************************

	Dijkstra algorithm
	Computes single-source distances for
	weighted graphs

******************************************/


#include <neatogen/bfs.h>
#include <neatogen/dijkstra.h>
#include <limits.h>
#include <stdlib.h>
/* #include <math.h> */

#define MAX_DIST (double)INT_MAX

typedef DistType Word;

#define LOOP while(TRUE)

/* This heap class is suited to the Dijkstra alg.
   data[i]=vertexNum <==> index[vertexNum]=i
*/

#define left(i) (2*(i))
#define right(i) (2*(i)+1)
#define parent(i) ((i)/2)
#define insideHeap(h,i) ((i)<h->heapSize)
#define greaterPriority(h,i,j,dist) (dist[h->data[i]]<dist[h->data[j]])
#define assign(h,i,j,index) {h->data[i]=h->data[j]; index[h->data[i]]=i;}
#define exchange(h,i,j,index) {int temp; \
		temp=h->data[i]; \
		h->data[i]=h->data[j]; \
		h->data[j]=temp; \
		index[h->data[i]]=i; \
		index[h->data[j]]=j; \
}

typedef struct {
    int *data;
    int heapSize;
} heap;

static void heapify(heap * h, int i, int index[], Word dist[])
{
    int l, r, largest;
    while (1) {
	l = left(i);
	r = right(i);
	if (insideHeap(h, l) && greaterPriority(h, l, i, dist))
	    largest = l;
	else
	    largest = i;
	if (insideHeap(h, r) && greaterPriority(h, r, largest, dist))
	    largest = r;

	if (largest == i)
	    break;

	exchange(h, largest, i, index);
	i = largest;
    }
}

#ifdef OBSOLETE
/* originally, the code called mkHeap to allocate space, then
 * initHeap to realloc the space. This is silly.
 * Now we just call initHeap.
 */
static void mkHeap(heap * h, int size)
{
    h->data = N_GNEW(size, int);
    h->heapSize = 0;
}
#endif

static void freeHeap(heap * h)
{
    free(h->data);
}

static void
initHeap(heap * h, int startVertex, int index[], Word dist[], int n)
{
    int i, count;
    int j;    /* We cannot use an unsigned value in this loop */
    /* h->data=(int*) realloc(h->data, (n-1)*sizeof(int)); */
    if (n == 1) h->data = NULL;
    else h->data = N_GNEW(n - 1, int);
    h->heapSize = n - 1;

    for (count = 0, i = 0; i < n; i++)
	if (i != startVertex) {
	    h->data[count] = i;
	    index[i] = count;
	    count++;
	}

    for (j = (n - 1) / 2; j >= 0; j--)
	heapify(h, j, index, dist);
}

static boolean extractMax(heap * h, int *max, int index[], Word dist[])
{
    if (h->heapSize == 0)
	return FALSE;

    *max = h->data[0];
    h->data[0] = h->data[h->heapSize - 1];
    index[h->data[0]] = 0;
    h->heapSize--;
    heapify(h, 0, index, dist);

    return TRUE;
}

static void
increaseKey(heap * h, int increasedVertex, Word newDist, int index[],
	    Word dist[])
{
    int placeInHeap;
    int i;

    if (dist[increasedVertex] <= newDist)
	return;

    placeInHeap = index[increasedVertex];

    dist[increasedVertex] = newDist;

    i = placeInHeap;
    while (i > 0 && dist[h->data[parent(i)]] > newDist) {	/* can write here: greaterPriority(i,parent(i),dist) */
	assign(h, i, parent(i), index);
	i = parent(i);
    }
    h->data[i] = increasedVertex;
    index[increasedVertex] = i;
}

void dijkstra(int vertex, vtx_data * graph, int n, DistType * dist)
{
    int i;
    heap H;
    int closestVertex, neighbor;
    DistType closestDist, prevClosestDist = INT_MAX;
    static int *index;

#ifdef OBSOLETE
    mkHeap(&H, n);
#endif
    index = realloc(index, n * sizeof(int));

    /* initial distances with edge weights: */
    for (i = 0; i < n; i++)
	dist[i] = (DistType) MAX_DIST;
    dist[vertex] = 0;
    for (i = 1; i < graph[vertex].nedges; i++)
	dist[graph[vertex].edges[i]] = (DistType) graph[vertex].ewgts[i];

    initHeap(&H, vertex, index, dist, n);

    while (extractMax(&H, &closestVertex, index, dist)) {
	closestDist = dist[closestVertex];
	if (closestDist == MAX_DIST)
	    break;
	for (i = 1; i < graph[closestVertex].nedges; i++) {
	    neighbor = graph[closestVertex].edges[i];
	    increaseKey(&H, neighbor,
			closestDist +
			(DistType) graph[closestVertex].ewgts[i], index,
			dist);
	}
	prevClosestDist = closestDist;
    }

    /* For dealing with disconnected graphs: */
    for (i = 0; i < n; i++)
	if (dist[i] == MAX_DIST)	/* 'i' is not connected to 'vertex' */
	    dist[i] = prevClosestDist + 10;
    freeHeap(&H);
}

 /* Dijkstra bounded to nodes in *unweighted* radius */
int
dijkstra_bounded(int vertex, vtx_data * graph, int n, DistType * dist,
		 int bound, int *visited_nodes)
 /* make dijkstra, but consider only nodes whose *unweighted* distance from 'vertex'  */
 /* is at most 'bound' */
 /* MON-EFFICIENT implementation, see below. */
{
    int num_visited_nodes;
    int i;
    static boolean *node_in_neighborhood = NULL;
    static int size = 0;
    static int *index;
    Queue Q;
    heap H;
    int closestVertex, neighbor;
    DistType closestDist;
    int num_found = 0;

    /* first, perform BFS to find the nodes in the region */
    mkQueue(&Q, n);
    /* remember that dist should be init. with -1's */
    for (i = 0; i < n; i++) {
	dist[i] = -1;		/* far, TOO COSTLY (O(n))! */
    }
    num_visited_nodes =
	bfs_bounded(vertex, graph, n, dist, &Q, bound, visited_nodes);
    if (size < n) {
	node_in_neighborhood = realloc(node_in_neighborhood, n * sizeof(boolean));
	for (i = size; i < n; i++) {
	    node_in_neighborhood[i] = FALSE;
	}
	size = n;
    }
    for (i = 0; i < num_visited_nodes; i++) {
	node_in_neighborhood[visited_nodes[i]] = TRUE;
    }


#ifdef OBSOLETE
    mkHeap(&H, n);
#endif
    index = realloc(index, n * sizeof(int));

    /* initial distances with edge weights: */
    for (i = 0; i < n; i++)	/* far, TOO COSTLY (O(n))! */
	dist[i] = (DistType) MAX_DIST;
    dist[vertex] = 0;
    for (i = 1; i < graph[vertex].nedges; i++)
	dist[graph[vertex].edges[i]] = (DistType) graph[vertex].ewgts[i];

    /* again, TOO COSTLY (O(n)) to put all nodes in heap! */
    initHeap(&H, vertex, index, dist, n); 

    while (num_found < num_visited_nodes
	   && extractMax(&H, &closestVertex, index, dist)) {
	if (node_in_neighborhood[closestVertex]) {
	    num_found++;
	}
	closestDist = dist[closestVertex];
	if (closestDist == MAX_DIST)
	    break;
	for (i = 1; i < graph[closestVertex].nedges; i++) {
	    neighbor = graph[closestVertex].edges[i];
	    increaseKey(&H, neighbor,
			closestDist +
			(DistType) graph[closestVertex].ewgts[i], index,
			dist);
	}
    }

    /* restore initial false-status of 'node_in_neighborhood' */
    for (i = 0; i < num_visited_nodes; i++) {
	node_in_neighborhood[visited_nodes[i]] = FALSE;
    }
    freeHeap(&H);
    freeQueue(&Q);
    return num_visited_nodes;
}

static void heapify_f(heap * h, int i, int index[], float dist[])
{
    int l, r, largest;
    while (1) {
	l = left(i);
	r = right(i);
	if (insideHeap(h, l) && greaterPriority(h, l, i, dist))
	    largest = l;
	else
	    largest = i;
	if (insideHeap(h, r) && greaterPriority(h, r, largest, dist))
	    largest = r;

	if (largest == i)
	    break;

	exchange(h, largest, i, index);
	i = largest;
    }
}

static void
initHeap_f(heap * h, int startVertex, int index[], float dist[], int n)
{
    int i, count;
    int j;			/* We cannot use an unsigned value in this loop */
    h->data = N_GNEW(n - 1, int);
    h->heapSize = n - 1;

    for (count = 0, i = 0; i < n; i++)
	if (i != startVertex) {
	    h->data[count] = i;
	    index[i] = count;
	    count++;
	}

    for (j = (n - 1) / 2; j >= 0; j--)
	heapify_f(h, j, index, dist);
}

static boolean extractMax_f(heap * h, int *max, int index[], float dist[])
{
    if (h->heapSize == 0)
	return FALSE;

    *max = h->data[0];
    h->data[0] = h->data[h->heapSize - 1];
    index[h->data[0]] = 0;
    h->heapSize--;
    heapify_f(h, 0, index, dist);

    return TRUE;
}

static void
increaseKey_f(heap * h, int increasedVertex, float newDist, int index[],
	      float dist[])
{
    int placeInHeap;
    int i;

    if (dist[increasedVertex] <= newDist)
	return;

    placeInHeap = index[increasedVertex];

    dist[increasedVertex] = newDist;

    i = placeInHeap;
    while (i > 0 && dist[h->data[parent(i)]] > newDist) {	/* can write here: greaterPriority(i,parent(i),dist) */
	assign(h, i, parent(i), index);
	i = parent(i);
    }
    h->data[i] = increasedVertex;
    index[increasedVertex] = i;
}

/* dijkstra_f:
 * Weighted shortest paths from vertex.
 * Assume graph is connected.
 */
void dijkstra_f(int vertex, vtx_data * graph, int n, float *dist)
{
    int i;
    heap H;
    int closestVertex = 0, neighbor;
    float closestDist;
    int *index;

#ifdef OBSOLETE
    mkHeap(&H, n);
#endif
    index = N_GNEW(n, int);

    /* initial distances with edge weights: */
    for (i = 0; i < n; i++)
	dist[i] = MAXFLOAT;
    dist[vertex] = 0;
    for (i = 1; i < graph[vertex].nedges; i++)
	dist[graph[vertex].edges[i]] = graph[vertex].ewgts[i];

    initHeap_f(&H, vertex, index, dist, n);

    while (extractMax_f(&H, &closestVertex, index, dist)) {
	closestDist = dist[closestVertex];
	if (closestDist == MAXFLOAT)
	    break;
	for (i = 1; i < graph[closestVertex].nedges; i++) {
	    neighbor = graph[closestVertex].edges[i];
	    increaseKey_f(&H, neighbor,
			  closestDist + graph[closestVertex].ewgts[i],
			  index, dist);
	}
    }

    freeHeap(&H);
    free(index);
}

// single source shortest paths that also builds terms as it goes
// mostly copied from dijkstra_f above
// returns the number of terms built
int dijkstra_sgd(graph_sgd *graph, int source, term_sgd *terms) {
    heap h;
#ifdef OBSOLETE
    mkHeap(&h, graph->n);
#endif
    int *indices = N_GNEW(graph->n, int);
    float *dists = N_GNEW(graph->n, float);
    int i;
    for (i=0; i<graph->n; i++) {
        dists[i] = MAXFLOAT;
    }
    dists[source] = 0;
    for (i=graph->sources[source]; i<graph->sources[source+1]; i++) {
        int target = graph->targets[i];
        dists[target] = graph->weights[i];
    }
    initHeap_f(&h, source, indices, dists, graph->n);

    int closest = 0, offset = 0;
    while (extractMax_f(&h, &closest, indices, dists)) {
        float d = dists[closest];
        if (d == MAXFLOAT) {
            break;
        }
        // if the target is fixed then always create a term as shortest paths are not calculated from there
        // if not fixed then only create a term if the target index is lower
        if (graph->pinneds[closest] || closest<source) {
            terms[offset].i = source;
            terms[offset].j = closest;
            terms[offset].d = d;
            terms[offset].w = 1 / (d*d);
            offset++;
        }
        for (i=graph->sources[closest]; i<graph->sources[closest+1]; i++) {
            int target = graph->targets[i];
            float weight = graph->weights[i];
            increaseKey_f(&h, target, d+weight, indices, dists);
        }
    }
    freeHeap(&h);
    free(indices);
    free(dists);
    return offset;
}

