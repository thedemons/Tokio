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

#ifndef SFG_H
#define SFG_H

namespace sfg {

struct gml_graph;
struct gml_node;
struct gml_edge;
struct gml_nlist;
struct gml_elist;

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

extern struct gml_graph* maingraph;

/* returns a version number as version 1.0 returns int 10 */
extern int sfg_version(void);

/* init
 * returns  0 if oke
 * returns -1 if already inited
 * returns -2 if other error
 */
extern int sfg_init();

/* de-init
 * returns  0 if oke
 * returns -1 if not inited
 */
extern int sfg_deinit(void);

/* add a node with uniq number starting at 1
 * with (tx,ty) as rectangle size for label text or (0,0)
 * before adding edges all node numbers must be defined
 * the data is optional and can be pointer too own structure
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if number is lower then 1
 * returns -3 if tx number is lower then 0
 * returns -4 if ty number is lower then 0
 * returns -5 if layout already done
 * returns -6 if node already defined
 * returns -7 if other error
 */
extern int sfg_addnode(int number, int tx, int ty);

/* add a edge with uniq number starting at 1
 * the from-node number is in from, the to-node number is in to
 * self-edges are allowed but not with a label
 * with (tx,ty) as rectangle size for label text or (0,0)
 * the data is optional and can be pointer too own structure
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
extern int sfg_addedge(int number, int from, int to, int tx, int ty);

/* run sugiyama barycenter layout
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if layout already done
 * returns -3 if no nodes in the graph
 */
extern int sfg_layout(void);

/* return edge crossings in the graph
 * returns crossings >= 0 if oke
 * returns -1 if layout is not done
 */
extern int sfg_crossings(void);

/* return initial edge crossings in the graph
 * returns crossings >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
extern int sfg_initialcrossings(void);

/* get min node number in use after layout
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if there are no nodes
 */
extern int sfg_nodemin(void);

/* get maxc node number in use after layout
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if there are no nodes
 */
extern int sfg_nodemax(void);

/* get min edge number in use after layout
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if there are no edges
 */
extern int sfg_edgemin(void);

/* get max edge number in use after layout
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if there are no edges
 */
extern int sfg_edgemax(void);

/* set 1 to add edgelabels, or 0 to remove edgelabels
 * returns crossings >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
extern int sfg_edgelabels(int status);

/* return x pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
extern int sfg_nodexpos(int num);

/* return y pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
extern int sfg_nodeypos(int num);

/* return relative x pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
extern int sfg_noderelxpos(int num);

/* return relative y pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
extern int sfg_noderelypos(int num);

/* return level y start pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
extern int sfg_nodely0(int num);

/* return level y end pos of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
extern int sfg_nodely1(int num);

/* return x size of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
extern int sfg_nodexsize(int num);

/* return y size of node with uniq number
 * returns >= 0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if number < 1
 * returns -4 if node not found
 */
extern int sfg_nodeysize(int num);

/* set min. x spacing between nodes
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if number is lower then 0
 * returns -3 if layout already done
 */
extern int sfg_xspacing(int num);

/* set min. y spacing between nodes
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if number is lower then 0
 * returns -3 if layout already done
 */
extern int sfg_yspacing(int num);

/* return type of node with uniq number
 * returns type of node, 1=regular, 2=dummy, 3=edgelabel node
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
extern int sfg_nodetype(int num);

/* return number of selfedges at node
 * returns number of selfedges if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
extern int sfg_nodeselfedges(int num);

/* return number of incoming edges to node
 * returns indegree number if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
extern int sfg_nodeindegree(int num);

/* return number of outgoing edges from node
 * returns outdegree number if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
extern int sfg_nodeoutdegree(int num);

/* return edge number of node if edgelabel node
 * returns number of original edge with edgelabel if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 * returns -5 if node is not edgelabel
 */
extern int sfg_nodeenum(int num);

/* get optional data pointer of node
 * returns data pointer if oke
 * returns NULL if not inited
 * returns NULL if layout not done
 * returns NULL if nodenumber is < 1
 * returns NULL if node not found
 */
extern void *sfg_nodedata(int num);

/* set optional data pointer of node
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 * returns -3 if nodenumber is < 1
 * returns -4 if node not found
 */
extern int sfg_setnodedata(int num, void *data);

/* get from-node of edge
 * returns from-node number if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if edgenumber is < 1
 * returns -4 if edge not found
 */
extern int sfg_edgefrom(int num);

/* get to-node of edge
 * returns to-node number if oke
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if edgenumber is < 1
 * returns -4 if edge not found
 */
extern int sfg_edgeto(int num);

/* get edge type
 * returns type if oke, 1=regular, 2=selfedge, 3=hor. edge
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if edgenumber is < 1
 * returns -4 if edge not found
 */
extern int sfg_edgetype(int num);

/* get edge reversed status
 * returns  1 if reversed edge or 0 if not
 * returns -1 not inited
 * returns -2 if layout not done
 * returns -3 if edgenumber is < 1
 * returns -4 if edge not found
 */
extern int sfg_edgerev(int num);

/* get max x pos in drawing
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
extern int sfg_maxx(void);

/* get max y pos in drawing
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
extern int sfg_maxy(void);

/* get number of levels
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
extern int sfg_nlevels(void);

/* get number of nodes
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
extern int sfg_nnodes(void);

/* get number of edges
 * returns value if oke
 * returns -1 if not inited
 * returns -2 if layout not done
 */
extern int sfg_nedges(void);

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
extern int sfg_node_foreach(int (*getnodedata)
			     (int num, int level, int pos, int xpos, int ypos, int tx, int ty, int nselfedges, int type,
			      int indegree, int outdegree, int ly0, int ly1));

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
 * int type, 1=regular, 2=selfedge, 3=horizontal-edge
 * int rev, set if edge is reversed
 * returns  0 if oke
 * returns -1 if not inited
 * returns -2 if no layout is done
 * returns -3 if no callback routine
 */
extern int sfg_edge_foreach(int (*getedgedata)(int num, int from, int to, int type, int rev));

}
#endif

/* end */
