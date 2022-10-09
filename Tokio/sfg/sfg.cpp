
/*
 *  Copyright t lefering
 *  parts are (C) Universitaet Passau 1986-1991
 *  parts are Copyright (C) 1998-2021 Free Software Foundation, Inc.
 *  parts are Copyright (C) Felix von Leitner from dietlibc
 *
 *  https://notabug.org/mooigraph/sfgraph
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  These are the four essential freedoms with GNU GPL software:
 *  1: freedom to run the program, for any purpose
 *  2: freedom to study how the program works, and change it to make it do what you wish
 *  3: freedom to redistribute copies to help your Free Software friends
 *  4: freedom to distribute copies of your modified versions to your Free Software friends
 *   ,           ,
 *  /             \
 * ((__-^^-,-^^-__))
 * `-_---'  `---_-'
 *  `--|o`   'o|--'
 *      \  `  /
 *       ): :(
 *       :o_o:
 *        "-"
 *
 * SPDX-License-Identifier: GPL-3.0+
 * License-Filename: LICENSE
 */

 /* Single File Graph layout for directed graphs.
  * the api to use this file is in sfg.h
  * the demo program how to use is sfgdemo.c
  */

#include <stdio.h>
#include <stdlib.h>		/* for calloc() free() */

namespace sfg
{

  /* from <limits.h> for CHAR_BIT is 8 definition */
#define CHAR_BIT 8

/* min (x,y) spacing between nodes */
#define NXSPACING 5
#define NYSPACING 15

#include "sfg.h"

/* how many bytes can a splay key to index on have max.
 * this data type must be large enough for a pointer.
 * The size of `void *', as computed by sizeof.
 * #define SIZEOF_VOID_P 8
 * in configure.ac is:
 * AC_CHECK_SIZEOF([void *])
 * The size of `uintptr_t', as computed by sizeof.
 * #define SIZEOF_UINTPTR_T 8
 *
 * #include <stdint.h>	// for uintptr_t definition
 * typedef unsigned long long int splay_tree_key;
 * typedef uintptr_t splay_tree_key;
 * in this situation it can be:
 */
typedef int splay_tree_key;

/* how many bytes can a splay value have max
 * typedef unsigned long long int splay_tree_value;
 * typedef uintptr_t splay_tree_value;
 * int this situation it can be:
 */
typedef struct gml_node* splay_tree_value;

/* Forward declaration for a tree.  */
typedef struct splay_tree_t* splay_tree;

/* The nodes in the splay tree.  */
struct splay_tree_node_n
{
	/* The key.  */
	splay_tree_key key;

	/* The value.  */
	splay_tree_value value;

	/* The left and right children, respectively.  */
	struct splay_tree_node_n* left;
	struct splay_tree_node_n* right;
};

/* Forward declaration for a node in the tree.  */
typedef struct splay_tree_node_n* splay_tree_node;

/* The type of a function which compares two splay-tree keys.  The
   function should return values as for qsort.  */
typedef int (*splay_tree_compare_fn)(splay_tree_key, splay_tree_key);

/* The type of a function used to deallocate any resources associated
   with the key.  */
typedef void (*splay_tree_delete_key_fn)(splay_tree_key);

/* The type of a function used to deallocate any resources associated
   with the value.  */
typedef void (*splay_tree_delete_value_fn)(splay_tree_value);

/* The type of a function used to iterate over the tree.  */
typedef int (*splay_tree_foreach_fn)(splay_tree_node, void*);

/* The splay tree itself.  */
struct splay_tree_t
{
	/* The root of the tree.  */
	struct splay_tree_node_n* root;

	/* The comparision function.  */
	splay_tree_compare_fn comp;

	/* The deallocate-key function.  NULL if no cleanup is necessary.  */
	splay_tree_delete_key_fn delete_key;

	/* The deallocate-value function.  NULL if no cleanup is necessary.  */
	splay_tree_delete_value_fn delete_value;
};

struct gml_graph
{
	int layouted;		/* set if layout is done */
	int nodenum;		/* highest node number in use */
	int nnodes;		/* number of nodes in the graph */
	int edgenum;		/* highest edge number in use */
	int nedges;		/* number of edges in the graph */
	int maxlevel;		/* maximum relative level */
	int nedgelabels;	/* number of edgelabels */
	int do_edgelabels;	/* if set add edgelabels in the graph */
	int nsinglenodes;	/* number of single nodes */
	int nhedges;		/* number of hor edges */
	int startnodeslevel;	/* level where graph drawing starts */
	int nstartnodes;	/* number of start node numbers */
	int* startnodes;	/* array with start node numbers */
	int xspacing;		/* min x spacing between nodes */
	int yspacing;		/* min y spacing between nodes */
	struct gml_nlist* nodelist;	/* list of nodes */
	struct gml_nlist* nodelistend;
	struct gml_nlist* singlenodelist;	/* list of single nodes */
	struct gml_nlist* singlenodelistend;
	struct gml_elist* edgelist;	/* list of edges */
	struct gml_elist* edgelistend;
	int* nnodes_of_level;	/* number of nodes for each level */
	int widestnnodes;	/* widest number of nodes on one level */
	int widestlevel;	/* widest level */
	int sugi_icrossings;	/* initial crossings */
	int sugi_fcrossings;	/* final crossings */
	int sugi_changes;	/* sugiyama changes made */
	int* numce;		/* number of crossings at every level */
	int* nume;		/* number of edges */
	int maxx;		/* max x pos of drawing */
	int maxy;		/* max y pos of drawing */
	int nodemin;		/* min. node number in use */
	int nodemax;		/* max. node number in use */
	int edgemin;		/* min. edge number in use */
	int edgemax;		/* max. edge number in use */
};

struct gml_node
{
	int nr;			/* uniq node number */
	int tx;			/* text xsize */
	int ty;			/* text ysize */
	int bbx;		/* text xsize */
	int bby;		/* text ysize */
	int dummy;		/* set to 1 if dummy node */
	int elabel;		/* set if node is a edge label */
	int enumber;		/* orig. edge number of the edge label */
	int nselfedges;		/* number of self edges at this node */
	int done;		/* dfs black/white */
	int grey;		/* dfs grey */
	int indegree;		/* incoming edges to node */
	int outdegree;		/* outgoing edges from node */
	int hashedge;		/* set if node has hor. edge */
	void* data;		/* user data */
	int relx;		/* relative xpos */
	int rely;		/* relative ypos */
	int absx;		/* absolute xpos */
	int absy;		/* absolute ypos */
	int lx0;		/* absolute xpos */
	int ly0;		/* absolute xpos */
	int lx1;		/* absolute ypos */
	int ly1;		/* absolute ypos */
	int finx;		/* absolute xpos */
	int finy;		/* absolute ypos */
	struct gml_elist* outgoing_e;	/* source list, outgoing edges */
	struct gml_elist* outgoing_etail;	/* source list, outgoing edges */
	struct gml_elist* incoming_e;	/* target list, incoming edges */
	struct gml_elist* incoming_etail;	/* target list, incoming edges */
	int startnode;		/* node belongs to part of graph with this startnode */
	struct gml_node* el_fnode;	/* in edge-label the from-node */
	struct gml_node* el_tnode;	/* in edge-label the to-node */
};

struct gml_edge
{
	int nr;			/* uniq edge number */
	struct gml_node* from_node;	/* from node */
	struct gml_node* to_node;	/* to node */
	int tx;			/* text xsize */
	int ty;			/* text ysize */
	int elabel;		/* set if there is a edge label */
	int reversed;		/* set if edge is reversed */
	int hedge;		/* set if hor. edge */
};

struct gml_nlist
{
	struct gml_node* node;
	struct gml_nlist* next;
};

struct gml_elist
{
	struct gml_edge* edge;
	struct gml_elist* next;
};
/* local vars */
struct gml_graph* maingraph = NULL;

/* by uniq number of node */
static splay_tree uniqnode_splaytree = NULL;

/* forward declarations zz */
static struct splay_tree_node_n* splay(splay_tree sp, splay_tree_key key);
static splay_tree_node splay_tree_lookup(splay_tree sp, splay_tree_key key);
static splay_tree splay_tree_delete(splay_tree sp);
static splay_tree splay_tree_new(splay_tree_compare_fn compare_fn, splay_tree_delete_key_fn delete_key_fn,
	splay_tree_delete_value_fn delete_value_fn);
static int splay_tree_compare_ints(splay_tree_key k1, splay_tree_key k2);
static struct gml_node* uniqnode(struct gml_graph* g, int nr);
static void uniqnode_add(struct gml_graph* g, struct gml_node* node);
static void clear_nodelist(struct gml_graph* g);
static void clear_edgelist(struct gml_graph* g);
static void prep(struct gml_graph* g);
static void reorg(struct gml_graph* g);
static void uncycle(struct gml_graph* g);
static void make_stlist(struct gml_graph* g);
static void clear_stlist(struct gml_node* node);
static void clear_stlist_all(struct gml_graph* g);
static void ylevels(struct gml_graph* g);
static void set_level2(struct gml_graph* g, struct gml_node* n, int i, int startnode);
static void shorteredges(struct gml_graph* g);
static void edgesdownwards(struct gml_graph* g);
static void edgelen(struct gml_graph* g);
static void doublespacey(struct gml_graph* g);
static void edgelabels(struct gml_graph* g);
static void splitedges(struct gml_graph* g);
static void nodecounts(struct gml_graph* g);
static void barycenter(struct gml_graph* g, int it1v, int it2v);
static void improve_positions(struct gml_graph* g);
static void finalxy(struct gml_graph* g);
static struct gml_edge* findedge(int num);
static void setminmax(struct gml_graph* g);

/* returns a version number as version 1.0 returns int 10 */
int sfg_version(void)
{
	return (30);
}

/* init
 * returns  0 if oke
 * returns -1 if already inited
 * returns -2 if other error
 */

int sfg_init()
{
	if (maingraph)
	{
		return (-1);
	}
	maingraph = (gml_graph*)calloc(1, sizeof(struct gml_graph));
	if (maingraph == NULL)
	{
		return (-2);
	}
	uniqnode_splaytree = splay_tree_new(splay_tree_compare_ints, NULL, NULL);
	if (uniqnode_splaytree == NULL)
	{
		return (-2);
	}
	/* min (x,y) spacing between nodes */
	maingraph->xspacing = NXSPACING;
	maingraph->yspacing = NYSPACING;
	maingraph->do_edgelabels = 1;
	return (0);
}

/* de-init
 * returns  0 if oke
 * returns -1 if not inited
 */
int sfg_deinit(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->numce)
	{
		free(maingraph->numce);
		maingraph->numce = NULL;
	}
	if (maingraph->nnodes_of_level)
	{
		free(maingraph->nnodes_of_level);
		maingraph->nnodes_of_level = NULL;
	}
	if (maingraph->startnodes)
	{
		free(maingraph->startnodes);
		maingraph->startnodes = NULL;
	}
	clear_stlist_all(maingraph);
	clear_edgelist(maingraph);
	clear_nodelist(maingraph);
	uniqnode_splaytree = splay_tree_delete(uniqnode_splaytree);
	free(maingraph);
	maingraph = NULL;
	return (0);
}

/* add a node with uniq number starting at 1
 * with (tx,ty) as rectangle size for label text or (0,0)
 * before adding edges all node numbers must be defined
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if number is lower then 1
 * returns -3 if tx number is lower then 0
 * returns -4 if ty number is lower then 0
 * returns -5 if layout already done
 * returns -6 if node already defined
 * returns -7 if other error
 */
int sfg_addnode(int number, int tx, int ty)
{
	struct gml_node* nn = NULL;
	struct gml_nlist* nl = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (number < 1)
	{
		return (-2);
	}
	if (tx < 0)
	{
		return (-3);
	}
	if (ty < 0)
	{
		return (-4);
	}
	if (maingraph->layouted)
	{
		return (-5);
	}
	/* check if node does exist already */
	if (uniqnode(maingraph, number))
	{
		return (-6);
	}
	/* create the new node */
	nn = (gml_node*)calloc(1, sizeof(struct gml_node));
	if (nn == NULL)
	{
		return (-7);
	}
	nl = (gml_nlist*)calloc(1, sizeof(struct gml_nlist));
	if (nl == NULL)
	{
		free(nn);
		return (-7);
	}
	nn->nr = number;
	nn->tx = tx;
	nn->ty = ty;
	/* data field is inited NULL and is set via other routine */
	nl->node = nn;
	if (maingraph->nodelist == NULL)
	{
		maingraph->nodelist = nl;
		maingraph->nodelistend = nl;
	}
	else
	{
		maingraph->nodelistend->next = nl;
		maingraph->nodelistend = nl;
	}
	if (number > maingraph->nodenum)
	{
		/* highest node number in use */
		maingraph->nodenum = number;
	}
	uniqnode_add(maingraph, nn);
	/* number of nodes in the graph */
	maingraph->nnodes++;
	return (0);
}

/* add a edge with uniq number starting at 1
 * the from-node number is in from, the to-node number is in to
 * self-edges are allowed but not with a label
 * with (tx,ty) as rectangle size for label text or (0,0)
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if number is lower then 1
 * returns -3 if tx number is lower then 0
 * returns -4 if ty number is lower then 0
 * returns -5 if from-node number is not defined
 * returns -6 if to-node number is not defined
 * returns -7 if self-edge with a label
 * returns -8 if layout already done
 * returns -9 if other error
 */
int sfg_addedge(int number, int from, int to, int tx, int ty)
{
	struct gml_node* fn = NULL;
	struct gml_node* tn = NULL;
	struct gml_edge* e = NULL;
	struct gml_elist* el = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (number < 1)
	{
		return (-2);
	}
	if (tx < 0)
	{
		return (-3);
	}
	if (ty < 0)
	{
		return (-4);
	}
	if (from < 1)
	{
		return (-5);
	}
	if (to < 1)
	{
		return (-6);
	}
	if (from == to)
	{
		if (tx || ty)
		{	/* do not allow self-edge with a label */
			return (-7);
		}
	}
	if (maingraph->layouted)
	{
		return (-8);
	}
	fn = uniqnode(maingraph, from);
	if (fn == NULL)
	{
		return (-5);
	}
	tn = uniqnode(maingraph, to);
	if (tn == NULL)
	{
		return (-6);
	}
	if (number > maingraph->edgenum)
	{
		maingraph->edgenum = number;
	}
	maingraph->nedges++;
	if (fn == tn)
	{
		/* at self-edge increase counter at node */
		fn->nselfedges++;
	}
	else
	{
		/* fresh new edge */
		e = (gml_edge*)calloc(1, sizeof(struct gml_edge));
		if (e == NULL)
		{
			return (-9);
		}
		el = (gml_elist*)calloc(1, sizeof(struct gml_elist));
		if (el == NULL)
		{
			free(e);
			return (-9);
		}
		e->nr = number;
		e->from_node = fn;
		e->to_node = tn;
		e->tx = tx;
		e->ty = ty;
		if (tx || ty)
		{
			/* mark there is a edgelabel */
			e->elabel = 1;
			/* number of edge labels in the graph */
			maingraph->nedgelabels++;
		}
		el->edge = e;
		if (maingraph->edgelist == NULL)
		{
			maingraph->edgelist = el;
			maingraph->edgelistend = el;
		}
		else
		{
			maingraph->edgelistend->next = el;
			maingraph->edgelistend = el;
		}
	}
	return (0);
}

/* run sugiyama barycenter layout
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if layout already done
 * returns -3 if no nodes in the graph
 */
int sfg_layout(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted)
	{
		return (-2);
	}
	if (maingraph->nodelist == NULL)
	{
		return (-3);
	}

	/* prepare */
	prep(maingraph);

	/* re-organize nodelist */
	reorg(maingraph);

	/* change cycles in the graph */
	uncycle(maingraph);

	/* re-organize nodelist */
	reorg(maingraph);

	/* set y level of all nodes */
	ylevels(maingraph);

	/* find shorter edges */
	shorteredges(maingraph);

	/* change edge directions downwards */
	edgesdownwards(maingraph);

	/* check length of edges */
	edgelen(maingraph);

	/* doublespace the vertical levels */
	doublespacey(maingraph);

	/* split edges with label into node->label->node */
	edgelabels(maingraph);

	/* split longer edges */
	splitedges(maingraph);

	/* create level node count data */
	nodecounts(maingraph);

	/* run barycenter using defaults (0,0) or a value */
	barycenter(maingraph, 100, 100);

	/* re-calc positions */
	improve_positions(maingraph);

	/* final (x,y) positioning of nodes/edges */
	finalxy(maingraph);

	/* update node min/max and edge min/max */
	setminmax(maingraph);

	/* set layout is calculated */
	maingraph->layouted = 1;

	return (0);
}

/* return edge crossings in the graph
 * returns crossings >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
int sfg_crossings(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-22);
	}
	return (maingraph->sugi_fcrossings);
}

/* return initial edge crossings in the graph
 * returns crossings >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
int sfg_initialcrossings(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	return (maingraph->sugi_icrossings);
}

/* set 1 to add edgelabels, or 0 to remove edgelabels
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if already layouted
 */
int sfg_edgelabels(int status)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted)
	{
		return (-2);
	}
	if (status)
	{
		maingraph->do_edgelabels = 1;
	}
	else
	{
		maingraph->do_edgelabels = 1;
	}
	return (0);
}

/* return x pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
int sfg_nodexpos(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->finx);
}

/* return y pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
int sfg_nodeypos(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->finy);
}

/* return relative x pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
int sfg_noderelxpos(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->relx);
}

/* return relative y pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
int sfg_noderelypos(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->rely);
}

/* return level y start pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
int sfg_nodely0(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->ly0);
}

/* return level y end pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
int sfg_nodely1(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->ly1);
}

/* return x size of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
int sfg_nodexsize(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->tx);
}

/* return y size of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
int sfg_nodeysize(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->ty);
}

/* set min. x spacing between nodes
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if number is lower then 0
 * returns -3 if layout already done
 */
int sfg_xspacing(int num)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (num < 1)
	{
		return (-2);
	}
	if (maingraph->layouted)
	{
		return (-3);
	}
	maingraph->xspacing = num;
	return (0);
}

/* set min. y spacing between nodes
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if number is lower then 0
 * returns -3 if layout already done
 */
int sfg_yspacing(int num)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (num < 1)
	{
		return (-2);
	}
	if (maingraph->layouted)
	{
		return (-3);
	}
	maingraph->yspacing = num;
	return (0);
}

/* get max x pos in drawing
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
int sfg_maxx(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	return (maingraph->maxx);
}

/* get max y pos in drawing
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
int sfg_maxy(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	return (maingraph->maxy);
}

/* get min node number in use after layout
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if there are no nodes
 */
int sfg_nodemin(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (maingraph->nodelist == NULL)
	{
		return (-3);
	}
	return (maingraph->nodemin);
}

/* get maxc node number in use after layout
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if there are no nodes
 */
int sfg_nodemax(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (maingraph->nodelist == NULL)
	{
		return (-3);
	}
	return (maingraph->nodemax);
}

/* get min edge number in use after layout
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if there are no edges
 */
int sfg_edgemin(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (maingraph->edgelist == NULL)
	{
		return (-3);
	}
	return (maingraph->edgemin);
}

/* get max edge number in use after layout
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if there are no edges
 */
int sfg_edgemax(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (maingraph->edgelist == NULL)
	{
		return (-3);
	}
	return (maingraph->edgemax);
}

/* get number of levels
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
int sfg_nlevels(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	return (maingraph->maxlevel + 1);
}

/* get number of nodes
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
int sfg_nnodes(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	return (maingraph->nnodes);
}

/* get number of edges
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
int sfg_nedges(void)
{
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	return (maingraph->nedges);
}

/* return type of node with uniq number
 * returns type of node, 1=regular, 2=dummy, 3=edgelabel node
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
int sfg_nodetype(int num)
{
	struct gml_node* n = NULL;
	int type = 0;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	if (n->dummy)
	{
		type = 2;
	}
	else if (n->elabel)
	{
		type = 3;
	}
	else
	{
		type = 1;
	}
	return (type);
}

/* return number of selfedges at node
 * returns number of selfedges if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
int sfg_nodeselfedges(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->nselfedges);
}

/* return number of incoming edges to node
 * returns indegree number if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
int sfg_nodeindegree(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->indegree);
}

/* return number of outgoing edges from node
 * returns outdegree number if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
int sfg_nodeoutdegree(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	return (n->outdegree);
}

/* return edge number of node if edgelabel node
 * returns number of original edge with edgelabel if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 * returns -5 if node is not edgelabel
 */
int sfg_nodeenum(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	if (n->elabel == 0)
	{
		return (-5);
	}
	return (n->enumber);
}

/* get optional data pointer of node
 * returns data pointer if oke
 * returns NULL if not inited
 * returns NULL if layout not done
 * returns NULL if nodenumber is < 1
 * returns NULL if node not found
 */
void* sfg_nodedata(int num)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (NULL);
	}
	if (maingraph->layouted == 0)
	{
		return (NULL);
	}
	if (num < 1)
	{
		return (NULL);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (NULL);
	}
	return (n->data);
}

/* set optional data pointer of node
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
int sfg_setnodedata(int num, void* data)
{
	struct gml_node* n = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	n = uniqnode(maingraph, num);
	if (n == NULL)
	{
		return (-4);
	}
	n->data = data;
	return (0);
}

/* get node data and the calculated positions
 * the user must supply a pointer to the callback routine
 * this runs a supplied callback routine for every node
 * the callback routine gets the node number as supplied,
 * the level as supplied and the calculated pos position.
 * the data is the supplied data and can be used similar.
 * when the callback function needs to stop the iteration
 * over the node list then it must return a non-zero status
 * and that status is returned by sfg_node_foreach()
 * the parameters in the callback function are
 * int num, uniq node number
 * int level, relative vertical level
 * int pos, relative horizontal level
 * void *data, optional user data
 * int xpos, x-coord of upperleft node label area or 0 at no label
 * int ypos, y-coord of upperleft node label area or 0 at no label
 * int tx, x size of text area
 * int ty, y size of text area
 * int nselfedges, number of self-edges at this node
 * int type, type of node, 1=regular, 2=dummy, 3=edgelabel node
 * int indegree, number of incoming edges to the node
 * int outdegree, number of outgoing edges from the node
 * int ly0, start y of level of node
 * int ly1, end y of level of node
 * returns  0 from this routine if everything is oke or no data.
 * returns -1 if not inited
 * returns -2 if no layout is done
 * returns -3 if no callback routine
 */
int sfg_node_foreach(int (*getnodedata)
	(int num, int level, int pos, int xpos, int ypos, int tx, int ty, int nselfedges, int type,
		int indegree, int outdegree, int ly0, int ly1))
{
	struct gml_nlist* nl = NULL;
	struct gml_node* n = NULL;
	int status = 0;
	int type = 0;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (getnodedata == NULL)
	{
		return (-3);
	}
	nl = maingraph->nodelist;
	while (nl)
	{
		n = nl->node;
		/* todo set type of node here */
		type = 0;
		status =
			(*getnodedata) (n->nr, n->rely, n->relx, n->finx, n->finy, n->tx, n->ty, n->nselfedges, type,
				n->indegree, n->outdegree, n->ly0, n->ly1);
		if (status != 0)
		{
			break;
		}
		nl = nl->next;
	}
	return (0);
}

/* get from-node of edge
 * returns from-node number if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if edgenumber is < 1
 * returns -4 if edge not found
 */
int sfg_edgefrom(int num)
{
	struct gml_edge* e = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	e = findedge(num);
	if (e == NULL)
	{
		return (-4);
	}
	return (e->from_node->nr);
}

/* get to-node of edge
 * returns to-node number if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if edgenumber is < 1
 * returns -4 if edge not found
 */
int sfg_edgeto(int num)
{
	struct gml_edge* e = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	e = findedge(num);
	if (e == NULL)
	{
		return (-4);
	}
	return (e->to_node->nr);
}

/* get edge type
 * returns type if oke, 1=regular, 2=selfedge, 3=hor. edge
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if edgenumber is < 1
 * returns -4 if edge not found
 */
int sfg_edgetype(int num)
{
	struct gml_edge* e = NULL;
	int type = 0;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	e = findedge(num);
	if (e == NULL)
	{
		return (-4);
	}
	if (e->from_node->nr == e->to_node->nr)
	{
		type = 2;
	}
	else if (e->hedge)
	{
		type = 3;
	}
	else
	{
		type = 1;
	}
	return (type);
}

/* get edge reversed status
 * returns  1 if reversed edge or 0 if not
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if edgenumber is < 1
 * returns -4 if edge not found
 */
int sfg_edgerev(int num)
{
	struct gml_edge* e = NULL;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (num < 1)
	{
		return (-3);
	}
	e = findedge(num);
	if (e == NULL)
	{
		return (-4);
	}
	return (e->reversed);
}

/* get edge data and the calculated position
 * the user must supply a pointer to the callback routine
 * this runs a supplied callback routine for every edge
 * when the callback function needs to stop the iteration
 * over the edge list then it must return a non-zero status
 * the parameters in the callback function are
 * int num, uniq edge number
 * int from, uniq from-node number
 * int to, uniq to-node number
 * void *data,  optional user data
 * int type, 1=regular, 2=selfedge, 3 hor. edge
 * int rev, set if edge is reversed
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if no layout is done
 * returns -3 if no callback routine
 */
int sfg_edge_foreach(int (*getedgedata)(int num, int from, int to, int type, int rev))
{
	struct gml_elist* el = NULL;
	struct gml_edge* e = NULL;
	int status = 0;
	int type = 0;
	if (maingraph == NULL)
	{
		return (-1);
	}
	if (maingraph->layouted == 0)
	{
		return (-2);
	}
	if (getedgedata == NULL)
	{
		return (-3);
	}
	el = maingraph->edgelist;
	while (el)
	{
		e = el->edge;
		/* set type */
		if (e->from_node->nr == e->to_node->nr)
		{
			type = 2;
		}
		else if (e->hedge)
		{
			type = 3;
		}
		else
		{
			type = 1;
		}
		status = (*getedgedata) (e->nr, e->from_node->nr, e->to_node->nr, type, e->reversed);
		if (status != 0)
		{
			break;
		}
		el = el->next;
	}
	return (0);
}

/* A splay-tree datatype.
   Copyright (C) 1998-2021 Free Software Foundation, Inc.
   Contributed by Mark Mitchell (mark@markmitchell.com).
This file is part of GNU CC.
GNU CC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
GNU CC is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

/* For an easily readable description of splay-trees, see:
	 Lewis, Harry R. and Denenberg, Larry.  Data Structures and Their
	 Algorithms.  Harper-Collins, Inc.  1991.  */

	 /* Deallocate NODE (a member of SP), and all its sub-trees.  */
static void splay_tree_delete_helper(splay_tree sp, splay_tree_node node)
{

	if (node == NULL)
	{
		return;
	}

	/* recurse */
	splay_tree_delete_helper(sp, node->left);
	splay_tree_delete_helper(sp, node->right);

	/* free() key if needed */
	if (sp->delete_key)
	{
		(*sp->delete_key) (node->key);
		node->key = (splay_tree_key)0;
	}

	/* free() value if needed */
	if (sp->delete_value)
	{
		(*sp->delete_value) (node->value);
		node->value = (splay_tree_value)0;
	}

	free((void*)node);

	return;
}

/* delete whole sp tree */
static splay_tree splay_tree_delete(splay_tree sp)
{
	if (sp)
	{
		splay_tree_delete_helper(sp, sp->root);
		free((void*)sp);
	}
	return ((splay_tree)0);
}

/* Allocate a new splay tree, using COMPARE_FN to compare nodes,
   DELETE_KEY_FN to deallocate keys, and DELETE_VALUE_FN to deallocate
   values.  */

static splay_tree
splay_tree_new(splay_tree_compare_fn compare_fn, splay_tree_delete_key_fn delete_key_fn, splay_tree_delete_value_fn delete_value_fn)
{
	splay_tree sp = (splay_tree)0;

	/* there must be a compare function, the free() functions are optional */
	if (compare_fn == (splay_tree_compare_fn)0)
	{
		return ((splay_tree)0);
	}

	sp = (splay_tree)calloc(1, sizeof(struct splay_tree_t));

	if (sp == (splay_tree)0)
	{
		return ((splay_tree)0);
	}

	/* set root node to use and the functions */
	sp->root = (splay_tree_node)0;
	sp->comp = compare_fn;
	sp->delete_key = delete_key_fn;
	sp->delete_value = delete_value_fn;

	return ((splay_tree)sp);
}

/* Insert a new node (associating KEY with DATA) into SP.  If a
   previous node with the indicated KEY exists, its data is not replaced
   with the new value.  */

static void splay_tree_insert(splay_tree sp, splay_tree_key key, splay_tree_value value)
{
	splay_tree_node spn = (splay_tree_node)0;
	int comparison = 0;

	if (sp == (splay_tree)0)
	{
		/* no tree */
		return;
	}

	spn = splay_tree_lookup(sp, key);

	if (spn != (splay_tree_node)0)
	{
		/* did already exist */
		return;
	}

	/* Create a new node, and insert it at the root.  */
	spn = (splay_tree_node)calloc(1, sizeof(struct splay_tree_node_n));

	if (spn == (splay_tree_node)0)
	{
		/* shouldnothappen */
		return;
	}

	/* set node data */
	spn->key = key;
	spn->value = value;

	if (sp->root == (splay_tree_node)0)
	{
		/* first entry */
		sp->root = spn;
		return;
	}

	/* add in tree */
	comparison = (*sp->comp) (sp->root->key, key);

	if (comparison < 0)
	{
		spn->left = sp->root;
		spn->right = spn->left->right;
		spn->left->right = (splay_tree_node)0;
	}
	else
	{
		/* > or == */
		spn->right = sp->root;
		spn->left = spn->right->left;
		spn->right->left = (splay_tree_node)0;
	}

	sp->root = spn;

	return;
}

/* Lookup KEY in SP, returning VALUE if present, and NULL
   otherwise.  */
static splay_tree_node splay_tree_lookup(splay_tree sp, splay_tree_key key)
{
	splay_tree_node spn = (splay_tree_node)0;

	if (sp == (splay_tree)0)
	{
		/* no tree */
		return ((splay_tree_node)0);
	}

	if (sp->root == (splay_tree_node)0)
	{
		/* no data */
		return ((splay_tree_node)0);
	}

	if ((*sp->comp) (sp->root->key, key) == 0)
	{
		/* found */
		return ((splay_tree_node)sp->root);
	}

	spn = splay(sp, key);

	if (spn)
	{

		if ((*sp->comp) (sp->root->key, key) == 0)
		{
			/* found */
			return ((splay_tree_node)sp->root);
		}
	}

	/* not found */
	return ((splay_tree_node)0);
}

/* Splay-tree comparison function, treating the keys as ints.  */

static int splay_tree_compare_ints(splay_tree_key k1, splay_tree_key k2)
{
	if ((int)k1 < (int)k2)
	{
		return ((int)-1);
	}
	else if ((int)k1 > (int)k2)
	{
		return (1);
	}
	else
	{
		return (0);
	}
}

/* */
static struct splay_tree_node_n* splay(splay_tree sp, splay_tree_key key)
{
	struct splay_tree_node_n* t = (struct splay_tree_node_n*)0;
	struct splay_tree_node_n* l = (struct splay_tree_node_n*)0;
	struct splay_tree_node_n* r = (struct splay_tree_node_n*)0;
	struct splay_tree_node_n* y = (struct splay_tree_node_n*)0;
	int comparevalue = 0;
	int comparevalue2 = 0;
	struct splay_tree_node_n tmp = {
		/* The key.  */
		(splay_tree_key)0,
		/* The value.  */
		(splay_tree_value)0,
		/* The left and right children, respectively.  */
		(struct splay_tree_node_n*)0,	/* left */
		(struct splay_tree_node_n*)0	/* right */
	};

	/* no tree */
	if (sp == (splay_tree)0)
	{
		return ((struct splay_tree_node_n*)0);
	}

	/* no data in root. return 0 */
	if (sp->root == (struct splay_tree_node_n*)0)
	{
		return ((struct splay_tree_node_n*)0);
	}

	/* current root */
	t = sp->root;

	tmp.left = (struct splay_tree_node_n*)0;
	tmp.right = (struct splay_tree_node_n*)0;

	l = &tmp;
	r = &tmp;

labelstart:

	/* */
	comparevalue = (*sp->comp) (key, t->key);

	if (comparevalue < 0)
	{

		if (t->left == (struct splay_tree_node_n*)0)
		{
			goto labelend;
		}

		/* */
		comparevalue2 = (*sp->comp) (key, t->left->key);

		if (comparevalue2 < 0)
		{

			y = t->left;
			t->left = y->right;
			y->right = t;
			t = y;

			if (t->left == (struct splay_tree_node_n*)0)
			{
				goto labelend;
			}
		}

		/* */
		r->left = t;
		r = t;
		t = t->left;

	}
	else if (comparevalue > 0)
	{

		if (t->right == (struct splay_tree_node_n*)0)
		{
			goto labelend;
		}

		/* */
		comparevalue2 = (*sp->comp) (key, t->right->key);

		if (comparevalue2 > 0)
		{

			/* */
			y = t->right;
			t->right = y->left;
			y->left = t;
			t = y;

			if (t->right == (struct splay_tree_node_n*)0)
			{
				goto labelend;
			}
		}

		/* */
		l->right = t;
		l = t;
		t = t->right;
	}
	else
	{
		/* here if (comparevalue == 0) */
		goto labelend;
	}

	goto labelstart;

labelend:

	l->right = t->left;
	r->left = t->right;
	t->left = tmp.right;
	t->right = tmp.left;

	sp->root = t;

	return ((struct splay_tree_node_n*)t);
}

/* get node with this number */
static struct gml_node* uniqnode(struct gml_graph* g, int nr)
{
	splay_tree_node spn = NULL;
	if (g)
	{
	}
	if (uniqnode_splaytree == NULL)
	{
		return (NULL);
	}
	spn = splay_tree_lookup(uniqnode_splaytree, (splay_tree_key)nr);
	if (spn)
	{
		return ((struct gml_node*)spn->value);
	}
	else
	{
		return (NULL);
	}
}

/* add node to db */
static void uniqnode_add(struct gml_graph* g, struct gml_node* node)
{
	splay_tree_node spn = NULL;
	if (g)
	{
	}
	if (node == NULL)
	{
		/* shouldnothappen */
		return;
	}
	if (uniqnode_splaytree == NULL)
	{
		uniqnode_splaytree = splay_tree_new(splay_tree_compare_ints, NULL, NULL);
	}
	spn = splay_tree_lookup(uniqnode_splaytree, (splay_tree_key)node->nr);
	if (spn)
	{
		/* shouldnothappen */
		return;
	}
	else
	{
		splay_tree_insert(uniqnode_splaytree, (splay_tree_key)node->nr, (splay_tree_value)node);
	}
	return;
}

/* clear nodelist with the nodes */
static void clear_nodelist(struct gml_graph* g)
{
	struct gml_nlist* lnll = NULL;
	struct gml_nlist* nlnext = NULL;
	lnll = g->nodelist;
	while (lnll)
	{
		nlnext = lnll->next;
		free(lnll->node);
		lnll->node = NULL;
		free(lnll);
		lnll = NULL;
		lnll = nlnext;
	}
	g->nodelist = NULL;
	g->nodelistend = NULL;
	g->nodenum = 0;
	g->nnodes = 0;
	return;
}

/* clear edgelist and edge itself */
static void clear_edgelist(struct gml_graph* g)
{
	struct gml_elist* el = NULL;
	struct gml_elist* elnext = NULL;
	el = g->edgelist;
	while (el)
	{
		elnext = el->next;
		free(el->edge);
		el->edge = NULL;
		free(el);
		el = NULL;
		el = elnext;
	}
	g->edgelist = NULL;
	g->edgelistend = NULL;
	g->nedges = 0;
	g->edgenum = 0;
	return;
}

/* optional prepare extra's here */
static void prep(struct gml_graph* g)
{
	struct gml_elist* el = NULL;
	el = g->edgelist;
	while (el)
	{
		/* update degree of nodes */
		el->edge->from_node->outdegree++;
		el->edge->to_node->indegree++;
		el = el->next;
	}

	return;
}

/* re-organize nodelist */
static void reorg(struct gml_graph* g)
{
	struct gml_nlist* nl = NULL;
	struct gml_nlist* nlnext = NULL;
	struct gml_nlist* nn1 = NULL;
	struct gml_nlist* nn2 = NULL;
	struct gml_nlist* nn3 = NULL;
	struct gml_nlist* nn4 = NULL;
	struct gml_nlist* nnl = NULL;
	struct gml_nlist* nnlend = NULL;
	nl = g->nodelist;
	if (nl == NULL)
	{
		return;
	}

	while (nl)
	{
		/* first the single nodes */
		if (nl->node->indegree == 0 && nl->node->outdegree == 0)
		{
			nn1 = (gml_nlist*)calloc(1, sizeof(struct gml_nlist));
			if (nn1)
			{
				nn1->node = nl->node;
				if (nnl == NULL)
				{
					nnl = nn1;
					nnlend = nn1;
				}
				else
				{
					nnlend->next = nn1;
					nnlend = nn1;
				}
			}
		}
		nl = nl->next;
	}

	nl = g->nodelist;
	while (nl)
	{
		/* second the starter nodes */
		if (nl->node->indegree == 0 && nl->node->outdegree != 0)
		{
			nn2 = (gml_nlist*)calloc(1, sizeof(struct gml_nlist));
			if (nn2)
			{
				nn2->node = nl->node;
				if (nnl == NULL)
				{
					nnl = nn2;
					nnlend = nn2;
				}
				else
				{
					nnlend->next = nn2;
					nnlend = nn2;
				}
			}
		}
		nl = nl->next;
	}

	nl = g->nodelist;
	while (nl)
	{
		/* third the middle nodes */
		if (nl->node->indegree != 0 && nl->node->outdegree != 0)
		{
			nn3 = (gml_nlist*)calloc(1, sizeof(struct gml_nlist));
			if (nn3)
			{
				nn3->node = nl->node;
				if (nnl == NULL)
				{
					nnl = nn3;
					nnlend = nn3;
				}
				else
				{
					nnlend->next = nn3;
					nnlend = nn3;
				}
			}
		}
		nl = nl->next;
	}

	nl = g->nodelist;
	while (nl)
	{
		/* fourth the tail nodes */
		if (nl->node->indegree != 0 && nl->node->outdegree == 0)
		{
			nn4 = (gml_nlist*)calloc(1, sizeof(struct gml_nlist));
			if (nn4)
			{
				nn4->node = nl->node;
				if (nnl == NULL)
				{
					nnl = nn4;
					nnlend = nn4;
				}
				else
				{
					nnlend->next = nn4;
					nnlend = nn4;
				}
			}
		}
		nl = nl->next;
	}

	/* clear nodelist but keep the nodes */
	nl = g->nodelist;
	while (nl)
	{
		nlnext = nl->next;
		free(nl);
		nl = NULL;
		nl = nlnext;
	}

	/* set the refreshed nodelist */
	g->nodelist = nnl;
	g->nodelistend = nnlend;

	return;
}

/* recursive dfs */
static int decycle3(struct gml_graph* g, struct gml_node* n, int level, struct gml_edge* e)
{
	struct gml_node* tmpnode = NULL;
	struct gml_node* source = NULL;
	struct gml_node* target = NULL;
	struct gml_elist* el = NULL;
	struct gml_edge* edge = NULL;
	int changed = 0;
	if (e)
	{
	}

	if (n->done)
	{
		if (level > n->rely)
		{
			n->rely = level;
		}
		return (0);
	}

	n->rely = level;
	n->done = 1;
	/* mark this node is being processed */
	n->grey = 1;
	source = n;
	/* follow outgoing edges */
	el = source->outgoing_e;
	while (el)
	{
		edge = el->edge;
		/* get the to-node */
		target = edge->to_node;
		if (target->grey)
		{
			changed++;
			tmpnode = edge->to_node;
			edge->to_node = edge->from_node;
			edge->from_node = tmpnode;
			/* toggle the edge is reversed bit */
			if (edge->reversed)
			{
				edge->reversed = 0;
			}
			else
			{
				edge->reversed = 1;
			}
		}
		else
		{
			if (target->done == 0)
			{
				changed += decycle3(g, target, (level + 1), e);
			}
		}
		el = el->next;
	}

	/* this node is ready being processed */
	n->grey = 0;
	return (changed);
}

/* remove cycles in the graph */
static void uncycle(struct gml_graph* g)
{
	struct gml_nlist* lnll = NULL;
	int changed = 0;
	/* build the s/tlist of a node */
	clear_stlist_all(g);
	make_stlist(g);
	/* revert cycles at the last edge in the chain */
	g->maxlevel = 0;
	lnll = g->nodelist;
	while (lnll)
	{
		lnll->node->rely = -1;
		lnll->node->done = 0;
		lnll->node->grey = 0;
		lnll = lnll->next;
	}

	/* first the startnodes */
	lnll = g->nodelist;
	changed = 0;
	while (lnll)
	{
		/* select start nodes */
		if (lnll->node->indegree == 0 && lnll->node->outdegree != 0)
		{
			if (lnll->node->done == 0)
			{
				/* use v3 */
				changed += decycle3(g, lnll->node, 0, NULL);
			}
		}
		lnll = lnll->next;
	}

	/* check nodes */
	lnll = g->nodelist;
	while (lnll)
	{
		/* select other nodes */
		if (lnll->node->rely == -1)
		{
			/* use v3 */
			changed += decycle3(g, lnll->node, 0, NULL);
		}
		lnll = lnll->next;
	}
	if (changed)
	{
		/* build the s/tlist of a node */
		clear_stlist_all(g);
		make_stlist(g);
	}

	return;
}

/* rebuild nodes st lists */
static void make_stlist(struct gml_graph* g)
{
	struct gml_elist* el = NULL;
	struct gml_edge* edge = NULL;
	struct gml_node* sn = NULL;
	struct gml_node* tn = NULL;
	struct gml_elist* ne = NULL;
	struct gml_nlist* lnll = NULL;
	/* refresh degree count of nodes */
	lnll = g->nodelist;
	while (lnll)
	{
		/* make ure these are cleared */
		lnll->node->outgoing_e = NULL;	/* source list, outgoing edges */
		lnll->node->outgoing_etail = NULL;	/* source list, outgoing edges */
		lnll->node->incoming_e = NULL;	/* target list, incoming edges */
		lnll->node->incoming_etail = NULL;	/* target list, incoming edges */
		lnll->node->indegree = 0;
		lnll->node->outdegree = 0;
		lnll = lnll->next;
	}

	el = g->edgelist;
	while (el)
	{
		edge = el->edge;
		/* from/to nodes */
		sn = edge->from_node;
		tn = edge->to_node;
		ne = (gml_elist*)calloc(1, sizeof(struct gml_elist));
		if (ne == NULL)
		{
			return;
		}

		ne->edge = edge;
		/* list of outgoing edges */
		if (sn->outgoing_e == NULL)
		{
			sn->outgoing_e = ne;
			sn->outgoing_etail = ne;
		}
		else
		{
			sn->outgoing_etail->next = ne;
			sn->outgoing_etail = ne;
		}

		sn->outdegree++;
		ne = (gml_elist*)calloc(1, sizeof(struct gml_elist));
		if (ne == NULL)
		{
			return;
		}

		ne->edge = edge;
		/* list of incoming edges */
		if (tn->incoming_e == NULL)
		{
			tn->incoming_e = ne;
			tn->incoming_etail = ne;
		}
		else
		{
			tn->incoming_etail->next = ne;
			tn->incoming_etail = ne;
		}

		tn->indegree++;
		el = el->next;
	}

	return;
}

/* clear the s/t list of a node */
static void clear_stlist(struct gml_node* node)
{
	struct gml_elist* ell = NULL;
	struct gml_elist* ellnext = NULL;
	/* free outgoing edges */
	ell = node->outgoing_e;
	while (ell)
	{
		ellnext = ell->next;
		free(ell);
		ell = NULL;
		ell = ellnext;
	}

	node->outgoing_e = NULL;
	node->outgoing_etail = NULL;
	node->outdegree = 0;
	/* free incoming edges */
	ell = node->incoming_e;
	while (ell)
	{
		ellnext = ell->next;
		free(ell);
		ell = NULL;
		ell = ellnext;
	}

	node->incoming_e = NULL;
	node->incoming_etail = NULL;
	node->indegree = 0;
	return;
}

/* clear the s/t list of all nodes */
static void clear_stlist_all(struct gml_graph* g)
{
	struct gml_nlist* lnll = NULL;
	lnll = g->nodelist;
	while (lnll)
	{
		clear_stlist(lnll->node);
		lnll = lnll->next;
	}
	return;
}

/* add node as single node */
static void add_singlenode(struct gml_graph* g, struct gml_node* node)
{
	struct gml_nlist* lnll = NULL;
	lnll = (struct gml_nlist*)calloc(1, sizeof(struct gml_nlist));
	if (lnll)
	{
		lnll->node = node;
		if (g->singlenodelist == NULL)
		{
			g->singlenodelist = lnll;
			g->singlenodelistend = lnll;
		}
		else
		{
			g->singlenodelistend->next = lnll;
			g->singlenodelistend = lnll;
		}
	}
	return;
}

/* set rel. y level of all nodes */
static void ylevels(struct gml_graph* g)
{
	struct gml_nlist* lnll = NULL;
	int i = 0;
	int start2 = 0;
	int special = 0;
	int nnodes = 0;
	if (g->nodelist == NULL)
	{
		/* nothing to do */
		return;
	}

	/* no single nodes in the graph */
	g->nsinglenodes = 0;
	/* set all y levels to undefined */
	lnll = g->nodelist;
	nnodes = 0;
	while (lnll)
	{
		nnodes++;
		/* y = -1, means undefined */
		lnll->node->rely = -1;
		lnll->node->done = 0;
		lnll->node->grey = 0;
		lnll->node->startnode = -1;
		/* check for single nodes and mark them */
		if (lnll->node->outgoing_e == NULL && lnll->node->incoming_e == NULL)
		{
			/* set single nodes at fixed level 0 */
			lnll->node->rely = 0;
			lnll->node->done = 1;
			/* node belongs to part of graph with this startnode */
			lnll->node->startnode = 0;
			g->nsinglenodes = (g->nsinglenodes + 1);
			add_singlenode(g, lnll->node);
		}
		lnll = lnll->next;
	}

	/* if there are single nodes on level 0, start the graph at level 1 */
	if (g->nsinglenodes)
	{
		start2 = 1;
	}
	else
	{
		start2 = 0;
	}

	/* number of start nodes in the graph */
	g->nstartnodes = 0;
	/* where the actual drawing starts at y-level */
	g->startnodeslevel = start2;
	special = 0;
	/* dfs */
	lnll = g->nodelist;
	while (lnll)
	{
		if (lnll->node->rely == -1)
		{
			/* select start nodes */
			if (lnll->node->indegree == 0 && lnll->node->outdegree != 0)
			{
				g->nstartnodes++;
				set_level2(g, lnll->node, start2, lnll->node->nr);
			}
		}
		lnll = lnll->next;
	}

	/* check that all nodes have y position now */
	lnll = g->nodelist;
	while (lnll)
	{
		if (lnll->node->rely == -1)
		{
			set_level2(g, lnll->node, start2, lnll->node->nr);
		}
		lnll = lnll->next;
	}

	/* graph can have zero startnodes.
	 * set first node as startnode.
	 */
	if (g->nstartnodes == 0)
	{
		g->nstartnodes++;
		if (g->nodelist)
		{
			set_level2(g, g->nodelist->node, start2, g->nodelist->node->nr);
		}
		special = 1;
	}

	/* fill the table with startnodes */
	g->startnodes = (int*)calloc(1, g->nstartnodes * sizeof(int));
	if (g->startnodes == NULL)
	{
		return;
	}

	/* special case if there were no startnodes */
	if (special)
	{
		/* set first node as startnode */
		if (g->nodelist)
		{
			g->startnodes[0] = g->nodelist->node->nr;
		}
	}
	else
	{
		/* copy the startnodes numbers in the (int *)array */
		i = 0;
		lnll = g->nodelist;
		while (lnll)
		{
			/* no incoming edges and at least one outgoing edge */
			if (lnll->node->indegree == 0 && lnll->node->outdegree != 0)
			{
				/* set node number. this is not the id number in the input. */
				g->startnodes[i] = lnll->node->nr;
				i++;
			}
			lnll = lnll->next;
		}
	}

	return;
}

/* set rel. y level of nodes */
static void set_level2(struct gml_graph* g, struct gml_node* n, int i, int startnode)
{
	struct gml_node* target = NULL;
	struct gml_edge* edge = NULL;
	struct gml_elist* el = NULL;
	if (n->done)
	{
		if (i > n->rely && n->grey == 0)
		{
			n->rely = i;
			if (i > g->maxlevel)
			{
				g->maxlevel = i;
			}
		}
		if (n->grey)
		{
			return;
		}
		if (n->done > 1)
		{
			return;
		}
	}

	n->grey++;
	n->done++;
	n->rely = i;
	n->startnode = startnode;

	if (i > g->maxlevel)
	{
		g->maxlevel = i;
	}

	/* follow outgoing edges */
	el = n->outgoing_e;
	while (el)
	{
		edge = el->edge;
		target = edge->to_node;
		set_level2(g, target, (i + 1), startnode);
		el = el->next;
	}
	n->grey = 0;
	return;
}

/* undo reversed edges and refresh node edgelists */
static void unrev(struct gml_graph* g)
{
	struct gml_elist* el = NULL;
	struct gml_node* tmpnode = NULL;
	struct gml_node* sn = NULL;
	struct gml_node* tn = NULL;
	struct gml_edge* edge = NULL;
	int changed = 0;
	el = g->edgelist;
	while (el)
	{
		edge = el->edge;
		if (el->edge->reversed)
		{
			changed++;
			sn = edge->from_node;
			tn = edge->to_node;
			/* swap */
			tmpnode = tn;
			el->edge->to_node = sn;
			el->edge->from_node = tmpnode;
			el->edge->reversed = 0;
		}
		el = el->next;
	}
	if (changed)
	{
		/* rebuild the in/out edges lists at the nodes in the modified graph */
		g->maxlevel = 0;

		/* refresh st lists */
		clear_stlist_all(g);
		make_stlist(g);
	}
	return;
}

static int do_abs(int i)
{
	if (i < 0)
	{
		return (-i);
	}
	else
	{
		return (i);
	}
}

/* try to find shorter edges */
static void shorteredges(struct gml_graph* g)
{
	struct gml_nlist* lnll = NULL;
	struct gml_elist* ine = NULL;
	struct gml_elist* oute = NULL;
	struct gml_edge* etop = NULL;
	struct gml_edge* ebot = NULL;
	struct gml_node* ntop = NULL;
	struct gml_node* nbot = NULL;
	/* undo reversed edges and refresh node edgelists */
	unrev(g);
	/* move in between nodes at halfway */
	lnll = g->nodelist;
	while (lnll)
	{
		if ((lnll->node->indegree == 1) && (lnll->node->outdegree == 1))
		{
			oute = lnll->node->outgoing_e;
			ine = lnll->node->incoming_e;
			etop = ine->edge;
			ebot = oute->edge;
			ntop = etop->from_node;
			nbot = ebot->to_node;
			if ((do_abs(ntop->rely - lnll->node->rely) > 1)
				|| (do_abs(nbot->rely - lnll->node->rely) > 1))
			{
				/* put node at the middle, does not depend on edge direction up/down */
				lnll->node->rely = ((ntop->rely + nbot->rely) / 2);
			}
		}
		lnll = lnll->next;
	}

	return;
}

/* all edges downwards */
static void edgesdownwards(struct gml_graph* g)
{
	struct gml_elist* el = NULL;
	struct gml_node* tmpnode = NULL;
	struct gml_node* sn = NULL;
	struct gml_node* tn = NULL;
	struct gml_edge* edge = NULL;
	int changed1 = 0;

	el = g->edgelist;
	while (el)
	{
		edge = el->edge;
		sn = edge->from_node;
		tn = edge->to_node;
		if ((tn->rely - sn->rely) < 0)
		{
			/* swap */
			tmpnode = tn;
			el->edge->to_node = el->edge->from_node;
			el->edge->from_node = tmpnode;
			/* toggle the edge is reversed bit */
			if (el->edge->reversed)
			{
				el->edge->reversed = 0;
			}
			else
			{
				el->edge->reversed = 1;
			}
			changed1++;
		}

		el = el->next;
	}
	if (changed1)
	{
		/* rebuild the in/out edges lists at the nodes in the modified graph */
		g->maxlevel = 0;
		/* refresh st lists */
		clear_stlist_all(g);
		make_stlist(g);
	}

	return;
}

/* dfs check again and revers if needed */
static void edgelen(struct gml_graph* g)
{
	struct gml_elist* el = NULL;
	struct gml_edge* edge = NULL;
	struct gml_node* sn = NULL;
	struct gml_node* tn = NULL;
	struct gml_node* tmpnode = NULL;
	int changed = 0;
	el = g->edgelist;
	while (el)
	{
		edge = el->edge;
		sn = edge->from_node;
		tn = edge->to_node;
		if ((tn->rely - sn->rely) < 0)
		{
			changed++;
			tmpnode = tn;
			el->edge->to_node = el->edge->from_node;
			el->edge->from_node = tmpnode;
		}

		el = el->next;
	}

	if (changed)
	{
		/* refresh st lists */
		clear_stlist_all(g);
		make_stlist(g);
	}

	return;
}

/* doublespace the vertical levels */
static void doublespacey(struct gml_graph* g)
{
	struct gml_nlist* lnll = NULL;
	/* same edges now will have different dummy nodes resulting in 2 lines */
	g->maxlevel = 0;
	/* at the odd levels the edge labels will be placed. */
	lnll = g->nodelist;
	while (lnll)
	{
		lnll->node->rely = (2 * lnll->node->rely);
		if (lnll->node->rely > g->maxlevel)
		{
			g->maxlevel = lnll->node->rely;
		}
		lnll = lnll->next;
	}

	return;
}

/* dummy nodes are only in nodelist, not raw nodelist */
static void add_new_dummynode(struct gml_graph* g, int foundid)
{
	struct gml_node* node = NULL;
	struct gml_nlist* lnll = NULL;
	if (uniqnode(maingraph, foundid))
	{
		return;
	}

	node = (gml_node*)calloc(1, sizeof(struct gml_node));
	if (node == NULL)
	{
		return;
	}
	node->nr = foundid;
	uniqnode_add(maingraph, node);
	lnll = (gml_nlist*)calloc(1, sizeof(struct gml_nlist));
	if (lnll == NULL)
	{
		free(node);
		return;
	}

	lnll->node = node;
	if (g->nodelist == NULL)
	{
		g->nodelist = lnll;
		g->nodelistend = lnll;
	}
	else
	{
		g->nodelistend->next = lnll;
		g->nodelistend = lnll;
	}

	return;
}

/* edge to dummy node */
static void add_new_dummyedge(struct gml_graph* g, int foundsource, int foundtarget, int reversed)
{
	struct gml_node* snode = NULL;
	struct gml_node* tnode = NULL;
	struct gml_edge* edge = NULL;
	struct gml_elist* el = NULL;
	snode = uniqnode(maingraph, foundsource);
	if (snode == NULL)
	{
		return;
	}

	tnode = uniqnode(maingraph, foundtarget);
	if (tnode == NULL)
	{
		return;
	}

	edge = (gml_edge*)calloc(1, sizeof(struct gml_edge));
	if (edge == NULL)
	{
		return;
	}
	g->edgenum++;
	edge->nr = g->edgenum;
	edge->from_node = snode;	/* from-node */
	edge->to_node = tnode;	/* to-node */
	edge->reversed = reversed;	/* edge attr. edge-is-reversed */
	el = (gml_elist*)calloc(1, sizeof(struct gml_elist));
	if (el == NULL)
	{
		free(edge);
		return;
	}

	el->edge = edge;
	if (g->edgelist == NULL)
	{
		g->edgelist = el;
		g->edgelistend = el;
	}
	else
	{
		g->edgelistend->next = el;
		g->edgelistend = el;
	}

	return;
}

/* delete edge when replaced with a chain of edges */
static void del_edge(struct gml_graph* g, struct gml_elist* edgeel)
{
	struct gml_elist* elprev = NULL;
	struct gml_elist* el = NULL;
	struct gml_elist* elto = NULL;
	if (g->edgelist == NULL)
	{
		return;
	}

	if (edgeel == g->edgelist)
	{
		g->edgelist = g->edgelist->next;
		if (g->edgelistend == edgeel)
		{
			g->edgelistend = NULL;
		}
		else
		{
			el = g->edgelist;
			elprev = el;
			while (el)
			{
				elprev = el;
				if (el->next == edgeel)
				{
					break;
				}
				el = el->next;
			}
			g->edgelistend = elprev;
		}

		free(edgeel->edge);
		edgeel->edge = NULL;
		free(edgeel);
	}
	else
	{

		elto = edgeel->next;
		el = g->edgelist;
		elprev = el;
		while (el)
		{
			elprev = el;
			if (el->next == edgeel)
			{
				break;
			}
			el = el->next;
		}

		elprev->next = elto;
		if (g->edgelistend == edgeel)
		{
			g->edgelistend = elprev;
		}

		free(edgeel->edge);
		edgeel->edge = NULL;
		free(edgeel);
	}

	return;
}

/* splits edgelabel edges into node->label->node */
static void edgelabels(struct gml_graph* g)
{
	struct gml_elist* el = NULL;
	struct gml_elist* elnext = NULL;
	struct gml_node* ln = NULL;
	char rev = 0;
	int ydiff = 0;
	int enumber = 0;

	if (g->nedgelabels == 0)
	{
		/* no edge labels, nothing todo here */
		return;
	}
	if (g->do_edgelabels == 0)
	{
		/* skip edge label processing */
		return;
	}

	/* scan edges all downwards */
	el = g->edgelist;
	while (el)
	{
		/* make sure, from-node is lower y then to-node */
		if (el->edge->from_node->rely > el->edge->to_node->rely)
		{
			ln = el->edge->from_node;
			el->edge->from_node = el->edge->to_node;
			el->edge->to_node = ln;
			/* toggle rev flag */
			if (el->edge->reversed)
			{
				el->edge->reversed = 0;
			}
			else
			{
				el->edge->reversed = 1;
			}
		}
		el = el->next;
	}

	/* scan edges for edge labels */
	el = g->edgelist;
	while (el)
	{
		elnext = el->next;
		/* do if there is a edge label */
		if (el->edge->elabel)
		{
			/* number of edge with edgelabel */
			enumber = el->edge->nr;
			/* edge attr. is-reversed */
			rev = (char)el->edge->reversed;
			maingraph->nodenum++;
			/* create label node */
			add_new_dummynode(g, maingraph->nodenum);
			/* mark this is a label node and set label text */
			ln = uniqnode(maingraph, maingraph->nodenum);
			/* edge-label-node, original from/to node */
			ln->el_fnode = el->edge->from_node;
			ln->el_tnode = el->edge->to_node;
			/* y level difference between original from/to-nodes */
			ydiff = (ln->el_tnode->rely - ln->el_fnode->rely);
			/* put edge label halfway */
			ln->rely = ln->el_fnode->rely + (ydiff / 2);
			ln->elabel = 1;	/* mark this is a edgelabel */
			ln->dummy = 0;
			/* set in the edgelabel node the number of the orig. edge */
			ln->enumber = enumber;
			/* set the size of the edge label text in the new node */
			ln->tx = el->edge->tx;
			ln->ty = el->edge->ty;
			/* node belongs to graph with this startnode */
			ln->startnode = el->edge->from_node->startnode;
			/* create new edges with label node in between */
			add_new_dummyedge(g, el->edge->from_node->nr, maingraph->nodenum, rev);
			add_new_dummyedge(g, maingraph->nodenum, el->edge->to_node->nr, rev);
			/* free old edge */
			del_edge(g, el);
		}

		el = elnext;
	}

	/* refresh st lists */
	clear_stlist_all(g);
	make_stlist(g);

	return;
}

/* split longer edges */
static void splitedges(struct gml_graph* g)
{
	struct gml_elist* el = NULL;
	struct gml_elist* elnext = NULL;
	struct gml_edge* edge = NULL;
	struct gml_node* sn = NULL;
	struct gml_node* tn = NULL;
	struct gml_node* nlnode = NULL;
	int edgelen = 0;
	int prevnodeid = 0;
	int newid = 0;
	int i = 0;
	int sny = 0;
	char rev = 0;
	el = g->edgelist;
	while (el)
	{
		elnext = el->next;
		edge = el->edge;
		sn = edge->from_node;	/* from-node */
		tn = edge->to_node;	/* to-node */
		rev = (char)edge->reversed;	/* edge attr. to copy when splitting edge */
		edgelen = (tn->rely - sn->rely);
		/* horizontal edge */
		if (edgelen == 0)
		{
			/* horizontal edge has original endpoints, used in drawing edges */
			edge->hedge = 1;
			g->nhedges++;	/* number of horizontal edges */
			/* mark that nodes have a hor. edge */
			sn->hashedge = 1;
			tn->hashedge = 1;
		}
		else if (edgelen > 1)
		{

			prevnodeid = sn->nr;
			sny = sn->rely;
			for (i = 1; i < edgelen; i++)
			{
				/* dummy node numbers start at first free node nr number */
				maingraph->nodenum++;
				newid = maingraph->nodenum;
				add_new_dummynode(maingraph, newid);
				nlnode = uniqnode(maingraph, newid);
				nlnode->dummy = 1;	/* this is a dummy node */
				nlnode->elabel = 0;	/* not a edgelabel */
				nlnode->rely = (sny + i);
				nlnode->startnode = sn->startnode;
				add_new_dummyedge(g, prevnodeid, newid, rev);
				prevnodeid = newid;
			}

			add_new_dummyedge(g, prevnodeid, tn->nr, rev);
			del_edge(g, el);
		}
		else if (edgelen == 1)
		{
			/* edge len is 1 oke. */
		}
		else
		{
			/* shouldnothappen */
		}

		el = elnext;
	}

	return;
}

/* create level node count data */
static void nodecounts(struct gml_graph* g)
{
	struct gml_nlist* lnll = NULL;
	/* refresh st lists */
	clear_stlist_all(g);
	make_stlist(g);
	/* table with number of nodes per level */
	g->nnodes_of_level = (int*)calloc((g->maxlevel + 1), sizeof(int));
	if (g->nnodes_of_level == NULL)
	{
		return;
	}

	/* determine widest level and how many nodes it has */
	g->widestlevel = 0;
	g->widestnnodes = 0;
	lnll = g->nodelist;
	while (lnll)
	{
		/* rely used for sugi */
		g->nnodes_of_level[lnll->node->rely] = g->nnodes_of_level[lnll->node->rely] + 1;
		/* x used for sugi, offset 1...n */
		lnll->node->relx = g->nnodes_of_level[lnll->node->rely];
		if (g->nnodes_of_level[lnll->node->rely] >= g->widestnnodes)
		{
			g->widestnnodes = g->nnodes_of_level[lnll->node->rely];
			g->widestlevel = lnll->node->rely;
		}
		lnll = lnll->next;
	}

	return;
}

struct mmatrix
{
	int level;		/* upper level */
	int nrows;		/* nr. of rows (in upper level) */
	int ncols;		/* nr. of cols (in level+1) */
	int nbytes;		/* bytes used for matrix */
	int* mi0;		/* row node id's level i */
	int nmi0;		/* how many byte's in mi0 */
	int* m0i;		/* col node id's level (i+1) */
	int nm0i;		/* how many bytes in m0i */
	int bbytes;		/* bytes for double barycenter values */
	double* b;		/* buffer barycenter values */
	unsigned char* bits;	/* matrix bits */
};

static inline void setbit(unsigned char a[], int k)
{
	if (k == 0)
	{
	}
	else
	{
		a[k / CHAR_BIT] |= (1 << (k % CHAR_BIT));
	}
	return;
}

static inline void clearbit(unsigned char a[], int k)
{
	if (k == 0)
	{
	}
	else
	{
		a[k / CHAR_BIT] &= ~(1 << (k % CHAR_BIT));
	}
	return;
}

static inline int testbit(struct mmatrix* m, unsigned char a[], int k)
{
	int ret = 0;
	unsigned int mask = 0;
	unsigned int mask2 = 0;
	unsigned int i = 0;

	if (k == 0)
	{
	}

	/* todo here tofix */
	if (k > ((m->ncols + 1) * (m->nrows + 1)))
	{
	}

	/* compiler issue: the use of << is undefined here */
	/* issue CHAR_BIT is often 8 but does not have to be 8 */
	mask = (k % CHAR_BIT);

	/*old:  mask2 = (1 << mask); */
	mask2 = 1;
	for (i = 0; i < mask; i++)
	{
		mask2 = (mask2 * 2);
	}
	ret = ((a[k / CHAR_BIT]) & mask2);
	/*old  return ((a[k / CHAR_BIT] & (1 << (k % CHAR_BIT))) != 0); */
	return (ret);
}

/* i cols, j rows */
static inline int mget(struct mmatrix* m, int i, int j)
{
	return (testbit(m, m->bits, ((i * (m->ncols + 0)) + j)));
}

/* i is the from-node, j is the to-node, value is 1 for edge, otherwise 0 */
static inline void mget_set(struct mmatrix* m, int i, int j, int value)
{
	if (value)
	{
		setbit(m->bits, ((i * (m->ncols + 0)) + j));
	}
	else
	{
		clearbit(m->bits, ((i * (m->ncols + 0)) + j));
	}
	return;
}

static int number_of_crossings2(struct mmatrix* m, int r, int c)
{
	int j = 0;
	int k = 0;
	int alpha = 0;
	int beta = 1;
	int result = 0;

	for (j = 1; j <= r - 1; j++)
	{
		for (k = j + 1; k <= r; k++)
		{
			for (alpha = 1; alpha <= c - 1; alpha++)
			{
				for (beta = alpha + 1; beta <= c; beta++)
				{
					/* here 1*0=0, 0*1=0, 0*0=0 or 1*1=1 */
					result = result + ((mget(m, j, beta) * mget(m, k, alpha)));
				}
			}
		}
	}
	return (result);
}

static int number_of_crossings3(struct mmatrix* m, int r, int c)
{
	int j = 0;
	int k = 0;
	int alpha = 0;
	int beta = 1;
	int result2 = 0;

	if (0)
	{
		result2 = number_of_crossings2(m, r, c);
	}

	for (j = 1; j <= (r - 1); j++)
	{
		for (k = (j + 1); k <= r; k++)
		{
			for (alpha = 1; alpha <= (c - 1); alpha++)
			{

				/* */
				if (mget(m, k, alpha))
				{
					for (beta = alpha + 1; beta <= c; beta++)
					{
						/* */
						if (mget(m, j, beta))
						{
							result2++;
						}
					}
				}
				/* */

			}
		}
	}

	return (result2);
}

/* number of crossings in whole graph */
static int number_of_crossings_a(struct gml_graph* g, struct mmatrix** mm)
{
	int ktot = 0;
	int k = 0;
	int i = 0;

	for (i = 0; i < g->maxlevel; i++)
	{
		if (mm[i])
		{
			k = number_of_crossings3(mm[i], mm[i]->nrows, mm[i]->ncols);
			/* save number of edge crossings at level */
			g->numce[i] = k;
			ktot = ktot + k;
		}
	}

	return (ktot);
}

/* configure matrix data for level l in the graph */
static void make_matrix(struct gml_graph* g, int l, struct mmatrix* m)
{
	struct gml_nlist* nl = NULL;
	struct gml_elist* el = NULL;
	int i = 0;
	int j = 0;
	int c = 0;
	int r = 0;

	/* add node numbers in the 0 position */

	nl = g->nodelist;

	while (nl)
	{
		/* if (level(n) == l) */
		if (nl->node->rely == l)
		{
			/* rows */
			i = nl->node->relx;
			m->mi0[i] = nl->node->nr;	/* uniq node number, not id */
		}
		else if (nl->node->rely == (l + 1))
		{
			/* columns */
			j = nl->node->relx;
			m->m0i[j] = nl->node->nr;	/* uniq node number, not id */
		}
		nl = nl->next;
	}

	/* matrix bits config, first init then set bits. */
	r = m->nrows;
	c = m->ncols;

	for (i = 1; i <= r; i++)
	{
		for (j = 1; j <= c; j++)
		{
			mget_set(m, i, j, 0);
		}
	}

	nl = g->nodelist;

	while (nl)
	{
		/* if (level(n) == l) */
		if (nl->node->rely == l)
		{
			/* outgoing edges : for_sourcelist (n, e) */
			el = nl->node->outgoing_e;
			while (el)
			{
				/* skip the horizontal edges */
				if (el->edge->hedge == 0)
				{
					/* from-node rel. x pos */
					i = nl->node->relx;
					/* to-node rel. x pos */
					j = el->edge->to_node->relx;
					/* set this is edge */
					mget_set(m, i, j, 1);
				}
				el = el->next;
			}
		}
		nl = nl->next;
	}

	return;
}

/* find node with given id number */
static struct gml_node* su_find_node_with_number(struct gml_graph* g, int nr)
{
	return (uniqnode(g, nr));
}

static void store_new_positions(struct gml_graph* g, struct mmatrix* m, int level)
{
	struct gml_node* n = NULL;
	int i = 0;

	if (level)
	{
	}

	if (m == NULL)
	{
		return;
	}

	for (i = 1; i <= m->nrows; i++)
	{
		/* rows */
		n = su_find_node_with_number(g, m->mi0[i]);
		if (n)
		{
			/* offset is 1, make it into 0..n */
			n->relx = (i - 1);
		}
		else
		{
		}
	}

	for (i = 1; i <= m->ncols; i++)
	{
		/* columns */
		n = su_find_node_with_number(g, m->m0i[i]);
		if (n)
		{
			/* offset is 1, make it into 0..n */
			n->relx = (i - 1);
		}
		else
		{
		}
	}

	return;
}

/*  parts are Copyright (C) Felix von Leitner from dietlibc */
static void* do_memmove(void* dst, void* src, size_t count)
{
	char* a = (char*)dst;
	char* b = (char*)src;
	if (src != dst)
	{
		if (src > dst)
		{
			while (count--)
				*a++ = *b++;
		}
		else
		{
			a += count - 1;
			b += count - 1;
			while (count--)
				*a-- = *b--;
		}
	}
	return dst;
}

/* copy matrix m1 to m2 */
static void copy_m(struct mmatrix* m1, struct mmatrix* m2)
{
	if (m1 && m2)
	{
		m2->level = m1->level;	/* upper level */
		m2->nrows = m1->nrows;	/* nr. of rows (in upper level) */
		m2->ncols = m1->ncols;	/* nr. of cols (in level+1) */
		m2->nbytes = m1->nbytes;	/* bytes used for matrix */
		(void)do_memmove(m2->bits, m1->bits, m1->nbytes);	/* matrix bits */
		(void)do_memmove(m2->mi0, m1->mi0, m1->nmi0);	/* row node id's level i */
		m2->nmi0 = m1->nmi0;	/* how many byte's in mi0 */
		(void)do_memmove(m2->m0i, m1->m0i, m1->nm0i);	/* col node id's level (i+1) */
		m2->nm0i = m1->nm0i;	/* how many bytes in m0i */
		m2->bbytes = m1->bbytes;	/* bytes for double barycenter values */
		(void)do_memmove(m2->b, m1->b, m1->bbytes);	/* barycenter values */
	}
	return;
}

/* are m1,m2 equal? */
static int equal_m(struct mmatrix* m1, struct mmatrix* m2, int r, int c)
{
	int i = 0;
	int j = 0;

	for (i = 1; i <= r; i++)
	{
		for (j = 1; j <= c; j++)
		{
			if (mget(m1, i, j) != mget(m2, i, j))
			{
				return (0);	/* FALSE */
			}
		}
	}

	return (1);		/* TRUE */
}

/* is whole graph equal */
static int equal_a(struct gml_graph* g, struct mmatrix** mm1, struct mmatrix** mm2)
{
	int l = 0;

	if (mm1 == NULL || mm2 == NULL)
	{
		return (0);
	}

	for (l = 0; l < g->maxlevel; l++)
	{
		if (equal_m(mm1[l], mm2[l], mm1[l]->nrows /* old g->nnodes_of_level[l] */,
			mm1[l]->ncols /* old g->nnodes_of_level[l + 1] */) ==
			0 /* FALSE */)
		{
			return (0);	/* FALSE */
		}
	}

	return (1);		/* TRUE */
}

/* copy whole graph */
static inline void copy_a(struct gml_graph* g, struct mmatrix** mm1, struct mmatrix** mm2)
{
	int i = 0;

	for (i = 0; i < g->maxlevel; i++)
	{
		copy_m(mm1[i], mm2[i]);
	}

	return;
}

static void exch_rows(struct mmatrix* m, int r1, int r2)
{
	int j = 0;
	int id1 = 0;
	int id2 = 0;
	int bit1 = 0;
	int bit2 = 0;

	/*
	 * h = m[r1][j];
	 * m[r1][j] = m[r2][j];
	 * m[r2][j] = h;
	 */

	 /* swap node id numbers */
	id1 = m->mi0[r1];
	id2 = m->mi0[r2];
	m->mi0[r1] = id2;
	m->mi0[r2] = id1;

	/* swap matrix bits */
	for (j = 1; j <= m->ncols; j++)
	{
		bit1 = mget(m, r1, j);
		bit2 = mget(m, r2, j);
		mget_set(m, r1, j, bit2);
		mget_set(m, r2, j, bit1);
	}

	return;
}

static void exch_columns(struct mmatrix* m, int c1, int c2)
{
	int i = 0;
	int id1 = 0;
	int id2 = 0;
	int bit1 = 0;
	int bit2 = 0;

	/*
	 * h = m[i][c1];
	 * m[i][c1] = m[i][c2];
	 * m[i][c2] = h;
	 */

	 /* swap node id numbers */
	id1 = m->m0i[c1];
	id2 = m->m0i[c2];
	m->m0i[c1] = id2;
	m->m0i[c2] = id1;

	/* swap matrix bits */
	for (i = 1; i <= m->nrows; i++)
	{
		bit1 = mget(m, i, c1);
		bit2 = mget(m, i, c2);
		mget_set(m, i, c1, bit2);
		mget_set(m, i, c2, bit1);
	}

	return;
}

static int reverse_r(struct mmatrix* m, int r1, int r2)
{
	int i = 0;
	int j = 0;
	int ch = 0;

	for (i = r1, j = r2; i < j; i++, j--)
	{
		ch++;
		exch_rows(m, i, j);
	}

	return (ch);
}

static int reverse_c(struct mmatrix* m, int c1, int c2)
{
	int i = 0;
	int j = 0;
	int ch = 0;

	for (i = c1, j = c2; i < j; i++, j--)
	{
		ch++;
		exch_columns(m, i, j);
	}

	return (ch);
}

static double row_barycenter(struct mmatrix* m, int i, int maxval)
{
	int j = 0;
	int r1 = 0;		/* sum */
	int r2 = 0;		/* div */

	for (j = 1; j <= maxval; j++)
	{
		if (mget(m, i, j))
		{
			r1 = r1 + j;
			r2++;
		}
	}

	if (r2 == 0)
	{
		return (0.0);
	}
	else
	{
		return ((double)r1 / (double)r2);
	}
}

static double column_barycenter(struct mmatrix* m, int j, int maxval)
{
	int i = 0;
	int r1 = 0;		/* sum */
	int r2 = 0;		/* div */

	for (i = 1; i <= maxval; i++)
	{
		if (mget(m, i, j))
		{
			r1 = r1 + i;
			r2++;
		}
	}

	if (r2 == 0)
	{
		return (0.0);
	}
	else
	{
		return ((double)r1 / (double)r2);
	}
}

/* reverse rows */
static int r_r(struct mmatrix* m1, struct mmatrix* m2, int max_r, int max_c)
{
	int i = 0;
	int j = 0;
	int ch = 0;

	for (i = 1; i <= max_r; i++)
	{
		m1->b[i] = row_barycenter(m1, i, max_c);
	}

	for (i = 1; i < max_r; i++)
	{
		j = i;

		while ((j < max_r) && (m1->b[j + 1] == m1->b[j]))
		{
			j++;
		}

		if (j > i)
		{
			ch += reverse_r(m1, i, j);

			if (m2 != NULL)
			{
				ch += reverse_c(m2, i, j);
			}

			i = j;
		}

	}

	return (ch);
}

/* reverse columns */
static int r_c(struct mmatrix* m1, struct mmatrix* m2, int max_r, int max_c)
{
	int i = 0;
	int j = 0;
	int ch = 0;

	for (i = 1; i <= max_c; i++)
	{
		m1->b[i] = column_barycenter(m1, i, max_r);
	}

	for (i = 1; i < max_c; i++)
	{
		j = i;

		while ((j < max_c) && (m1->b[j + 1] == m1->b[j]))
		{
			j++;
		}

		if (j > i)
		{
			ch += reverse_c(m1, i, j);

			if (m2 != NULL)
			{
				ch += reverse_r(m2, i, j);
			}

			i = j;
		}

	}

	return (ch);
}

/* barycenter rows */
static int b_r(struct mmatrix* m1, struct mmatrix* m2, int max_r, int max_c)
{
	double tmpb = 0.0;
	int i = 0;
	int j = 0;
	int k = 0;
	int ch = 0;

	for (i = 1; i <= max_r; i++)
	{
		m1->b[i] = row_barycenter(m1, i, max_c);
	}

	for (j = max_r; j > 1; j--)
	{
		if (m1->b[j] != 0.0)
		{
			for (i = 1; i < j; i++)
			{
				if (m1->b[i] != 0.0)
				{
					k = i + 1;
					while (m1->b[k] == 0.0)
					{
						k++;
					}
					if (m1->b[i] > m1->b[k])
					{
						ch++;
						/* exch_double */
						tmpb = m1->b[k];
						m1->b[k] = m1->b[i];
						m1->b[i] = tmpb;
						exch_rows(m1, i, k);
						if (m2 != NULL)
						{
							ch++;
							/* exchange cols from i to k */
							exch_columns(m2, i, k);
						}
					}
				}
			}
		}
	}

	return (ch);
}

/* barycenter cols */
static int b_c(struct mmatrix* m1, struct mmatrix* m2, int max_r, int max_c)
{
	double tmpb = 0.0;
	int i = 0;
	int j = 0;
	int k = 0;
	int ch = 0;

	for (i = 1; i <= max_c; i++)
	{
		m1->b[i] = column_barycenter(m1, i, max_r);
	}

	for (j = max_c; j > 1; j--)
	{
		if (m1->b[j] != 0.0)
		{
			for (i = 1; i < j; i++)
			{
				if (m1->b[i] != 0.0)
				{
					k = i + 1;

					while (m1->b[k] == 0.0)
					{
						k++;
					}

					if (m1->b[i] > m1->b[k])
					{
						ch++;
						/* exch_double */
						tmpb = m1->b[k];
						m1->b[k] = m1->b[i];
						m1->b[i] = tmpb;
						/* exchange cols from i to k */
						exch_columns(m1, i, k);
						if (m2 != NULL)
						{
							ch++;
							exch_rows(m2, i, k);
						}
					}
				}
			}
		}
	}

	return (ch);
}

/* test if array is sorted, 1 if sorted from hight-to-low */
static int sorted(double* vector, int maxval)
{
	int i = 0;

	for (i = 1; i < maxval; i++)
	{
		/* but ignore 0.0 values */
		if ((vector[i] > vector[i + 1]) && (vector[i + 1] != 0.0))
		{
			return (0);	/* FALSE */
		}
	}

	return (1);		/* TRUE */
}

static inline int phase1_down(struct gml_graph* g, struct mmatrix** mm)
{
	int i = 0;
	int ch = 0;

	/* from level0 down to level max */
	for (i = 0; i < g->maxlevel - 1; i++)
	{
		ch += b_c(mm[i], mm[i + 1], mm[i]->nrows, mm[i]->ncols);
	}

	ch += b_c(mm[g->maxlevel - 1], NULL, mm[g->maxlevel - 1]->nrows, mm[g->maxlevel - 1]->ncols);

	return (ch);
}

static inline int phase1_up(struct gml_graph* g, struct mmatrix** mm)
{
	int i = 0;
	int ch = 0;

	if (mm == NULL)
	{
		return (0);
	}

	/* from level max up to level0 */
	for (i = (g->maxlevel - 1); i > 0; i--)
	{
		ch += b_r(mm[i], mm[i - 1], mm[i]->nrows /* old g->nnodes_of_level[i] */,
			mm[i]->ncols /* old g->nnodes_of_level[i + 1] */);
	}

	ch += b_r(mm[0], NULL, mm[0]->nrows /* g->nnodes_of_level[0] */,
		mm[0]->ncols /* g->nnodes_of_level[1] */);

	return (ch);
}

/* */
static inline int phase2_down(struct gml_graph* g, struct mmatrix** mm)
{
	int l = 0;		/* Level */
	int i = 0;
	int ch = 0;

	for (l = 0; l < (g->maxlevel - 1); l++)
	{

		for (i = 1; i <= mm[l]->ncols /* g->nnodes_of_level[l + 1] */; i++)
		{
			mm[l]->b[i] = column_barycenter(mm[l], i, mm[l]->nrows /* g->nnodes_of_level[l] */);
		}

		if (sorted(mm[l]->b, mm[l]->ncols /* g->nnodes_of_level[l + 1] */) ==
			1 /* TRUE */)
		{
			/* reverse columns */
			ch += r_c(mm[l], mm[l + 1], mm[l]->nrows /* g->nnodes_of_level[l] */,
				mm[l]->ncols /* g->nnodes_of_level[l + 1] */);
		}
		else
		{
			return (ch);
		}

	}

	for (i = 1; i <= g->nnodes_of_level[g->maxlevel]; i++)
	{
		mm[g->maxlevel - 1]->b[i] = column_barycenter(mm[g->maxlevel - 1], i, mm[g->maxlevel - 1]->nrows
		/* g->nnodes_of_level[g->maxlevel - 1] */);
	}

	if (sorted(mm[g->maxlevel - 1]->b, mm[g->maxlevel - 1]->ncols /* g->nnodes_of_level[g->maxlevel] */) ==
		1 /* TRUE */)
	{
		/* reverse columns */
		ch += r_c(mm[g->maxlevel - 1], NULL, mm[g->maxlevel - 1]->nrows /* g->nnodes_of_level[g->maxlevel - 1] */,
			mm[g->maxlevel - 1]->ncols /* g->nnodes_of_level[g->maxlevel] */);
	}

	return (ch);
}

/* */
static inline int phase2_up(struct gml_graph* g, struct mmatrix** mm)
{
	int l = 0;		/* Level */
	int i = 0;
	int ch = 0;

	if (g)
	{
	}

	for (l = (g->maxlevel - 1); l > 0; l--)
	{

		for (i = 1; i <= /* g->nnodes_of_level[l] */ mm[l]->nrows; i++)
		{
			mm[l]->b[i] = row_barycenter(mm[l], i,	/* g->nnodes_of_level[l + 1] */
				mm[l]->ncols);
		}

		if (sorted(mm[l]->b, /* g->nnodes_of_level[l] */ mm[l]->nrows) ==
			1 /* TRUE */)
		{
			/* reverse rows */
			ch += r_r(mm[l], mm[l - 1], mm[l]->nrows /* g->nnodes_of_level[l] */,
				mm[l]->ncols /* g->nnodes_of_level[l + 1] */);
		}
		else
		{
			return (ch);
		}

	}

	for (i = 1; i <= mm[0]->nrows /* g->nnodes_of_level[0] */; i++)
	{
		mm[0]->b[i] = row_barycenter(mm[0], i, mm[0]->ncols /* g->nodes_of_level[1] */);
	}

	/* if level0 is sorted, r_r */
	if (sorted(mm[0]->b, mm[0]->nrows /* g->nnodes_of_level[0] */) ==
		1 /* TRUE */)
	{
		/* reverse rows */
		ch += r_r(mm[0], NULL, mm[0]->nrows /* g->nnodes_of_level[0] */,
			mm[0]->ncols /* g->nnodes_of_level[1] */);
	}

	return (ch);
}

/* here if maxlevel >1 */
static void bc_n(struct gml_graph* g, int it1value, int it2value)
{
	struct mmatrix** a = NULL;
	struct mmatrix** a1 = NULL;
	struct mmatrix** a2 = NULL;
	struct mmatrix** as = NULL;
	int i = 0;
	int ks = 0;
	int k = 0;
	int n1 = 0;
	int n2 = 0;
	int cht = 0;
	int ch1 = 0;
	int ch2 = 0;
	int r1 = 0;
	int r2 = 0;
	int r3 = 0;
	int rr1 = 0;
	int rr2 = 0;
	int rr3 = 0;
	int it1 = 40;		/* iterations phase1 */
	int it2 = 80;		/* iterations pahse2 */

	if (it1value == 0)
	{
		it1 = 20;
	}
	else
	{
		it1 = it1value;
	}

	if (it2value == 0)
	{
		it2 = 40;
	}
	else
	{
		it2 = it2value;
	}

	/* the whole graph structures */
	a = (mmatrix**)calloc(1, g->maxlevel * sizeof(struct mmatrix*));
	a1 = (mmatrix**)calloc(1, g->maxlevel * sizeof(struct mmatrix*));
	a2 = (mmatrix**)calloc(1, g->maxlevel * sizeof(struct mmatrix*));
	as = (mmatrix**)calloc(1, g->maxlevel * sizeof(struct mmatrix*));

	/* get matrix struct */
	for (i = 0; i < g->maxlevel; i++)
	{
		a[i] = (mmatrix*)calloc(1, sizeof(struct mmatrix));
		a1[i] = (mmatrix*)calloc(1, sizeof(struct mmatrix));
		a2[i] = (mmatrix*)calloc(1, sizeof(struct mmatrix));
		as[i] = (mmatrix*)calloc(1, sizeof(struct mmatrix));
	}

	/* get data inside struct */
	for (i = 0; i < g->maxlevel; i++)
	{
		/* upper level */
		a[i]->level = i;
		a1[i]->level = i;
		a2[i]->level = i;
		as[i]->level = i;

		/* number of rows */
		a[i]->nrows = g->nnodes_of_level[i];
		a1[i]->nrows = g->nnodes_of_level[i];
		a2[i]->nrows = g->nnodes_of_level[i];
		as[i]->nrows = g->nnodes_of_level[i];

		/* number of columns */
		a[i]->ncols = g->nnodes_of_level[i + 1];
		a1[i]->ncols = g->nnodes_of_level[i + 1];
		a2[i]->ncols = g->nnodes_of_level[i + 1];
		as[i]->ncols = g->nnodes_of_level[i + 1];

		/* buffer for barycenter values */
		if (a[i]->nrows > a[i]->ncols)
		{
			a[i]->bbytes = ((a[i]->nrows + 1) * sizeof(double));
			a1[i]->bbytes = ((a1[i]->nrows + 1) * sizeof(double));
			a2[i]->bbytes = ((a2[i]->nrows + 1) * sizeof(double));
			as[i]->bbytes = ((as[i]->nrows + 1) * sizeof(double));
		}
		else
		{
			a[i]->bbytes = ((a[i]->ncols + 1) * sizeof(double));
			a1[i]->bbytes = ((a1[i]->ncols + 1) * sizeof(double));
			a2[i]->bbytes = ((a2[i]->ncols + 1) * sizeof(double));
			as[i]->bbytes = ((as[i]->ncols + 1) * sizeof(double));
		}

		a[i]->b = (double*)calloc(1, a[i]->bbytes);
		a1[i]->b = (double*)calloc(1, a1[i]->bbytes);
		a2[i]->b = (double*)calloc(1, a2[i]->bbytes);
		as[i]->b = (double*)calloc(1, as[i]->bbytes);

		/* number of bytes used */
		a[i]->nmi0 = ((a[i]->nrows + 1) * sizeof(int));
		a1[i]->nmi0 = ((a[i]->nrows + 1) * sizeof(int));
		a2[i]->nmi0 = ((a[i]->nrows + 1) * sizeof(int));
		as[i]->nmi0 = ((a[i]->nrows + 1) * sizeof(int));

		/* row node id's (int's) */
		a[i]->mi0 = (int*)calloc(1, a[i]->nmi0);
		a1[i]->mi0 = (int*)calloc(1, a1[i]->nmi0);
		a2[i]->mi0 = (int*)calloc(1, a2[i]->nmi0);
		as[i]->mi0 = (int*)calloc(1, as[i]->nmi0);

		/* number of bytes used */
		a[i]->nm0i = ((a[i]->ncols + 1) * sizeof(int));
		a1[i]->nm0i = ((a[i]->ncols + 1) * sizeof(int));
		a2[i]->nm0i = ((a[i]->ncols + 1) * sizeof(int));
		as[i]->nm0i = ((a[i]->ncols + 1) * sizeof(int));

		/* col node id's (int's) */
		a[i]->m0i = (int*)calloc(1, a[i]->nm0i);
		a1[i]->m0i = (int*)calloc(1, a1[i]->nm0i);
		a2[i]->m0i = (int*)calloc(1, a2[i]->nm0i);
		as[i]->m0i = (int*)calloc(1, as[i]->nm0i);

		/* bits array for the matrix */
		a[i]->nbytes = 1 + ((((a[i]->nrows + 1) * (a[i]->ncols + 1)) + CHAR_BIT) / CHAR_BIT);
		a1[i]->nbytes = 1 + ((((a1[i]->nrows + 1) * (a1[i]->ncols + 1)) + CHAR_BIT) / CHAR_BIT);
		a2[i]->nbytes = 1 + ((((a2[i]->nrows + 1) * (a2[i]->ncols + 1)) + CHAR_BIT) / CHAR_BIT);
		as[i]->nbytes = 1 + ((((as[i]->nrows + 1) * (as[i]->ncols + 1)) + CHAR_BIT) / CHAR_BIT);

		a[i]->bits = (unsigned char*)calloc(1, a[i]->nbytes);
		a1[i]->bits = (unsigned char*)calloc(1, a1[i]->nbytes);
		a2[i]->bits = (unsigned char*)calloc(1, a2[i]->nbytes);
		as[i]->bits = (unsigned char*)calloc(1, as[i]->nbytes);
	}

	/* fill the matrix with data for all levels */
	for (i = 0; i < g->maxlevel; i++)
	{
		make_matrix(g, i, a[i]);
	}

	copy_a(g, a, as);
	ks = number_of_crossings_a(g, as);

	g->sugi_icrossings = ks;	/* sugiyama initial crossings */

	if (ks > 0)
	{

		/* Phase1 */
		ch1 = 0;

		/* first does alwasy improve */
		ch1 += phase1_down(g, a);
		copy_a(g, a, as);
		ch1 += phase1_up(g, a);
		copy_a(g, a, as);

		/* loop phase1 */
		n1 = 0;
		do
		{

			copy_a(g, a, a1);

			ch1 += phase1_down(g, a);
			k = number_of_crossings_a(g, a);

			if (k < ks)
			{
				/* reduced crossings, save the new state */
				ks = k;
				copy_a(g, a, as);
			}

			ch1 += phase1_up(g, a);

			k = number_of_crossings_a(g, a);

			if (k < ks)
			{
				ks = k;
				copy_a(g, a, as);

			}
			cht += ch1;

			if (ks == 0)
			{
				break;
			}

			/* check if number of crossings changed */
			r1 = r2;
			r2 = r3;
			r3 = ks;
			if (r1 == r2)
			{
				if (r2 == r3)
				{
					break;
				}
			}

		} while (++n1 < it1 && (equal_a(g, a, a1) == 0 /* FALSE */));

		/* if matrices differ, save state */
		if (equal_a(g, a, as) == 0 /* FALSE */)
		{
			copy_a(g, as, a);
		}

		if (ks > 0)
		{
			/* Phase2 */
			n2 = 0;
			cht += ch1;

			do
			{
				ch2 = 0;

				copy_a(g, a, a2);
				ch2 += phase2_down(g, a);
				n1 = 0;

				do
				{
					ch1 = 0;
					copy_a(g, a, a1);
					ch1 += phase1_down(g, a);
					k = number_of_crossings_a(g, a);
					if (k < ks)
					{
						ks = k;
						copy_a(g, a, as);
					}

					ch1 += phase1_up(g, a);
					k = number_of_crossings_a(g, a);
					if (k < ks)
					{
						ks = k;
						copy_a(g, a, as);
					}

					if (ks == 0)
					{
						break;
					}

					/* check if number of crossings changed */
					rr1 = rr2;
					rr2 = rr3;
					rr3 = ks;
					if (rr1 == rr2)
					{
						if (rr2 == rr3)
						{
							break;
						}
					}

				} while (++n1 < it1 && equal_a(g, a, a1) == 0 /* FALSE */);

				ch2 += phase2_up(g, a);

				n1 = 0;
				do
				{
					copy_a(g, a, a1);
					ch1 += phase1_up(g, a);
					k = number_of_crossings_a(g, a);
					if (k < ks)
					{
						ks = k;
						copy_a(g, a, as);
					}

					ch1 += phase1_down(g, a);
					k = number_of_crossings_a(g, a);
					if (k < ks)
					{
						ks = k;
						copy_a(g, a, as);
					}

					cht += ch1;

					if (ks == 0)
					{
						break;
					}

					/* check if number of crossings changed */
					rr1 = rr2;
					rr2 = rr3;
					rr3 = ks;
					if (rr1 == rr2)
					{
						if (rr2 == rr3)
						{
							break;
						}
					}

				} while (++n1 < it1 && equal_a(g, a, a1) == 0 /* FALSE */);

				cht += ch1;
				cht += ch2;

				if (ks == 0)
				{
					break;
				}

				/* check if number of crossings changed */
				r1 = r2;
				r2 = r3;
				r3 = ks;
				if (r1 == r2)
				{
					if (r2 == r3)
					{
						break;
					}
				}

			} while (++n2 < it2 && equal_a(g, a, a2) == 0 /* FALSE */);
		}
	}

	/* sugiyama final crossings */
	g->sugi_fcrossings = ks;

	/* sugiyama changes made */
	g->sugi_changes = cht;

	for (i = 0; i < g->maxlevel; i += 2)
	{
		/* set new node positions for 2 levels */
		store_new_positions(g, as[i], i);
	}

	if (i == g->maxlevel)
	{
		store_new_positions(g, as[g->maxlevel - 1], (g->maxlevel - 1));
	}

	for (i = 0; i < g->maxlevel; i++)
	{
		if (a[i])
		{
			free(a[i]->b);
			free(a[i]->mi0);
			free(a[i]->m0i);
			free(a[i]->bits);
		}
		if (a1[i])
		{
			free(a1[i]->b);
			free(a1[i]->mi0);
			free(a1[i]->m0i);
			free(a1[i]->bits);
		}
		if (a2[i])
		{
			free(a2[i]->b);
			free(a2[i]->mi0);
			free(a2[i]->m0i);
			free(a2[i]->bits);
		}
		if (as[i])
		{
			free(as[i]->b);

			free(as[i]->mi0);

			free(as[i]->m0i);

			free(as[i]->bits);
		}
	}

	for (i = 0; i < g->maxlevel; i++)
	{
		free(a[i]);
		free(a1[i]);
		free(a2[i]);
		free(as[i]);
	}

	free(a);
	free(a1);
	free(a2);
	free(as);

	return;
}

/*
This algorithm is for routing hierarchies of elements.  A "good route" is
one that has a minimum number of link crossings.  An algorithm that was
truly optimal (for minimizing link crossings) would be combinatorial and
therefore cost prohibitive; therefore, this algorithm uses a heuristic
approach that finds a route with close to the minimum number of crossings
in a reasonable amount of time.
This algorithm assumes that all the elements form a directed acyclic graph
(DAG), which means (1) that links flow one way between elements and (2) for
any given node there is no way to get back to the node if, starting at the
node, you traverse the links going from node to node.  This algorithm also
assumes that AT MOST only ONE link may exist between a pair of nodes.
-------------------------------------------------------------------------------
OVERVIEW OF ALGORITHM
All elements that do not have any parents are placed in the first row (row 0).
Elements are assigned to rows, where the row number for each child is equal to
the [maximum(row number of all its parents) + 1].  Crossovers are determined
by examining links between elements on adjacent rows, so if a parent is in a
row that is not adjacent to its child's row, "dummy" nodes are created on the
rows in between the parent and child, and the parent and child are connected
via these dummy nodes.
Once the elements (now called nodes) are assigned to individual rows, the
rows are sorted (repeatedly) in order to minimize link crossings.  The
sort criteria involves attempting to both center children under parents and
to center parents over children.  The sort orders are then tweaked by swapping
nodes that have the same sort value.
After the column orders are finalized, the nodes are spread out so they are
more or less centered above their children and below their parents.  When
centering children below parents, a row of children is sorted by which node
has the greatest number of parents.  These get first choice of where to be
placed under the parents (actually, dummy nodes get first preference, then
all of the others).  Centering parents above children is analogous.
When done with node placement, there may be some empty columns, and the
numbering scheme may not start at 0.  Therefore, the empty columns must
be eliminated and every node needs its column renumbered, starting at 0.
Then you are done.
-------------------------------------------------------------------------------
REALIZATION MATRIX
When it comes to both sorting nodes and horizontally spacing the nodes, two
adjacent rows are always involved.  For example, if we are sorting row[i]
based on the children of row[i]'s nodes, then row[i+1] is also involved
at this step.  These two rows are called the "i-th realization", and form
the "i-th realization matrix".  A realization matrix shows the parent-child
relationships between adjacent rows, with the parents on the rows and the
children on the columns.  If there is a parent-child relationship, a 1 is
stored in the matrix at the position, else a 0 is stored.
An example:
							A     B     C  D
							\ \  / \  / /  |
							 \ /\  / \ /   |
							 /\ / \  / \   |
							/ /\   /\    \ |
						   //   \ /   \   \|
						  E      F     G   H
   E  F  G  H
A  0  1  1  0    In this example, parent 'A' has children 'F' and 'G',
B  1  0  0  1                     parent 'B' has children 'E' and 'H',
C  1  1  0  0                     parent 'C' has children 'E' and 'F',
D  0  0  0  1                 and parent 'D' has child 'H'.
-------------------------------------------------------------------------------
ROW AND COLUMN BARYCENTERS
Two other important concepts are the "row barycenter" and the "column
barycenter" for a node.  The "row barycenter" is the basically the average
of the positions of a node's children.  The "column barycenter" is the average
of the positions of a node's parents.  These numbers tell us where a node
would like to be positioned in its row, depending whether we are positioning
relative to children or parents.
For example, using the above realization matrix, we can calculate the row
barycenters for A, B, C, and D, and the column barycenters for E, F, G, and H.
Since the row barycenter of a node is equal to the sum of the positions of
the node's children divided by the number of children of the node, the row
barycenter for A is (1 + 2)/2 = 1.5.  This assumes that we start numbering
rows and columns at 0.  Similarly, the column barycenter of a node is equal
to the sum of the positions of the node's parents divided by the number of
parents of the node.  So, the column barycenter of F is (0 + 2)/2 = 1.0.
The complete example is as follows:
							 Row
	  |  E    F    G    H  | Barys
------+--------------------+-----
A     |  0    1    1    0  | 1.5
B     |  1    0    0    1  | 1.5
C     |  1    1    0    0  | 0.5
D     |  0    0    0    1  | 3.0
------+--------------------+-----
Col   | 1.5  1.0  0.0  2.0 |
Barys |                    |
If we were to sort the child nodes here by their column barycenters, the new
order would be G, F, E, H.  If we were to sort the parent nodes here by their
row barycenters, then the order would be C, A, B, D (if two or more nodes have
the same value, be sure to keep the same precedence that already exists
between them, e.g., make sure that order after sorting is not C, B, A, D).
If a node has no parents then it can't have a column barycenter.  This case
should never happen, as all nodes that have no parents should be in the first
level of the hierarchy, and these nodes would only be represented in
realization matrix 0, and they would only have row barycenters.
If a node has no children then it can't have a row barycenter.  In this case,
while sorting based on row barycenters, sort AROUND these nodes, i.e., do
not change their positions at all.  For example, if we had the following:
							 Row
	  |  W    X    Y    Z  | Barys
------+--------------------+-----
Q     |  0    1    1    1  | 2.0
R     |  0    0    0    0  | ???
S     |  1    0    0    0  | 0.0
T     |  0    1    0    1  | 2.0
------+--------------------+-----
Col   | 2.0  1.5  0.0  1.5 |
Barys |                    |
and we were to sort by row barycenters, the resulting order should be S, R,
Q, T.  Notice how R stayed in its position, and even though Q and T had the
same barycentric value, Q stayed before T.
The whole reason for sorting rows and columns by their barycenters is to
decrease the number of crossovers.
-------------------------------------------------------------------------------
CROSSOVERS
The realization matrix is also used to count the number of crossovers between
two adjacent rows of nodes.  For each row, starting with the second row, if
a row element has a 1, then sum up all of the matrix elements that are above
AND to the right of this element.  Looking again at the first example:
							A     B     C  D
							\ \  / \  / /  |
							 \ /\  / \ /   |
							 /\ / \  / \   |
							/ /\   /\    \ |
						   //   \ /   \   \|
						  E      F     G   H
							 Row
	  |  E    F    G    H  | Barys
------+--------------------+-----
A     |  0    1    1    0  | 1.5
B     |  1    0    0    1  | 1.5
C     |  1    1    0    0  | 0.5
D     |  0    0    0    1  | 3.0
------+--------------------+-----
Col   | 1.5  1.0  0.0  2.0 |
Barys |                    |
Starting with the second row (parent B's row), position B-E has a 1.  Looking
at positions above and to the right, we see positions A-F and A-G both have
a 1, so the number of crossovers is currently = 2.  Position B-H has a 1, but
there are no elements above and to the right, so crossovers is still = 2.
For parent row of C, position C-E crosses over with B-H, A-F, and A-G, so
crossovers = 5.  C-F crosses over with B-H and A-G, so crossovers = 7.  For
parent row D, position D-H doesn't cross over with any other link.  So for
this realization matrix representing these two rows, the number of crossovers
is 7.
The total number of crossovers for the whole graph would be the sum of the
crossovers from each matrix realization.
-------------------------------------------------------------------------------
NODE CENTERING
After the nodes for each row have their final sort order, the nodes need to
be assigned to grid positions.  Their initial grid position will be their
column position, by which we mean their array position in the row.  From now
on, when we take a row or column barycenter, we will be using grid positions
instead of column positions.
Note:  The following examples will be based on centering children with respect
to their parents' positions.  Centering parents based on their children's
positions is analogous.
When positioning the nodes on a row based on their parents' positions, the
nodes must be initially sorted to see which nodes get first choice.  The dummy
nodes go first, and the rest of nodes are sorted in descending order based on
the number of parents the node has.  If a dummy node has a parent that has
multiple dummy nodes, all of these dummy nodes are again sorted by how close
to the center of the parent's children they are.  This is a confusing
statement, best illustrated by example:
			P1              P2
			 \              |
			   \  __________^__________
				 \|    |    |    |    |
				  C1   D1   D2   C2   D3
Here, parent P1 has one child, C1.  Parent P2 has five children, and three of
the child nodes are dummy nodes: D1, D2, and D3.  C1 is child 0 of P2, D1 is
child 1 of P2, D2 is child 2 of P2, C2 is child 3 of P2, and D3 is child 4 of
P2.  The child midpoint underneath the parent is equal to
(the number of children - 1) / 2, so (5 - 1) / 2 = 2.  Since the dummy nodes
go first, the initial order is D1, D2, D3, C1 (because it has 2 parents), and
finally C2.  All of the dummy nodes have the same parent, so we will sort them
based on how far away they are from the parent's child midpoint.  D1 is child
1 of P2, so it is 1 away.  D2 is child 2 of P2, so it is 0 away.  D3 is child
4 of P2, so it is 2 away.  Therefore, the final order for choosing positions
is D2, D1, D3, C1, C2.
In a situation similar to the dummy nodes, if a non-dummy node has a only one
parent, and that parent has other children with just one parent, then these
one parent child nodes that have the same parent need additional sorting in
the exact same manner that we just did the dummy nodes.
The whole purpose behind this is so that the left most node doesn't get first
choice.  If it did, we would get graphs that look like:
				 A                                     A
				 |                                     |
				 |_________       instead of      _____^_____
				 |    |    |                      |    |    |
				 B    C    D                      B    C    D
Anyway, once we have a sort order for the nodes of a row, we place the nodes
in their preferred positions.  Using the previous example, assume that P1
is in grid position 2 and P2 is in grid position 5.  D2 gets first choice,
and its column barycenter (based now on parent grid positions, not column
positions) is 5, so we place D2 in position 5.  D1 is next, its barycenter
is also 5.  We can't give it 5 since that position is occupied, so we give
it the closest possible position we can, which in this case is 4.  D3 is next,
and its barycenter is also 5.  The closest position that we can give it is
position 7, since we must allow room for C2.  C1 is next, and its barycenter
is (2 + 5)/2 = 3.5, which we round to 3.  Position 3 is open, so we go ahead
and give it position 3.  C2 is last, and its barycenter is 5.  However, the
first position available to it based on its left neighbor is position 6, so
we assign it position 6.
-------------------------------------------------------------------------------
GOING 'UP' OR 'DOWN' THE GRAPH
"Going down the graph" means taking each realization matrix situation,
starting with Realization Matrix 0, and performing some action on it, then
going to the next realization matrix, and continuing until all of the
realization matrices have been visited.
"Going up the graph" is analogous, except you start at the bottom with the
last realization matrix and work up to Realization Matrix 0.
*/

static void barycenter(struct gml_graph* g, int it1v, int it2v)
{

	/* number of crossing edges at level */
	if (g->numce == NULL)
	{
		g->numce = (int*)calloc(1, (g->maxlevel + 1) * sizeof(int));
	}

	if (g->maxlevel == 0)
	{
		/* if graph has only 1 or more nodes */
		return;
	}

	if (g->nnodes < 2)
	{
		return;
	}

	if (g->nedges < 2)
	{
		return;
	}

	bc_n(g, it1v, it2v);

	return;
}

/* min. distance between 2 nodes */
static int mindist = 1;

/* current startnode nr field */
static int csn = 0;

/* node list of part of graph */
static struct gml_nlist* cnodelist = NULL;
static struct gml_nlist* cnodelisttail = NULL;

/* number of nodes at level */
static int* cnnodes_of_level = NULL;

/* max. x,y in part of graph */
static int cmaxx = 0;
static int cmaxy = 0;

/* widest x level */
static int cwidestnnodes = 0;

/* x width at position */
static int* cwpos = NULL;

/* lists per pos. */
static struct gml_nlist** cposnodes = NULL;

/* hor pos */
static int* chpos = NULL;

/* hor pos */
static struct gml_nlist** clevelnodes = NULL;

/* (x,y) spacing */
static int xspacing = 0;
static int yspacing = 0;

/* */
struct node_data
{
	struct gml_node* node;
	int priority;
	int done;
};

/* nodes in one level */
static struct node_data* nl = NULL;

static int is_dummy(struct gml_node* node)
{
	if (node->dummy)
	{
		return (1);
	}
	else
	{
		return (0);
	}
}

/* how many connection edges from previous level */
static int upper_connectivity(struct gml_node* node)
{
	struct gml_elist* el = NULL;
	int result = 0;

	result = 0;

	if (node == NULL)
	{
		/* shouldnothappen */
		return (0);
	}

	/* incoming edges for_targetlist(node,edge) */
	el = node->incoming_e;

	while (el)
	{
		/* skip hor. edges */
		if (el->edge->hedge == 0)
		{
			/* only in this part of graph */
			if (el->edge->from_node->startnode == csn)
			{
				result++;
			}
		}
		el = el->next;
	}

	return (result);
}

/* how many connection edges to next level */
static int lower_connectivity(struct gml_node* node)
{
	struct gml_elist* el = NULL;
	int result = 0;

	result = 0;

	if (node == NULL)
	{
		/* shouldnothappen */
		return (0);
	}

	/* outgoing edges for_sourcelist(node,edge) */
	el = node->outgoing_e;

	while (el)
	{
		/* skip hor. edges */
		if (el->edge->hedge == 0)
		{
			/* only in this part of graph */
			if (el->edge->to_node->startnode == csn)
			{
				result++;
			}
		}
		el = el->next;
	}

	return (result);
}

/* simple floor() function */
static double do_floor(double num)
{
	double ret = 0.0;
	if (num < 0)
	{
		ret = (int)(num - 1);
	}
	else
	{
		ret = (int)num;
	}
	return (ret);
}

/* avg x pos of incoming edges */
static int upper_barycenter(struct gml_node* node)
{
	struct gml_elist* el = NULL;
	int result = 0;
	double r = 0.0;

	if (node == NULL)
	{
		/* shouldnothappen */
		return (0);
	}

	/* incoming edges x sum for_targetlist(node,edge) */
	el = node->incoming_e;

	while (el)
	{
		/* skip hor. edges */
		if (el->edge->hedge == 0)
		{
			/* only in this part of graph */
			if (el->edge->from_node->startnode == csn)
			{
				result += (el->edge->from_node->absx);
			}
		}
		el = el->next;
	}

	if (result == 0)
	{
		r = (0.0);
	}
	else
	{
		if (upper_connectivity(node) == 0)
		{
			r = 0.0;
		}
		else
		{
			r = (result / upper_connectivity(node));
		}
	}

	r = do_floor(r + 0.5);

	return ((int)r);
}

/* avg x pos of outgoing edges */
static int lower_barycenter(struct gml_node* node)
{
	struct gml_elist* el = NULL;
	int result = 0;
	double r = 0.0;

	if (node == NULL)
	{
		/* shouldnothappen */
		return (0);
	}

	/* get avg. x pos of outgoing edges for_sourcelist(node,edge) */
	el = node->outgoing_e;

	while (el)
	{
		/* skip hor. edges */
		if (el->edge->hedge == 0)
		{
			/* only in this part of graph */
			if (el->edge->to_node->startnode == csn)
			{
				result += (el->edge->to_node->absx);
			}
		}
		el = el->next;
	}

	if (result == 0)
	{
		r = (0.0);
	}
	else
	{
		if (lower_connectivity(node) == 0)
		{
			r = 0.0;
		}
		else
		{
			r = (result / lower_connectivity(node));
		}
	}

	r = do_floor(r + 0.5);

	return ((int)r);
}

static void sort(int n)
{
	int i = 0;
	int j = 0;
	struct node_data h;

	for (j = n - 1; j > 0; j--)
	{
		for (i = 0; i < j; i++)
		{
			/* issue here */
			if (nl[i].node && nl[i + 1].node)
			{
				if (nl[i].node->relx > nl[i + 1].node->relx)
				{
					/* swap */
					h = nl[i];
					nl[i] = nl[i + 1];
					nl[i + 1] = h;
				}
			}
		}
	}

	return;
}

/* */
static void make_node_list_up(int l)
{
	struct gml_nlist* gnl = NULL;
	struct gml_node* n = NULL;
	int i = 0;

	/* for_all_nodes(g,n) */
	gnl = cnodelist;
	i = 0;

	while (gnl)
	{
		n = gnl->node;

		if (n->absy == l)
		{
			nl[i].node = n;
			nl[i].done = 0;	/* FALSE */
			if (is_dummy(n) == 1)
			{	/* */
/* higer value then the highest node in this level */
/*old nl[i].priority = (cnnodes_of_level[l + 1] + 1000 */
				nl[i].priority = (100000 - n->relx);
			}
			else
			{
				nl[i].priority = lower_connectivity(n);
			}
			i++;
		}
		gnl = gnl->next;
	}

	sort(cnnodes_of_level[l]);

	return;
}

/* */
static void make_node_list_down(int l)
{
	struct gml_nlist* gnl = NULL;
	struct gml_node* n = NULL;
	int i = 0;

	/* for_all_nodes(g,n) */
	gnl = cnodelist;

	while (gnl)
	{
		n = gnl->node;
		if (n->absy == l)
		{
			nl[i].node = n;
			nl[i].done = 0;	/* FALSE */
			if (is_dummy(n) == 1)
			{	/* */
/* give dummy node uniq high number */
/*old  nl[i].priority = (cnnodes_of_level[l - 1] + 1000 */
				nl[i].priority = (100000 - n->relx);
			}
			else
			{
				nl[i].priority = upper_connectivity(n);
			}
			i++;
		}
		gnl = gnl->next;
	}

	sort(cnnodes_of_level[l]);

	return;
}

/* get number of node with highest prio which is not done yet */
static int find_next(int n)
{
	int index = 0;
	int i = 0;
	int highest_priority = 0;

	for (i = 0; i < n; i++)
	{
		if ((nl[i].priority >= highest_priority)
			&& (nl[i].done == 0 /* FALSE */))
		{
			index = i;
			highest_priority = nl[i].priority;
		}
	}

	return (index);
}

static void do_down(int l)
{
	int i = 0;
	int index = 0;
	int j = 0;
	int optimal_position = 0;
	int distance = 0;
	int possible_distance = 0;

	for (i = 0; i < cnnodes_of_level[l]; i++)
	{
		index = find_next(cnnodes_of_level[l]);

		if (nl[index].node)
		{

			optimal_position = upper_barycenter(nl[index].node);

			if (optimal_position == 0)
			{
				optimal_position = nl[index].node->absx;
			}

			if (optimal_position < nl[index].node->absx)
			{
				distance = nl[index].node->absx - optimal_position;

				possible_distance = 0;
				j = index;

				do
				{
					if (j > 0)
					{
						possible_distance += nl[j].node->absx - nl[j - 1].node->absx - mindist;
					}
					else
					{
						/* j==0, no nodes at left */
						possible_distance += nl[j].node->absx - mindist;
					}
					j--;
				} while ((j >= 0) && !(nl[j].done));

				if (possible_distance < distance)
				{
					distance = possible_distance;
				}

				j = index;
				while (distance > 0)
				{
					int d = 0;
					int k = 0;

					if (j == 0)
					{
						d = distance;
					}
					else
					{
						if (nl[j].node->absx - nl[j - 1].node->absx - mindist < distance)
						{
							d = nl[j].node->absx - nl[j - 1].node->absx - mindist;
						}
						else
						{
							d = distance;
						}
					}

					for (k = j; k <= index; k++)
					{
						nl[k].node->absx -= d;
					}

					j--;
					distance -= d;
				}
			}
			else
			{
				distance = optimal_position - nl[index].node->absx;

				possible_distance = 0;
				j = index;

				do
				{
					if (j < cnnodes_of_level[l] - 1)
					{
						possible_distance += nl[j + 1].node->absx - nl[j].node->absx - mindist;
					}
					else
					{
						/* j == cnnodes_of_level[l]-1, no nodes rechts */
						possible_distance += distance;
					}
					j++;
				} while ((j < cnnodes_of_level[l]) && !(nl[j].done));

				if (possible_distance < distance)
				{
					distance = possible_distance;
				}

				j = index;
				while (distance > 0)
				{
					int d = 0;
					int k = 0;

					if (j == cnnodes_of_level[l] - 1)
					{
						d = distance;
					}
					else
					{
						if (nl[j + 1].node->absx - nl[j].node->absx - mindist < distance)
						{
							d = nl[j + 1].node->absx - nl[j].node->absx - mindist;
						}
						else
						{
							d = distance;
						}
					}

					for (k = index; k <= j; k++)
					{
						nl[k].node->absx += d;
					}

					j++;
					distance -= d;
				}
			}
			nl[index].done = 1;	/* TRUE */
		}
	}

	return;
}

static void do_up(int l)
{
	int i = 0;
	int index = 0;
	int j = 0;
	int optimal_position = 0;
	int distance = 0;
	int possible_distance = 0;

	for (i = 0; i < cnnodes_of_level[l]; i++)
	{
		index = find_next(cnnodes_of_level[l]);

		if (nl[index].node)
		{
			optimal_position = lower_barycenter(nl[index].node);

			if (optimal_position == 0)
			{
				optimal_position = nl[index].node->absx;
			}

			if (optimal_position < nl[index].node->absx)
			{
				distance = nl[index].node->absx - optimal_position;

				possible_distance = 0;
				j = index;
				do
				{
					if (j > 0)
					{
						possible_distance += nl[j].node->absx - nl[j - 1].node->absx - mindist;
					}
					else
					{
						/* j == 0, no nodes links */
						possible_distance += nl[0].node->absx - mindist;
					}
					j--;
				} while ((j >= 0) && !(nl[j].done));

				if (possible_distance < distance)
				{
					distance = possible_distance;
				}

				j = index;
				while (distance > 0)
				{
					int d = 0;
					int k = 0;

					if (j == 0)
					{
						d = distance;
					}
					else
					{
						if (nl[j].node->absx - nl[j - 1].node->absx - mindist < distance)
						{
							d = nl[j].node->absx - nl[j - 1].node->absx - mindist;
						}
						else
						{
							d = distance;
						}
					}

					for (k = j; k <= index; k++)
					{
						nl[k].node->absx -= d;
					}

					j--;
					distance -= d;
				}
			}
			else
			{
				/* optimal_position >= nl[index].node->absx */
				distance = optimal_position - nl[index].node->absx;

				possible_distance = 0;
				j = index;
				do
				{
					if (j < cnnodes_of_level[l] - 1)
					{
						possible_distance += nl[j + 1].node->absx - nl[j].node->absx - mindist;
					}
					else
					{
						/* j == cnnodes_of_level[l]-1, no nodes rechts */
						possible_distance += distance;
					}
					j++;
				} while ((j < cnnodes_of_level[l]) && !(nl[j].done));

				if (possible_distance < distance)
				{
					distance = possible_distance;
				}

				j = index;
				while (distance > 0)
				{
					int d = 0;
					int k = 0;

					if (j == cnnodes_of_level[l] - 1)
					{
						d = distance;
					}
					else
					{
						if (nl[j + 1].node->absx - nl[j].node->absx - mindist < distance)
						{
							d = nl[j + 1].node->absx - nl[j].node->absx - mindist;
						}
						else
						{
							d = distance;
						}
					}

					for (k = index; k <= j; k++)
					{
						nl[k].node->absx += d;
					}

					j++;
					distance -= d;
				}
			}
			nl[index].done = 1;	/* TRUE */
		}
	}

	return;
}

/* determine relative node pos. from the barycenter rel. node pos. */
static void improve_positions2local(struct gml_graph* g)
{
	int i = 0;
	int count = 0;
	int ii = 0;
	int sl = 0;

	/* start level is 0 */
	sl = 0;

	/* min. node dist */
	mindist = 1;

	/* number of up/down sweeps */
	count = 1;

	for (ii = 0; ii < count; ii++)
	{

		/* DOWN */
		for (i = sl; i < g->maxlevel; i++)
		{
			if (cnnodes_of_level[i])
			{
				nl = (struct node_data*)calloc(1, cnnodes_of_level[i] * sizeof(struct node_data));
				make_node_list_down(i);
				do_down(i);
				free(nl);
				nl = NULL;
			}
		}

		/* UP */
		for (i = (g->maxlevel - 1); i >= sl; i--)
		{
			if (cnnodes_of_level[i])
			{
				nl = (struct node_data*)calloc(1, cnnodes_of_level[i] * sizeof(struct node_data));
				make_node_list_up(i);
				do_up(i);
				free(nl);
				nl = NULL;
			}
		}

	}

	/* top+bottom update */
	if ((sl + 2) < g->maxlevel)
	{

		for (i = sl + 2; i >= sl; i--)
		{
			if (cnnodes_of_level[i])
			{
				nl = (struct node_data*)calloc(1, cnnodes_of_level[i] * sizeof(struct node_data));
				make_node_list_up(i);
				do_up(i);
				free(nl);
				nl = NULL;
			}
		}
	}

	for (i = (g->maxlevel - 2); i <= g->maxlevel; i++)
	{
		if (i >= 0)
		{
			if (cnnodes_of_level[i])
			{
				nl = (struct node_data*)calloc(1, cnnodes_of_level[i] * sizeof(struct node_data));
				make_node_list_down(i);
				do_down(i);
				free(nl);
				nl = NULL;
			}
		}
	}

	return;
}

/* create nodes-at-level-count */
static void make_cnnodes_at_level(struct gml_graph* g)
{
	struct gml_nlist* gnl = NULL;

	cnnodes_of_level = (int*)calloc(1, ((g->maxlevel + 1) * sizeof(int)));

	gnl = cnodelist;

	while (gnl)
	{
		cnnodes_of_level[gnl->node->rely] = cnnodes_of_level[gnl->node->rely] + 1;
		gnl = gnl->next;
	}

	return;
}

/* clear nodes-at-level-count */
static void clear_cnnodes_at_level(void)
{

	/* number of nodes at level */
	if (cnnodes_of_level)
	{
		free(cnnodes_of_level);
	}

	/* number of nodes at level */
	cnnodes_of_level = NULL;

	return;
}

/* copy part of graph */
static void make_cnodelist(struct gml_graph* g)
{
	struct gml_nlist* gnl = NULL;
	struct gml_nlist* newnl = NULL;

	gnl = g->nodelist;

	while (gnl)
	{

		/* check if node belongs to part of graph */
		if (gnl->node->startnode == csn)
		{
			/* copy node in new list */
			newnl = (struct gml_nlist*)calloc(1, sizeof(struct gml_nlist));
			newnl->node = gnl->node;
			if (cnodelist == NULL)
			{
				cnodelist = newnl;
				cnodelisttail = newnl;
			}
			else
			{
				cnodelisttail->next = newnl;
				cnodelisttail = newnl;
			}
		}

		gnl = gnl->next;
	}

	return;
}

/* done with this part of graph */
static void clear_cnodelist(void)
{
	struct gml_nlist* gnl = NULL;
	struct gml_nlist* gnlnext = NULL;

	gnl = cnodelist;

	while (gnl)
	{
		gnlnext = gnl->next;
		(void)free(gnl);
		gnl = gnlnext;
	}

	/* node list of part of graph */
	cnodelist = NULL;
	cnodelisttail = NULL;

	return;
}

/* move image of this part of graph */
static void move0(void)
{
	struct gml_nlist* gnl = NULL;
	int mx = 0;

	/* find min. x pos in-use */
	mx = 1000 * 1000;	/* just some high value */

	gnl = cnodelist;

	while (gnl)
	{
		if (gnl->node->absx < mx)
		{
			mx = gnl->node->absx;
		}
		gnl = gnl->next;
	}

	/* move whole drawing to the left */
	gnl = cnodelist;

	while (gnl)
	{
		gnl->node->absx = (gnl->node->absx - mx);
		gnl = gnl->next;
	}

	return;
}

/* */
static void make_cposnodes(void)
{
	struct gml_nlist* lnl = NULL;
	struct gml_nlist* newl = NULL;
	int i = 0;
	int lmaxw = 0;
	int maxrx = 0;

	/* widest x level */
	cwidestnnodes = 0;

	/* x width at position */
	cwpos = NULL;

	/* lists per pos. */
	cposnodes = NULL;

	/* extra check max rel. x pos. */
	lnl = cnodelist;

	while (lnl)
	{
		if (lnl->node->absx > maxrx)
		{
			maxrx = lnl->node->absx;
		}
		lnl = lnl->next;
	}

	/* pos2.c has moved node in x dir. */
	cwidestnnodes = maxrx;

	/* x width at position */
	cwpos = (int*)calloc(1, (cwidestnnodes + 1) * sizeof(int));

	if (cwpos == NULL)
	{
		return;
	}

	/* lists with nodes up to down at position */
	cposnodes = (struct gml_nlist**)calloc(1, (cwidestnnodes + 1) * sizeof(struct gml_nlist*));

	if (cposnodes == NULL)
	{
		return;
	}

	/* create for every postion the list of nodes at that position */
	lnl = cnodelist;

	while (lnl)
	{
		i = lnl->node->absx;

		newl = (struct gml_nlist*)calloc(1, sizeof(struct gml_nlist));

		if (newl == NULL)
		{
			return;
		}

		newl->node = lnl->node;

		if (cposnodes[i] == NULL)
		{
			cposnodes[i] = newl;
			newl->next = NULL;
		}
		else
		{
			newl->next = cposnodes[i];
			cposnodes[i] = newl;
		}

		lnl = lnl->next;
	}

	/* determine the max width of a element at vertical pos. */
	for (i = 0; i < (cwidestnnodes + 1); i++)
	{
		lmaxw = 0;

		/* lists per pos. */
		lnl = cposnodes[i];

		while (lnl)
		{
			if (lnl->node->bbx > lmaxw)
			{
				lmaxw = lnl->node->bbx;
			}
			lnl = lnl->next;
		}

		cwpos[i] = lmaxw;
	}

	return;
}

/* */
static void clear_cposnodes(void)
{
	int i = 0;
	struct gml_nlist* lnl = NULL;
	struct gml_nlist* nlnext = NULL;

	/* width of positions */
	if (cwpos)
	{
		free(cwpos);
		cwpos = NULL;
	}

	for (i = 0; i < (cwidestnnodes + 1); i++)
	{
		/* lists per pos. */
		lnl = cposnodes[i];

		while (lnl)
		{
			nlnext = lnl->next;
			free(lnl);
			lnl = nlnext;
		}

		cposnodes[i] = NULL;
	}

	free(cposnodes);
	cposnodes = NULL;

	return;
}

/* y positioning */
static void make_clevelnodes(struct gml_graph* g)
{
	struct gml_nlist* lnl = NULL;
	struct gml_nlist* newl = NULL;
	int i = 0;
	int lmaxh = 0;

	chpos = (int*)calloc(1, (g->maxlevel + 1) * sizeof(int));

	if (chpos == NULL)
	{
		return;
	}

	clevelnodes = (gml_nlist**)calloc(1, (g->maxlevel + 1) * sizeof(struct gml_nlist*));

	if (clevelnodes == NULL)
	{
		return;
	}

	lnl = cnodelist;

	while (lnl)
	{
		i = lnl->node->absy;

		newl = (gml_nlist*)calloc(1, sizeof(struct gml_nlist));

		if (newl == NULL)
		{
			return;
		}

		newl->node = lnl->node;

		if (clevelnodes[i] == NULL)
		{
			clevelnodes[i] = newl;
			newl->next = NULL;
		}
		else
		{
			newl->next = clevelnodes[i];
			clevelnodes[i] = newl;
		}

		lnl = lnl->next;
	}

	/* determine the max width of a element at vertical pos. */
	for (i = 0; i < (g->maxlevel + 1); i++)
	{

		lmaxh = 0;

		/* lists per pos. */
		lnl = clevelnodes[i];

		while (lnl)
		{
			if (lnl->node->bby > lmaxh)
			{
				lmaxh = lnl->node->bby;
			}
			lnl = lnl->next;
		}

		chpos[i] = lmaxh;
	}

	return;
}

static void clear_clevelnodes(struct gml_graph* g)
{
	int i = 0;
	struct gml_nlist* lnl = NULL;
	struct gml_nlist* nlnext = NULL;

	/* width of positions */
	if (chpos)
	{
		free(chpos);
		chpos = NULL;
	}

	for (i = 0; i < (g->maxlevel + 1); i++)
	{
		/* lists per pos. */
		lnl = clevelnodes[i];

		while (lnl)
		{
			nlnext = lnl->next;
			free(lnl);
			lnl = nlnext;
		}

		clevelnodes[i] = NULL;
	}

	free(clevelnodes);
	clevelnodes = NULL;

	return;
}

/* determine final (x,y) pos */
static void cfinalxy(struct gml_graph* g)
{
	struct gml_nlist* lnl = NULL;
	int hw = 0;
	int xoff = 0;
	int yoff = 0;
	int i = 0;
	int ecount = 0;

	/* x positioning */
	make_cposnodes();

	cmaxx = 0;
	xoff = 0;

	/* scan hor. to adjust the x positions. */
	for (i = 0; i < (cwidestnnodes + 1); i++)
	{
		/* x spacing between the hor. levels */
		if (0)
		{
			xoff = xoff + xspacing;
		}

		/* determine half-way of the xpos. */
		if (cwpos[i] == 0)
		{
			/* if only dummy nodes */
			hw = xspacing / 2;
		}
		else
		{
			hw = (cwpos[i] / 2);
		}

		/* update with current x */
		hw = hw + xoff;

		lnl = cposnodes[i];

		/* scan the nodes at this x pos. */
		while (lnl)
		{
			/* center the node around the half-way */
			lnl->node->finx = (hw - (lnl->node->bbx / 2));

			if ((lnl->node->finx + lnl->node->bbx) > cmaxx)
			{
				cmaxx = (lnl->node->finx + lnl->node->bbx);
			}

			lnl = lnl->next;
		}

		/* set x0,x1 pos in the nodes */
		lnl = cposnodes[i];

		/* scan the nodes at this x pos. */
		while (lnl)
		{
			/* */
			lnl->node->lx0 = xoff;
			lnl->node->lx1 = xoff + cwpos[i];
			lnl = lnl->next;
		}

		/* x spacing between the hor. levels */
		xoff = xoff + xspacing;

		/* x to next pos. */
		xoff = xoff + cwpos[i];
	}

	/* */
	clear_cposnodes();

	/* y positioning */
	make_clevelnodes(g);

	cmaxy = 0;
	yoff = 0;

	/* number of edges between level n and n+1 */
	g->nume = (int*)calloc(1, (g->maxlevel + 1) * sizeof(int));

	/* scan vert. to adjust the y positions. */
	for (i = 0; i < (g->maxlevel + 1); i++)
	{
		/* y spacing between the vert. levels */
		if (0)
		{
			yoff = (yoff + yspacing);
		}

		/* determine half-way of the ypos. */
		if (chpos[i] == 0)
		{
			/* if only dummy nodes */
			hw = (yspacing / 2);
		}
		else
		{
			hw = (chpos[i] / 2);
		}

		/* update with current y */
		hw = hw + yoff;

		lnl = clevelnodes[i];

		ecount = 0;

		/* scan the nodes at this y pos. */
		while (lnl)
		{
			/* set start, end of y level */
			lnl->node->ly0 = yoff;
			lnl->node->ly1 = (yoff + chpos[i]);

			/* center the node around the half-way */
			lnl->node->finy = (hw - (lnl->node->bby / 2));

			/* update drawing max y pos used */
			if ((lnl->node->finy + lnl->node->bby) > cmaxy)
			{
				cmaxy = (lnl->node->finy + lnl->node->bby);
			}

			/* give dummy nodes a vertical size of the level */
			if (lnl->node->dummy)
			{
				lnl->node->bby = chpos[i];
				/* if only dummy nodes at level, use spacing */
				if (chpos[i] == 0)
				{
					lnl->node->bby = yspacing;
				}
			}

			/* number of edges between level n and n+1 */
			ecount = ecount + lnl->node->outdegree;

			lnl = lnl->next;
		}

		g->nume[i] = ecount;

		/* y spacing between the vert. levels */
		yoff = yoff + yspacing;

		/* yspacing depends on number of edges at this level
		 * turned off, does increase y too much
		 * yoff = yoff + (ecount * 2);
		 */
		 /* yspacing depends on number of crossing edges at this level
		  * temp test
		  */
		yoff = yoff + (1 * (g->numce[i] / 16));

		/* y to next pos. */
		yoff = yoff + chpos[i];
	}

	clear_clevelnodes(g);

	/* clear number of edges between level n and n+1 */
	if (g->nume)
	{
		free(g->nume);
		g->nume = NULL;
	}

	return;
}

static void movefinal(int xoffset)
{
	struct gml_nlist* gnl = NULL;

	gnl = cnodelist;

	while (gnl)
	{
		gnl->node->finx = gnl->node->finx + xoffset;
		gnl->node->lx0 = gnl->node->lx0 + xoffset;
		gnl->node->lx1 = gnl->node->lx1 + xoffset;
		gnl = gnl->next;
	}

	return;
}

/* dummy nodes can be centered, or left/right most placed */
static void tunedummy(struct gml_graph* g)
{
	struct gml_nlist* gnl = NULL;
	int x1 = 0;
	int x2 = 0;
	int x3 = 0;

	gnl = g->nodelist;

	while (gnl)
	{
		if (gnl->node->dummy)
		{
			x1 = gnl->node->finx;
			x2 = gnl->node->incoming_e->edge->from_node->finx + gnl->node->incoming_e->edge->from_node->bbx / 2;
			x3 = gnl->node->outgoing_e->edge->to_node->finx + gnl->node->outgoing_e->edge->to_node->bbx / 2;
			if ((x1 == x2) && (x1 == x3))
			{
				/* no move */
			}
			else
			{
				if ((x2 < x1) && (x3 < x1))
				{
					/* to left */
					gnl->node->finx = gnl->node->lx0;
				}
				if ((x2 > x1) && (x3 > x1))
				{
					/* to right */
					gnl->node->finx = gnl->node->lx1;
				}
			}
		}
		gnl = gnl->next;
	}

	return;
}

/* move some nodes up/down */
static void tunenodes(struct gml_graph* g)
{
	struct gml_nlist* gnl = NULL;

	gnl = g->nodelist;

	while (gnl)
	{
		/* only at real nodes */
		if (gnl->node->dummy == 0)
		{
			if (gnl->node->hashedge)
			{
				/* do not move node with hor. edge */
			}
			else
			{
				if (gnl->node->indegree > 0 && gnl->node->outdegree == 0)
				{
					/* move up */
					gnl->node->finy = gnl->node->ly0;
				}
				if (gnl->node->indegree == 0 && gnl->node->outdegree > 0)
				{
					/* move down */
					gnl->node->finy = (gnl->node->ly1 - gnl->node->bby);
				}
				if (gnl->node->indegree > 0 && gnl->node->outdegree > 0)
				{
					if (gnl->node->indegree == gnl->node->outdegree)
					{
						/* no movement
						 *
						 */
					}
					else
					{
						if (gnl->node->indegree > gnl->node->outdegree)
						{
							/* move up */
							gnl->node->finy = gnl->node->ly0;
						}
						if (gnl->node->outdegree > gnl->node->indegree)
						{
							/* move down */
							gnl->node->finy = (gnl->node->ly1 - gnl->node->bby);
						}
					}
				}
			}
		}
		gnl = gnl->next;
	}

	return;
}

/* position in parts of graph at each step */
static void improve_positions(struct gml_graph* g)
{
	struct gml_nlist* gnl = NULL;
	int i = 0;
	int xoffset = 0;

	xspacing = g->xspacing;
	yspacing = g->yspacing;

	/* copy the rel(x,y) pos into abs(x,y) and modify the absx pos here */
	gnl = g->nodelist;

	while (gnl)
	{
		gnl->node->bbx = gnl->node->tx;
		gnl->node->bby = gnl->node->ty;
		gnl->node->absx = gnl->node->relx;
		gnl->node->absy = gnl->node->rely;
		gnl->node->finx = 0;
		gnl->node->finy = 0;
		gnl = gnl->next;
	}

	/* offset in drawing of part of graph */
	xoffset = 0;

	for (i = 0; i < g->nstartnodes; i++)
	{
		/* set current startnode */
		csn = g->startnodes[i];

		/* print progress info */
		if ((i == 0) || (i == g->nstartnodes / 2) || (i == g->nstartnodes - 1))
		{
		}

		/* max. x in part of graph */
		cmaxx = 0;

		/* copy part of graph */
		make_cnodelist(g);

		/* create nodes-at-level-count */
		make_cnnodes_at_level(g);

		/* run up/down placement */
		improve_positions2local(g);

		/* move image of this part of graph */
		move0();

		/* set final x,y */
		cfinalxy(g);

		/* tune dummy nodes */
		tunedummy(g);

		/* tune nodes */
		tunenodes(g);

		/* move */
		movefinal(xoffset);

		/* update for next */
		xoffset = xoffset + cmaxx + xspacing;

		/* clear nodes-at-level-count */
		clear_cnnodes_at_level();

		/* done with this part of graph */
		clear_cnodelist();
	}

	/* position level 0, single nodes if any */
	if (g->nsinglenodes)
	{
		/* done in finalxy() in main.c */
	}

	return;
}

/* for pos2.c which does set finx,finy */
static void finalxy(struct gml_graph* g)
{
	struct gml_nlist* lnl = NULL;
	int maxx = 0;
	int maxy = 0;
	int curx = 0;
	int my = 0;

	/* position the single nodes */
	if (g->nsinglenodes)
	{
		lnl = maingraph->singlenodelist;
		while (lnl)
		{
			lnl->node->finx = curx;
			curx = curx + g->xspacing + lnl->node->bbx;
			if (lnl->node->bby > my)
			{
				my = lnl->node->bby;
			}
			lnl = lnl->next;
		}
		my = my + g->yspacing;
		/* update level data for singlenodes */
		lnl = maingraph->singlenodelist;
		while (lnl)
		{
			lnl->node->ly0 = 0;
			lnl->node->ly1 = my;
			lnl = lnl->next;
		}
	}

	/* determine max. x pos in use */
	lnl = maingraph->nodelist;
	while (lnl)
	{

		if ((lnl->node->finx + lnl->node->bbx) > maxx)
		{
			maxx = lnl->node->finx + lnl->node->bbx;
		}

		/* correct for height of single nodes if any */
		if (lnl->node->indegree || lnl->node->outdegree)
		{
			lnl->node->finy = lnl->node->finy + my;
		}

		/* update drawing max y pos used */
		if ((lnl->node->finy + lnl->node->bby) > maxy)
		{
			maxy = (lnl->node->finy + lnl->node->bby);
		}

		lnl = lnl->next;
	}

	g->maxx = maxx;
	g->maxy = maxy;

	return;
}

static struct gml_edge* findedge(int num)
{
	struct gml_elist* el = NULL;
	struct gml_edge* e = NULL;
	if (maingraph == NULL)
	{
		return (NULL);
	}
	el = maingraph->edgelist;
	while (el)
	{
		e = el->edge;
		if (e->nr == num)
		{
			break;
		}
		el = el->next;
	}
	return (e);
}

/* update node min/max and edge min/max */
static void setminmax(struct gml_graph* g)
{
	struct gml_nlist* gml_nl = NULL;
	struct gml_elist* el = NULL;
	int count = 0;

	g->nodemin = 0;
	g->nodemax = 0;
	g->edgemin = 0;
	g->edgemax = 0;

	gml_nl = g->nodelist;
	count = 0;
	while (gml_nl)
	{
		if (count == 0)
		{
			g->nodemin = gml_nl->node->nr;
			g->nodemax = gml_nl->node->nr;
		}
		else
		{
			if (gml_nl->node->nr < g->nodemin)
			{
				g->nodemin = gml_nl->node->nr;
			}
			if (gml_nl->node->nr > g->nodemax)
			{
				g->nodemax = gml_nl->node->nr;
			}
		}
		count++;
		gml_nl = gml_nl->next;
	}

	el = g->edgelist;
	count = 0;
	while (el)
	{
		if (count == 0)
		{
			g->edgemin = el->edge->nr;
			g->edgemax = el->edge->nr;
		}
		else
		{
			if (el->edge->nr < g->edgemin)
			{
				g->edgemin = el->edge->nr;
			}
			if (el->edge->nr > g->edgemax)
			{
				g->edgemax = el->edge->nr;
			}
		}
		count++;
		el = el->next;
	}

	return;
}

}
/* end zzzz */