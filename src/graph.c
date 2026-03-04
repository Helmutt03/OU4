// Test push

#include <graph.h>
#include <array_2d.h>
#include <array_1d.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Uses an array_1d containing the nodes and 
 * keeping track of index that way.
 * The neighbour graph then uses the indexes
 * that are found through the array_1d;
 */

struct graph {
	int node_count;
	int edge_count;
	array_1d *nodes;
	array_2d *neighbour_graph;
};

struct node {
	char *name;
	bool is_seen;
};

// INTERNALLY USED FUNCTIONS

/**
 * node_kill() - Free the memory used by a node pointer.
 * @p: The node pointer in void form.
 *
 * Returns: Nothing
 */

void node_kill(void *p)
{
	node *n = p;
	free(n->name);
	free(n);
}

/**
 * int_kill() - Free the memory used by an int pointer.
 * @p: The int pointer in void form.
 *
 * Returns: Nothing
 */

void int_kill(void *p)
{
	int *i = p;
	free(i);
}

// Functions in graph.h

bool nodes_are_equal(const node* n1, const node* n2)
{
	return strcmp(n1->name, n2->name) == 0;
}

graph *graph_empty(int max_nodes)
{
	graph *g = malloc(sizeof(*g));

	g->node_count = 0;
	g->edge_count = 0;
	g->nodes = array_1d_create(0, max_nodes - 1, node_kill);
	g->neighbour_graph = array_2d_create(0, max_nodes - 1, 0, max_nodes - 1, int_kill);

	return g;
}

bool graph_is_empty(const graph* g)
{
	return g->node_count == 0;
}

bool graph_has_edges(const graph* g)
{
	return g->edge_count == 0;
}

graph* graph_insert_node(graph* g, const char* s)
{
	if(g->node_count >= array_1d_high(g->nodes) + 1){
		fprintf(stderr, "FAIL: Can't have more than %d nodes in the graph", array_1d_high(g->nodes) + 1);
		return g;
	}

	// Create a new node
	node *n = malloc(sizeof(*n));

	strcpy(n->name, s);
	n->is_seen = false;

	// Add the node to the last available index
	array_1d_set_value(g->nodes, n, g->node_count);

	return g;
}

node* graph_find_node(const graph* g, const char* s)
{
	// Go through the nodes and look for a match
	for (int i = 0; i < g->node_count; i++)
	{
		node *n = array_1d_inspect_value(g->nodes, i);
		if(strcmp(s, n->name)){
			return(n);
		}
	}

	// If nothing is found, return NULL
	return NULL;
}

bool graph_node_is_seen(const graph* g, const node* n)
{
	return n->is_seen;
}

graph* graph_node_set_seen(graph* g, node* n, bool seen)
{
	n->is_seen = seen;
	return g;
}