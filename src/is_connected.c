#include <stdio.h>
#include <stdlib.h>

#include <graph.h>

void check_params(int argc, char* argv[], FILE** map_data);
graph* load_graph(FILE* map_data);
bool find_path(graph* g, node* src, node* dest);


int main(int argc, char* argv[]) {

	printf("%d\n", argc);

	FILE* map_data = NULL;
	check_params(argc, argv, &map_data);

	graph* g = malloc(sizeof(&g));
	g = load_graph(map_data);

	free(g);
	fclose(map_data);

	return 0;
}

void check_params(int argc, char* argv[], FILE** map_data) {

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

graph* load_graph(FILE* map_data) {
	graph* g = NULL;

	char string[100];


	fgets(string, 99, map_data);
	printf("%s\n", string);

	return g;
}

bool find_Path(graph* g, node* src, node* dest) {

	return true;
}
