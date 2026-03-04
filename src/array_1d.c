#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <array_1d.h>

/*
 * Implementation of a generic 1D array for the "Datastructures and
 * algorithms" courses at the Department of Computing Science, Umea
 * University.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *          Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *          Lars Karlsson (larsk@cs.umu.se)
 *
 * Based on earlier code by: Johan Eliasson (johane@cs.umu.se).
 *
 * Version information:
 *   v1.0  2018-01-28: First public version.
 *   v1.5  2024-03-13: Renamed free_* stuff to kill_*. Converted to 4-tabs.
 *   v2.0  2024-05-10: Added print_internal.
 */

// =========== INTERNAL DATA TYPES ============

struct array_1d {
    int low; // Low index limit.
    int high; // High index limit.
    int array_size; // Number of array elements.
    void **values; // Pointer to where the actual values are stored.
    kill_function kill_func;
};

// =========== INTERNAL FUNCTION IMPLEMENTATIONS ============

/**
 * array_1d_create() - Create an array without values.
 * @lo: low index limit.
 * @hi: high index limit.
 * @kill_func: A pointer to a function (or NULL) to be called to
 *             de-allocate memory on remove/kill.
 *
 * The index limits are inclusive, i.e. all indices i such that low <=
 * i <= high are defined.
 *
 * Returns: A pointer to the new array, or NULL if not enough memory
 * was available.
 */
array_1d *array_1d_create(int lo, int hi, kill_function kill_func)
{
    // Allocate array structure.
    array_1d *a = calloc(1, sizeof(*a));
    // Store index limit.
    a->low = lo;
    a->high = hi;

    // Number of elements.
    a->array_size = hi - lo + 1;

    // Store kill function.
    a->kill_func = kill_func;

    a->values = calloc(a->array_size, sizeof(void *));

    // Check whether the allocation succeeded.
    if (a->values == NULL) {
        free(a);
        a = NULL;
    }
    return a;
}

/**
 * array_1d_low() - Return the low index limit for the array.
 * @a: array to inspect.
 *
 * Returns: The low index limit.
 */
int array_1d_low(const array_1d *a)
{
    return a->low;
}

/**
 * array_1d_high() - Return the high index limit for the array.
 * @a: array to inspect.
 *
 * Returns: The high index limit.
 */
int array_1d_high(const array_1d *a)
{
    return a->high;
}

/**
 * array_1d_inspect_value() - Inspect a value at a given array position.
 * @a: array to inspect.
 * @i: index of position to inspect.
 *
 * Returns: The element value at the specified position. The result is
 *          undefined if no value are stored at that position.
 */
void *array_1d_inspect_value(const array_1d *a, int i)
{
    int offset = i - array_1d_low(a);
    // Return the value.
    return a->values[offset];
}

/**
 * array_1d_has_value() - Check if a value is set at a given array position.
 * @a: array to inspect.
 * @i: index of position to inspect.
 *
 * Returns: True if a value is set at the specified position, otherwise false.
 */
bool array_1d_has_value(const array_1d *a, int i)
{
    int offset = i - array_1d_low(a);
    // Return true if the value is not NULL.
    return a->values[offset] != NULL;
}

/**
 * array_1d_set_value() - Set a value at a given array position.
 * @a: array to modify.
 * @v: value to set element to, or NULL to clear value.
 * @i: index of position to modify.
 *
 * If the old element value is non-NULL, calls kill_func if it was
 * specified at array creation.
 *
 * Returns: Nothing.
 */
void array_1d_set_value(array_1d *a, void *v, int i)
{
    int offset = i - array_1d_low(a);
    // Call kill_func if specified and old element value was non-NULL.
    if (a->kill_func != NULL && a->values[offset] != NULL) {
        a->kill_func( a->values[offset] );
    }
    // Set value.
    a->values[offset] = v;
}

/**
 * array_1d_kill() - Return memory allocated by array.
 * @a: array to kill.
 *
 * Iterates over all elements. If kill_func was specified at array
 * creation, calls it for every non-NULL element value.
 *
 * Returns: Nothing.
 */
void array_1d_kill(array_1d *a)
{
    if (a->kill_func != NULL) {
        // Return user-allocated memory for each non-NULL element.
        for (int i = 0; i < a->array_size; i++) {
            if (a->values[i] != NULL) {
                a->kill_func(a->values[i]);
            }
        }
    }
    // Return memory for value pointers.
    free(a->values);
    // Return memory for array structure.
    free(a);
}

/**
 * array_1d_print() - Iterate over the array element and print their values.
 * @a: Array to inspect.
 * @print_func: Function called for each non-NULL element.
 *
 * Iterates over each position in the array. Calls print_func for each
 * non-NULL value.
 *
 * Returns: Nothing.
 */
void array_1d_print(const array_1d *a, inspect_callback print_func)
{
    printf("[ ");
    for (int i=array_1d_low(a); i<=array_1d_high(a); i++) {
        if (array_1d_has_value(a, i)) {
            printf("[");
            print_func(array_1d_inspect_value(a, i));
            printf("]");
        } else {
            printf(" []");
        }
        if (i < array_1d_high(a)) {
            printf(", ");
        }
    }
    printf(" ]\n");
}

// ===========INTERNAL FUNCTIONS USED BY array_1d_print_internal ============

// The functions below output code in the dot language, used by
// GraphViz. For documention of the dot language, see graphviz.org.

/**
 * indent() - Output indentation string.
 * @n: Indentation level.
 *
 * Print n tab characters.
 *
 * Returns: Nothing.
 */
static void indent(int n)
{
    for (int i=0; i<n; i++) {
        printf("\t");
    }
}
/**
 * iprintf(...) - Indent and print.
 * @n: Indentation level
 * @...: printf arguments
 *
 * Print n tab characters and calls printf.
 *
 * Returns: Nothing.
 */
static void iprintf(int n, const char *fmt, ...)
{
    // Indent...
    indent(n);
    // ...and call printf
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

/**
 * print_edge() - Print a edge between two addresses.
 * @from: The address of the start of the edge. Should be non-NULL.
 * @to: The address of the destination for the edge, including NULL.
 * @port: The name of the port on the source node, or NULL.
 * @label: The label for the edge, or NULL.
 * @options: A string with other edge options, or NULL.
 *
 * Print an edge from port PORT on node FROM to TO with label
 * LABEL. If to is NULL, the destination is the NULL node, otherwise a
 * memory node. If the port is NULL, the edge starts at the node, not
 * a specific port on it. If label is NULL, no label is used. The
 * options string, if non-NULL, is printed before the label.
 *
 * Returns: Nothing.
 */
static void print_edge(int indent_level, const void *from, const void *to, const char *port,
                       const char *label, const char *options)
{
    indent(indent_level);
    if (port) {
        printf("m%04lx:%s -> ", PTR2ADDR(from), port);
    } else {
        printf("m%04lx -> ", PTR2ADDR(from));
    }
    if (to == NULL) {
        printf("NULL");
    } else {
        printf("m%04lx", PTR2ADDR(to));
    }
    printf(" [");
    if (options != NULL) {
        printf("%s", options);
    }
    if (label != NULL) {
        printf(" label=\"%s\"",label);
    }
    printf("]\n");
}

/**
 * print_head_node() - Print a node corresponding to the array head struct.
 * @indent_level: Indentation level.
 * @a: Array to inspect.
 *
 * Returns: Nothing.
 */
static void print_head_node(int indent_level, const array_1d *a)
{
    iprintf(indent_level, "m%04lx [shape=record "
            "label=\"kill\\n%04lx|low\\n%d|high\\n%d|array_size\\n%d|<v>values\\n%04lx\"]\n",
            PTR2ADDR(a), PTR2ADDR(a->kill_func), a->low, a->high, a->array_size,
            PTR2ADDR(a->values));
}

void print_values(int indent_level, const array_1d *a)
{
    iprintf(indent_level, "m%04lx [shape=record label=\"", PTR2ADDR(a->values));
    for (int i=0; i < a->array_size; i++) {
        printf("<%02d>%d\\n%02d\\n%04lx", i, i + a->low, i, PTR2ADDR(a->values[i]));
        if (i < a->array_size - 1) {
            printf("|");
        }
    }
    printf("\"]\n");
}

// Print edge from the array head to the values array.
static void print_head_edge(int indent_level, const array_1d *a)
{
    iprintf(indent_level, "m%04lx:v -> ", PTR2ADDR(a));
    if (a->values == NULL) {
        printf("NULL");
    } else {
        printf("m%04lx", PTR2ADDR(a->values));
    }
    printf(" [label=\"values\"]\n");
}

// Print nodes for each value memory block
static void print_value_nodes(int indent_level, const array_1d *a, inspect_callback print_func)
{
    for (int i=0; i <= a->array_size; i++) {
        if (a->values[i] != NULL) {
            iprintf(indent_level, "m%04lx [label=\"", PTR2ADDR(a->values[i]));
            if (print_func != NULL) {
                print_func(a->values[i]);
            }
            printf("\" xlabel=\"%04lx\"]\n", PTR2ADDR(a->values[i]));
        }
    }
}

// Print edges from each value pointer to payload memory
static void print_value_edges(int indent_level, const array_1d *a)
{
    for (int i=0; i < a->array_size; i++) {
        // Buffer to store port name in. Good for array up to 1e9 elements.
        char port[15];
        // Create port name
        sprintf(port, "%02d", i);
        if (a->kill_func) {
            print_edge(indent_level, a->values, a->values[i], port, port,
                       "color=red");
        } else {
            print_edge(indent_level, a->values, a->values[i], port, port,
                       "color=red style=dashed");
        }
    }
}

// Create an escaped version of the input string. The most common
// control characters - newline, horizontal tab, backslash, and double
// quote - are replaced by their escape sequence. The returned pointer
// must be deallocated by the caller.
static char *escape_chars(const char *s)
{
    int i, j;
    int escaped = 0; // The number of chars that must be escaped.

    // Count how many chars need to be escaped, i.e. how much longer
    // the output string will be.
    for (i = escaped = 0; s[i] != '\0'; i++) {
        if (s[i] == '\n' || s[i] == '\t' || s[i] == '\\' || s[i] == '\"') {
            escaped++;
        }
    }
    // Allocate space for the escaped string. The variable i holds the input
    // length, escaped how much the string will grow.
    char *t = malloc(i + escaped + 1);

    // Copy-and-escape loop
    for (i = j = 0; s[i] != '\0'; i++) {
        // Convert each control character by its escape sequence.
        // Non-control characters are copied as-is.
        switch (s[i]) {
        case '\n': t[i+j] = '\\'; t[i+j+1] = 'n';  j++; break;
        case '\t': t[i+j] = '\\'; t[i+j+1] = 't';  j++; break;
        case '\\': t[i+j] = '\\'; t[i+j+1] = '\\'; j++; break;
        case '\"': t[i+j] = '\\'; t[i+j+1] = '\"'; j++; break;
        default:   t[i+j] = s[i]; break;
        }
    }
    // Terminal the output string
    t[i+j] = '\0';
    return t;
}

/**
 * array_1d_print_internal() - Print the internal structure of the array in dot format.
 * @a: Array to inspect.
 * @print_func: Function called for each element value.
 * @desc: String with a description/state of the array, or NULL for no description.
 * @indent_level: Indentation level, 0 for outermost
 *
 * Iterates over the array and outputs dot code that shows the internal
 * structure of the array.  The dot code can be visualized by
 * Graphviz.
 *
 * On linux system, the output can be parsed by the dot program, e.g.
 *
 *   <array_program> | dot -Tsvg > /tmp/dot.svg; firefox /tmp/dot.svg
 *
 * where <array_program> is the name of the executable
 *
 * The output may also be possible to visualize online on
 * https://dreampuf.github.io/GraphvizOnline/ or google "graphviz
 * online".
 *
 * For documention of the dot language, see graphviz.org.
 *
 * Returns: Nothing.
 */
void array_1d_print_internal(const array_1d *a, inspect_callback print_func, const char *desc,
                             int indent_level)
{
    static int graph_number = 0;
    graph_number++;
    int il = indent_level;

    if (indent_level == 0) {
        // If this is the outermost datatype, start a graph...
        printf("digraph ARRAY_1D_%d {\n", graph_number);

        // Specify default shape and fontname
        il++;
        iprintf(il, "node [shape=rectangle fontname=\"Courier New\"]\n");
        iprintf(il, "ranksep=0.01\n");
        iprintf(il, "subgraph cluster_nullspace {\n");
        iprintf(il+1, "NULL\n");
        iprintf(il, "}\n");
    }

    if (desc != NULL) {
        // Escape the string before printout
        char *escaped = escape_chars(desc);
        // Use different names on inner description nodes
        if (indent_level == 0) {
            iprintf(il, "description [label=\"%s\"]\n", escaped);
        } else {
            iprintf(il, "cluster_array_1d_%d_description [label=\"%s\"]\n", graph_number,
                    escaped);
        }
        // Return the memory used by the escaped string
        free(escaped);
    }

    if (indent_level == 0) {
        // Use a single "pointer" edge as a starting point for the
        // outermost datatype
        iprintf(il, "a [label=\"%04lx\" xlabel=\"a\"]\n", PTR2ADDR(a));
        iprintf(il, "a -> m%04lx\n", PTR2ADDR(a));
    }

    // Print the subgraph to surround the Array content
    iprintf(il, "subgraph cluster_array_1d_%d { label=\"Array_1d\"\n", graph_number);
    il++;

    // Output the head node
    print_head_node(il, a);

    // Output the values array
    print_values(il, a);

    // Close the subgraph
    il--;
    iprintf(il, "}\n");

    if (indent_level == 0) {
        // Put the user nodes in userspace
        iprintf(il, "subgraph cluster_userspace { label=\"User space\"\n");
        il++;
    }

    // Print nodes for each value memory block
    print_value_nodes(il, a, print_func);

    if (indent_level == 0) {
        // Close userspace
        il--;
        iprintf(il, "}\n");
    }

    // Output the edges from the head
    print_head_edge(il, a);

    // Print edges from each value pointer to payload memory
    print_value_edges(il, a);

    if (indent_level == 0) {
        // Termination of graph
        printf("}\n");
    }
}
