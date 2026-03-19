/**
 * @file graph2.c
 * @brief something something something...
 *
 * @authors: Linus Scott      (ens24lst@cs.umu.se)
 *           Helmer Nordström (ens24hnm@cs.umu.se)
 *           Otto Silander    (c25osr@cs.umu.se)
 *
 * Version information:
 *     v1.0 xxxx-xx-xx:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graph.h>
#include <array_1d.h>
#include <dlist.h>


/**
 * Uses a directed list to store pointers 
 * to each neighbouring node
 */
struct node {
	char *name;
	bool is_seen;
	dlist *neighbours;
};


/**
 * Uses an array1d to store every node in the graph.
 * 
 */
struct graph {
	int amount_of_nodes;
	int amount_of_edges;
	int max_nodes;
	array_1d *nodes;
};

//-------INTERNALLY USED FUNCTIONS-------

/**
 * create_node() - Creates a new node
 * 
 * @name: Name to give created node
 * 
 * Returns: Pointer to newly created node.
 */
node *create_node(const char *name) {
	node *n = calloc(1, sizeof(*n));

	int len = strlen(name) + 1;
	n->name = calloc(len, sizeof(*(n->name)));
	strncpy(n->name, name, len);
	n->is_seen = false;
	n->neighbours = dlist_empty(NULL);

	return n;
}

/**
 * node_kill() - Frees memory used by a node.
 * @p: Pointer to node in void form.
 * 
 * Returns: Nothing
 */
void node_kill(void *p) {
	node *n = p;
	free(n->name);
	dlist_kill(n->neighbours);
	free(n);
}

/**
 * nodes_are_equal() - Check whether two nodes are equal.
 * @n1: Pointer to node 1.
 * @n2: Pointer to node 2.
 *
 * Returns: true if the nodes are considered equal, otherwise false.
 *
 */
bool nodes_are_equal(const node *n1, const node *n2) {
	return strncmp(n1->name, n2->name, strlen(n1->name)) == 0;
}

/**
 * graph_empty() - Create an empty graph.
 * @max_nodes: The maximum number of nodes the graph can hold.
 *
 * Returns: A pointer to the new graph.
 */
graph *graph_empty(const int max_nodes) {
	graph *g = calloc(1, sizeof(*g));
	g->amount_of_edges = 0;
	g->amount_of_nodes = 0;
	g->max_nodes = max_nodes;

	g->nodes = array_1d_create(0, max_nodes - 1, NULL);

	return g;
}

/**
 * graph_is_empty() - Check if a graph is empty, i.e. has no nodes.
 * @g: Graph to check.
 *
 * Returns: True if graph is empty, otherwise false.
 */
bool graph_is_empty(const graph *g) {
	return g->amount_of_nodes == 0;
}

/**
 * graph_has_edges() - Check if a graph has any edges.
 * @g: Graph to check.
 *
 * Returns: True if graph has any edges, otherwise false.
 */
bool graph_has_edges(const graph *g) {
	return g->amount_of_edges != 0;
}

/**
 * graph_insert_node() - Inserts a node with the given name into the graph.
 * @g: Graph to manipulate.
 * @s: Node name.
 *
 * Creates a new node with a copy of the given name and puts it into
 * the graph.
 *
 * Returns: The modified graph.
 */
graph *graph_insert_node(graph *g, const char *s) {
	node *n = create_node(s);

	array_1d_set_value(g->nodes, n, g->amount_of_nodes);
	g->amount_of_nodes++;

	return g;
}

/**
 * graph_find_node() - Find a node stored in the graph.
 * @g: Graph to manipulate.
 * @s: Node identifier, e.g. a char *.
 *
 * Returns: A pointer to the found node, or NULL.
 */
node *graph_find_node(const graph *g, const char *s) {
    //Loops through all nodes in graph g
	for (int i = 0; i < g->amount_of_nodes; i++) {
		node *n = array_1d_inspect_value(g->nodes, i);

        //Checks if node names are the same
		if (strncmp(n->name, s, strlen(n->name)) == 0) {
			return n;
		}
	}

	return NULL;
}

/**
 * graph_node_is_seen() - Return the seen status for a node.
 * @g: Graph storing the node.
 * @n: Node in the graph to return seen status for.
 *
 * Returns: The seen status for the node.
 */
bool graph_node_is_seen(const graph *g, const node *n) {
	return n->is_seen;
}

/**
 * graph_node_set_seen() - Set the seen status for a node.
 * @g: Graph storing the node.
 * @n: Node in the graph to set seen status for.
 * @s: Status to set.
 *
 * Returns: The modified graph.
 */
graph *graph_node_set_seen(graph *g, node *n, bool seen) {
	n->is_seen = seen;
	return g;
}

/**
 * graph_reset_seen() - Reset the seen status on all nodes in the graph.
 * @g: Graph to modify.
 *
 * Returns: The modified graph.
 */
graph *graph_reset_seen(graph *g) {
    //Loops through all nodes in graph g
	for (int i = 0; i < g->amount_of_nodes; i++) {
		node *n = array_1d_inspect_value(g->nodes, i);
		n->is_seen = false;
	}

	return g;
}

/**
 * graph_insert_edge() - Insert an edge into the graph.
 * @g: Graph to manipulate.
 * @n1: Source node (pointer) for the edge.
 * @n2: Destination node (pointer) for the edge.
 *
 * NOTE: Undefined unless both nodes are already in the graph.
 *
 * Returns: The modified graph.
 */
graph *graph_insert_edge(graph *g, node *n1, node *n2) {
	dlist_insert(n1->neighbours, n2, dlist_first(n1->neighbours));

	return g;
}

/**
 * graph_neighbours() - Return a list of neighbour nodes.
 * @g: Graph to inspect.
 * @n: Node to get neighbours for.
 *
 * Returns: A pointer to a list of nodes. Note: The list must be
 * dlist_kill()-ed after use.
 */
dlist *graph_neighbours(const graph *g, const node *n) {
    //Creates dlist to return.
	dlist *neighbours = dlist_empty(NULL);

    //Makes sure that list in graph g and list to return are identical
	dlist_pos n_pos = dlist_first(n->neighbours);

    //Loops through list of neighbours
	while (!dlist_is_end(n->neighbours, n_pos)) {
		dlist_insert(neighbours, dlist_inspect(n->neighbours, n_pos), dlist_first(neighbours));

		n_pos = dlist_next(n->neighbours, n_pos);
	}

	return neighbours;
}
/**
 * graph_kill() - Destroy a given graph.
 * @g: Graph to destroy.
 *
 * Return all dynamic memory used by the graph.
 *
 * Returns: Nothing.
 */
void graph_kill(graph *g) {
    //Loops through all nodes
	for (int i = 0; i < g->amount_of_nodes; i++) {
		node *n = array_1d_inspect_value(g->nodes, i);
		node_kill(n);
	}
	array_1d_kill(g->nodes);
	free(g);
}
