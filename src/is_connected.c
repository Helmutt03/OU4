/**
 * @file is_connected.c
 *
 * @brief An interactive program used to determine if there is a path from one aiport to the
 *        other. Takes as input a map file containing the routes of the airports.
 *        The file needs to follow the specification for the program to work correctly.
 *
 *        This program is made for the OU4 assignment of the "Datastructures and algorithms"
 *        course at the Department of Computing Science, Umea University
 *
 * Duplicates are handled via the function load_graph()
 *
 * @authors: Helmer Nordström (ens24hnm@cs.umu.se)
 *           Otto Silander
 *           Linus Scott
 *
 * Version information:
 *     v1.0 2026-03-16:
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include <graph.h>
#include <queue.h>

#define MAXNODENAME 40
#define BUFSIZE 400 // Used for reading input lines

// Function declarations:
void check_params(int argc, char *argv[], FILE **map_data);
graph *load_graph(FILE *map_data);
int get_num_of_routes(FILE *map_data);
bool find_path(graph *g, node *src, node *dest);

// Helper functions
int first_non_white_space(const char *s);
bool line_is_blank(const char *s);
bool line_is_comment(const char *s);
int parse_map_line(const char *buf, char *n1, char *n2);

/**
 * @brief Main program loop, loads the graph,
 *        asks the user for two airports and returns
 *        if they are connected or not
 *
 * @param argc The amount of args the program was given,
 *             used in verifying correct use of the program
 * @param argv The argument strings,
 *             used to read from the map file
 *
 * @return 0 on normal exit
 */
int main(int argc, char *argv[]) {

	// Get the map data
	FILE *map_data = NULL;
	check_params(argc, argv, &map_data);

	// Load the graph structure with all routes
	graph *g = load_graph(map_data);

	// Allocate memory and initialise variables
	char *buf = calloc(BUFSIZE, sizeof(*buf));
	char *n1 = calloc(MAXNODENAME, sizeof(*n1));
	char *n2 = calloc(MAXNODENAME, sizeof(*n1));
	node *src = NULL;
	node *dest = NULL;

	// flag used to indicate if the program should terminate
	int quit = 0;

	// main program loop
	while (quit == 0) {

		// Ask the user for input and load it
		printf("Enter origin and destination (quit to exit): ");
		fgets(buf, BUFSIZE, stdin);

		// If user wants to exit
		if (strncmp("quit", buf, 4) == 0) {
			quit = 1;
			printf("Normal exit.\n");
		}
		// if two words were read correcctly
		else if (parse_map_line(buf, n1, n2) == 2) {

			// if one or more of the inputs doesnt exist
			// dont try to find a path
			if ((src = graph_find_node(g, n1)) == NULL || (dest = graph_find_node(g, n2)) == NULL) {
				printf("Usage: AIRPORT1 AIRPORT2\n\n");
			}

			// Try to find a path
			else {
				if (find_path(g, src, dest)) {
					printf("There is a path from %s to %s.\n\n", n1, n2);
				}

				else {
					printf("There is no path from %s to %s.\n\n", n1, n2);
				}
			}
		}
		// Wrong input
		else {
			printf("Usage: AIRPORT1 AIRPORT2\n\n");
		}
	}

	// return all memory and close the file
	free(n1);
	free(n2);
	free(buf);
	graph_kill(g);
	fclose(map_data);

	return 0;
}

/**
 * @brief Check that we got the right amount of args
 *        and can open the file given to read from
 *
 * @param argc The number of args
 * @param argv The file to be opened and read
 * @param map_data The FILE pointer used for reading the file
 */
void check_params(int argc, char *argv[], FILE **map_data) {

	// Correct amount of arguments
	if (argc != 2) {
		fprintf(stderr, "Please input correct file\n");
		exit(EXIT_FAILURE);
	}

	// Open the file for reading
	*map_data = fopen(argv[1], "r");
	if (*map_data == NULL) {
		fprintf(stderr, "Could not open file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Loads the graph structure from the given map data
 *
 * Handles duplicates by checking if they already exists in
 * the graph before inserting nodes.
 *
 * @param map_data FILE pointer containing the map file
 * @return The modified graph structure
 */
graph *load_graph(FILE *map_data) {

	// get the amount of routes to be loaded into the graph
	int num_of_routes = get_num_of_routes(map_data);

	// Every route will have two nodes,
	// all of which could be unique
	graph *g = graph_empty(num_of_routes * 2);

	// Allocate the needed variables for reading and assigning
	char *buf = calloc(BUFSIZE, sizeof(*buf));
	char *n1 = calloc(MAXNODENAME, sizeof(*n1));
	char *n2 = calloc(MAXNODENAME, sizeof(*n2));

	// Given the number of routes, read that many lines from the file
	for (int i = 1; i <= num_of_routes; i++) {

		// if we reach EOF early the num_of_routes variables was
		// incorrect and the map file is incorrect
		if (fgets(buf, BUFSIZE, map_data) == NULL) {
			fprintf(stderr, "Input file doesnt follow the specification\n");
			exit(EXIT_FAILURE);
		}

		// We dont want to count this line
		if (line_is_blank(buf) || line_is_comment(buf)) {
			i--;
		}
		else {
			// Read the current line and extract the route names
			parse_map_line(buf, n1, n2);

			// Only add the nodes if they aren't in the graph already
			if (graph_find_node(g, n1) == NULL) {
				graph_insert_node(g, n1);
			}
			if (graph_find_node(g, n2) == NULL) {
				g = graph_insert_node(g, n2);
			}

			// Add the edge between these two nodes
			g = graph_insert_edge(g, graph_find_node(g, n1), graph_find_node(g, n2));
		}
	}

	// Free the memory allocated and return
	// the modified graph g
	free(buf);
	free(n1);
	free(n2);
	return g;
}

/**
 * @brief Reads the file for a the number specifying
 *        the amount of routes the file contains
 *
 * @param map_data FILE pointer to the map data
 *                 which we are to read from
 *
 * @pre We assume that there will be no blank space
 *      before the number in the file
 *
 * @return an integer with the number of routes in the file
 */
int get_num_of_routes(FILE *map_data) {

	// Statically allocated memory
	char buf[BUFSIZE];
	int num = 0;

	while (num == 0) {
		// if we reach EOF before reading a number on the first
		// character of the line
		if (fgets(buf, BUFSIZE, map_data) == NULL) {
			fprintf(stderr, "Input file doesnt follow the specification\n");
			exit(EXIT_FAILURE);
		}

		// Checks if the first character in the string is a
		// number representing the number of routes
		if (buf[0] >= '0' && buf[0] <= '9') {
			sscanf(buf, "%d", &num);
		}
	}

	// return the number found
	return num;
}

/**
 * @brief Uses a breadth-first search algorithm to determine
 *        if there is a path connecting two nodes in the graph
 *
 * @param g A graph structure
 * @param src The airport we are to depart from
 * @param dest The airport we try to reach
 *
 * @return true if we can reach dest from src, false if we cannot
 */
bool find_path(graph *g, node *src, node *dest) {

	// There will always be a route to and from the same airport
	if (nodes_are_equal(src, dest)) {
		return true;
	}

	// Breadth-first search
	g = graph_node_set_seen(g, src, true);
	// Add the src node first in the queue
	queue *q = queue_enqueue(queue_empty(NULL), src);
	node *n;
	node *b;
	dlist *neighbours = NULL;
	dlist_pos p = NULL;
	int found = 0;

	// Loop through all nodes reachable via src or until we have found dest
	while (!queue_is_empty(q) && found == 0) {

		// Get the node first in the queue,
		// delete it from the queue
		n = queue_front(q);
		q = queue_dequeue(q);

		// Get its neighbours
		neighbours = graph_neighbours(g, n);
		p = dlist_first(neighbours);

		// Loop over the neighbours until it we either find
		// the destination or we run out of neighbours
		while (!dlist_is_end(neighbours, p) && found == 0) {
			b = dlist_inspect(neighbours, p);

			if (nodes_are_equal(b, dest)) {
				// Update the flag to end the loop
				found = 1;
			}
			else {
				// if this is a new node, make it seen
				// and add it to the queue
				if (!graph_node_is_seen(g, b)) {
					graph_node_set_seen(g, b, true);
					q = queue_enqueue(q, b);
				}
			}
			// Update the position in the list
			p = dlist_next(neighbours, p);
		}
		// free memory for neigbours
		dlist_kill(neighbours);
	}

	// Free the memory for the queue
	queue_kill(q);

	// Reset graph for next search
	graph_reset_seen(g);

	// Return the results
	if (found == 1) {
		return true;
	}
	else {
		return false;
	}
}

/**
 * @brief Find position of first non-whitespace character.
 *
 * @param s - string to search.
 * @return int - Returns the position of the first non-whitespace character, or -1 if not found.
 */
int first_non_white_space(const char *s) {
	int i = 0; // Start at first char.

	// Advance as long as we're loooking at white-space until we hit EOL.
	while (s[i] && isspace(s[i])) {
		i++;
	}
	// We found a non-whitespace char.
	if (s[i]) {
		// It was a proper characted. Return its position.
		return i;
	}
	else {
		// It was the null terminator. Return fail.
		return -1;
	}
}

/**
 * @brief Determine if the string is blank.
 *
 * @param s - string to search.
 * @return true if the line contains only whitespace chars.
 * @return false if the line contains at least one non-whitespace char.
 */
bool line_is_blank(const char *s) {
	// Line is blank if it only contained white-space chars.
	return first_non_white_space(s) < 0;
}

/**
 * @brief Determine if the string is a comment line.
 *
 * @param s - string to search.
 * @return true if the line is a comment line.
 * @return false if the line is not a comment line.
 *
 * A comment line has a hash sign '#' as the first non-whitespace char on the line.
 */
bool line_is_comment(const char *s) {
	int i = first_non_white_space(s);
	return (i >= 0 && s[i] == '#');
}

/**
 * @brief Extract node names from a line from the map file.
 *
 * @param buf - Input buffer.
 * @param n1 - Output buffer for the first node name. Must be at least MAXNODENAME+1 long.
 * @param n2 - Ditto for the second node name.
 * @return int - Returns the number of correctly parsed node names. If the return value is 2, both n1
 * and n2 contain node names. If the return value is less than 2, parsing of at least one node name
 * failed, in which case the content of n1 and n2 are undefined.
 */
int parse_map_line(const char *buf, char *n1, char *n2) {
	// Create a format string that will do the work.
	char fmt[20];
	// This will generate the format string " %40s %40s" if MAXNODENAME is 40.
	snprintf(fmt, sizeof(fmt), " %%%ds %%%ds", MAXNODENAME, MAXNODENAME);

	// sscanf does all the necessary parsing.
	// Node names must be separated by whitespace.
	// Whitespace before the first node name is allowed.
	// Anything after the second node name is ignored.
	int n = sscanf(buf, fmt, n1, n2);

	// The return value from sscanf contains the number of properly parsed format codes, i.e. the
	// number of node names.
	return n;
}
