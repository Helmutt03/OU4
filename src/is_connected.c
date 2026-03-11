#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include <graph.h>
#include <queue.h>

#define MAXNODENAME 40
#define BUFSIZE 400

void check_params(int argc, char *argv[], FILE **map_data);
graph *load_graph(FILE *map_data);
int get_num_of_routes(FILE *map_data);
bool find_path(graph *g, node *src, node *dest);

// Helper functions
int first_non_white_space(const char *s);
bool line_is_blank(const char *s);
bool line_is_comment(const char *s);
int parse_map_line(const char *buf, char *n1, char *n2);


int main(int argc, char *argv[]) {

	// Get the map data
	FILE *map_data = NULL;
	check_params(argc, argv, &map_data);

	// load the graph structure with all routes
	graph *g = load_graph(map_data);

	int quit = 0;
	char *buf = malloc(BUFSIZE * sizeof(*buf));
	char *n1 = malloc(MAXNODENAME * sizeof(*n1));
	char *n2 = malloc(MAXNODENAME * sizeof(*n1));
	node *src = NULL;
	node *dest = NULL;

	while (quit == 0) {

		printf("Enter origin and destination (quit to exit): ");

		fgets(buf, BUFSIZE, stdin);

		if (strncmp("quit", buf, 4) == 0) {
			quit = 1;
			printf("Normal exit.\n");
		}
		else if (parse_map_line(buf, n1, n2) == 2) {

			// if one or more of the inputs doesnt exist
			// dont try to find a path
			if ((src = graph_find_node(g, n1)) == NULL || (dest = graph_find_node(g, n2)) == NULL) {
				fprintf(stderr, "Please input correct airports\n\n");
			}
			else {
				if (find_path(g, src, dest)) {
					printf("There is a path from %s to %s\n\n", n1, n2);
					graph_reset_seen(g);
				}
				else {
					printf("There is no path from %s to %s\n\n", n1, n2);
					graph_reset_seen(g);
				}
			}
		}
		else {
			printf("Usage: AIRPORT1 AIRPORT2\n\n");
		}
	}

	free(n1);
	free(n2);
	free(buf);
	free(g);
	fclose(map_data);

	return 0;
}

void check_params(int argc, char *argv[], FILE **map_data) {

	if (argc != 2) {
		printf("Please input correct file\n");
		exit(EXIT_FAILURE);
	}

	*map_data = fopen(argv[1], "r");
	if (map_data == NULL) {
		fprintf(stderr, "Could not open file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
}

graph *load_graph(FILE *map_data) {

	// get the amount of routes to be loaded into the graph
	int num_of_routes = get_num_of_routes(map_data);

	// Every route will have two nodes
	graph *g = graph_empty(num_of_routes * 2);

	char *buf = malloc(BUFSIZE * sizeof(*buf));
	char *n1 = malloc(MAXNODENAME * sizeof(*n1));
	char *n2 = malloc(MAXNODENAME * sizeof(*n2));

	for (int i = 1; i <= num_of_routes; i++) {

		// if we reach the EOF early the num_of_routes number was
		// incorrect and the map file is incorrect
		if (fgets(buf, BUFSIZE, map_data) == NULL) {
			fprintf(stderr, "something wrong...\n");
			exit(EXIT_FAILURE);
		}

		if (line_is_blank(buf) || line_is_comment(buf)) {
			i--;
		}
		else {
			// Read the current line and extract the route names
			parse_map_line(buf, n1, n2);

			// debug
			printf("%s-%s\n", n1, n2);

			// Only add the nodes if they aren't in the graph already
			if (graph_find_node(g, n1) == NULL) {
				graph_insert_node(g, n1);
			}
			if (graph_find_node(g, n2) == NULL) {
				g = graph_insert_node(g, n2);
			}

			// could do this differently and save the pointers above if found so we avoid doing find_node if on or
			// more of the nodes already are in the graph
			g = graph_insert_edge(g, graph_find_node(g, n1), graph_find_node(g, n2));
		}
	}

	free(buf);
	free(n1);
	free(n2);
	return g;
}

int get_num_of_routes(FILE *map_data) {
	// We assume that there will be no blank space before the number,
	char buf[BUFSIZE];
	int num = 0;

	while (num == 0) {
		if (fgets(buf, BUFSIZE, map_data) == NULL) {
			fprintf(stderr, "Input file doesnt follow the specification\n");
			exit(EXIT_FAILURE);
		}

		// Checks if the first character in the string is a
		// number representing the number of routes
		if (buf[0] >= '0' && buf[0] <= '9') {
			sscanf(buf, "%d", &num);
			printf("%s", buf);
		}
	}

	return num;
}

bool find_path(graph *g, node *src, node *dest) {

	if (nodes_are_equal(src, dest)) {
		return true;
	}


	graph_node_set_seen(g, src, true);
	queue *q = queue_enqueue(queue_empty(NULL), src);
	node *n;
	node *b;
	dlist *neighbours = NULL;
	dlist_pos p = NULL;

	while (!queue_is_empty(q)) {

		n = queue_front(q);
		q = queue_dequeue(q);

		// Get its neighbours
		neighbours = graph_neighbours(g, n);
		p = dlist_first(neighbours);

		while (!dlist_is_end(neighbours, p)) {
			b = dlist_inspect(neighbours, p);

			// need to change this when freeing memory
			// possibly just use a flag so we
			// can free the memory last and check
			// if the flag is 1 and return true
			// else return false
			if (nodes_are_equal(b, dest)) {
				return true;
			}
			else {
				if (!graph_node_is_seen(g, b)) {
					graph_node_set_seen(g, b, true);
					q = queue_enqueue(q, b);
				}
			}
			p = dlist_next(neighbours, p);
		}
	}
	dlist_kill(neighbours);

	return false;
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
