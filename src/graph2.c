/**
 * @file graph2.c
 * @brief something something something...
 *
 * @authors: Linus Scott
 *           Helmer Nordström (ens24hnm@cs.umu.se)
 *           Otto Silander
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

struct node {
	char *name;
	bool is_seen;
	dlist *neighbours;
};

struct graph {
	int amount_of_nodes;
	int amount_of_edges;
	int max_nodes;
	array_1d *nodes;
};

node *create_node(const char *name) {
	node *n = calloc(1, sizeof(*n));

	int len = strlen(name);
	n->name = calloc(len, sizeof(n->name));
	strncpy(n->name, name, len);
	n->is_seen = false;
	n->neighbours = dlist_empty(NULL);

	return n;
}

void node_kill(void *p) {
	node *n = p;
	free(n->name);
	dlist_kill(n->neighbours);
}

bool nodes_are_equal(const node *n1, const node *n2) {
	return strncmp(n1->name, n2->name, strlen(n1->name)) == 0;
}

graph *graph_empty(const int max_nodes) {
	graph *g = calloc(1, sizeof(*g));
	g->amount_of_edges = 0;
	g->amount_of_nodes = 0;
	g->max_nodes = max_nodes;

	g->nodes = array_1d_create(0, max_nodes - 1, NULL);

	return g;
}

bool graph_is_empty(const graph *g) {
	return g->amount_of_nodes == 0;
}

bool graph_has_edges(const graph *g) {
	return g->amount_of_edges != 0;
}

graph *graph_insert_node(graph *g, const char *s) {
	node *n = create_node(s);

	array_1d_set_value(g->nodes, n, g->amount_of_nodes);
	g->amount_of_nodes++;

	return g;
}

node *graph_find_node(const graph *g, const char *s) {
	for (int i = 0; i < g->amount_of_nodes; i++) {
		node *n = array_1d_inspect_value(g->nodes, i);
		if (strncmp(n->name, s, strlen(n->name)) == 0) {
			return n;
		}
	}

	return NULL;
}

bool graph_node_is_seen(const graph *g, const node *n) {
	return n->is_seen;
}

graph *graph_node_set_seen(graph *g, node *n, bool seen) {
	n->is_seen = seen;
	return g;
}

graph *graph_reset_seen(graph *g) {
	for (int i = 0; i < g->amount_of_nodes; i++) {
		node *n = array_1d_inspect_value(g->nodes, i);
		n->is_seen = false;
	}

	return g;
}

graph *graph_insert_edge(graph *g, node *n1, node *n2) {
	dlist_insert(n1->neighbours, n2, dlist_first(n1->neighbours));

	return g;
}

dlist *graph_neighbours(const graph *g, const node *n) {
	dlist *neighbours = dlist_empty(NULL);

	dlist_pos n_pos = dlist_first(n->neighbours);
	while (!dlist_is_end(n->neighbours, n_pos)) {
		dlist_insert(neighbours, dlist_inspect(n->neighbours, n_pos), dlist_first(neighbours));

		n_pos = dlist_next(n->neighbours, n_pos);
	}

	return neighbours;
}

void graph_kill(graph *g) {
	for (int i = 0; i < g->amount_of_nodes; i++) {
		node *n = array_1d_inspect_value(g->nodes, i);
		free(n);
	}
	array_1d_kill(g->nodes);
	free(g);
}
