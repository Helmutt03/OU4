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
	g->node_count++;

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

graph* graph_reset_seen(graph* g)
{
	for (int i = 0; i < g->node_count; i++)
	{
		node *n = array_1d_inspect_value(g->nodes, i);

		n->is_seen = false;
	}
	
	return g;
}

graph* graph_insert_edge(graph* g, node* n1, node* n2)
{
	// Indexes in the array for the nodes, -1 if not found
	int i1 = -1, i2 = -1;
	// Search the graph
	for (int i = 0; i < g->node_count; i++)
	{
		node *g_node = array_1d_inspect_value(g->nodes, i);

		if (nodes_are_equal(n1, g_node)){
			i1 = i;
		}
		else if (nodes_are_equal(n2, g_node)){
			i2 = i;
		}
		
	}

	// Do nothing if n1 or n2 are not found in the graph
	if(i1 == -1 || i2 == -1){
		fprintf(stderr, "One or more node not found, graph is not changed.\n");
		return g;
	}

	// Allocate an integer
	int *edge = malloc(sizeof(*edge));
	*edge = 1;

	// Insert the edge
	array_2d_set_value(g->neighbour_graph, edge, i1, i2);
	g->edge_count++;
	return g;
}

dlist* graph_neighbours(const graph* g, const node* n)
{
	// Find the node index, -1 is not found
	int node_index = -1;
	for (int i = 0; i < g->node_count; i++)
	{
		node *g_node = array_1d_inspect_value(g->nodes, i);
		if(nodes_are_equal(n, g_node)){
			node_index = i;
			// Finish the loop
			i = g->node_count;
		}
	}

	
	if(node_index == -1){
		fprintf(stderr, "Node not found, returning NULL");
		return NULL;
	}
	dlist *neighbours = dlist_empty(node_kill);

	for (int i = 0; i < g->node_count; i++)
	{
		int *is_neighbour = array_2d_inspect_value(g->neighbour_graph, node_index, i);
		if (is_neighbour != NULL && *is_neighbour == 1)
		{
			node *n = array_1d_inspect_value(g->nodes, i);
			// Insert to first pos for simplicity
			dlist_insert(neighbours, n, dlist_first(neighbours));
		}
		
	}
	return neighbours;
	
}

void graph_kill(graph* g)
{
	array_1d_kill(g->nodes);
	array_2d_kill(g->neighbour_graph);

	free(g);
}