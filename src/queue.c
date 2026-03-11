#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <queue.h>
#include <list.h>

/*
 * Implementation of a generic queue for the "Datastructures and
 * algorithms" courses at the Department of Computing Science, Umea
 * University.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *          Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *
 * Based on earlier code by: Johan Eliasson (johane@cs.umu.se).
 *
 * Version information:
 *   v1.0  2018-01-28: First public version.
 *   v1.01 2024-01-16: Fix include to be with brackets, not citation marks.
 *   v2.0  2024-03-14: Added queue_print_internal to output dot code for visualization.
 *                     Renamed free_* stuff to kill_*. Converted to 4-tabs.
 *   v2.1  2024-05-10: Updated print_internal with improved encapsulation.
 */

// ===========INTERNAL DATA TYPES ============

/*
 * The queue is implemented using the list abstract datatype. Almost
 * everything is done by the list.
 */

struct queue {
    list *elements;
};

// ===========INTERNAL FUNCTION IMPLEMENTATIONS ============

/**
 * queue_empty() - Create an empty queue.
 * @kill_func: A pointer to a function (or NULL) to be called to
 *             de-allocate memory on remove/kill.
 *
 * Returns: A pointer to the new queue.
 */
queue *queue_empty(kill_function kill_func)
{
    // Allocate the queue head.
    queue *q = calloc(1, sizeof(*q));
    // Create an empty list.
    q->elements = list_empty(kill_func);

    return q;
}

/**
 * queue_is_empty() - Check if a queue is empty.
 * @q: Queue to check.
 *
 * Returns: True if queue is empty, otherwise false.
 */
bool queue_is_empty(const queue *q)
{
    return list_is_empty(q->elements);
}

/**
 * queue_enqueue() - Put a value at the end of the queue.
 * @q: Queue to manipulate.
 * @v: Value (pointer) to be put in the queue.
 *
 * Returns: The modified queue.
 */
queue *queue_enqueue(queue *q, void *v)
{
    list_insert(q->elements, v, list_end(q->elements));
    return q;
}

/**
 * queue_dequeue() - Remove the element at the front of a queue.
 * @q: Queue to manipulate.
 *
 * NOTE: Undefined for an empty queue.
 *
 * Returns: The modified queue.
 */
queue *queue_dequeue(queue *q)
{
    list_remove(q->elements, list_first(q->elements));
    return q;
}

/**
 * queue_front() - Inspect the value at the front of the queue.
 * @q: Queue to inspect.
 *
 * Returns: The value at the top of the queue.
 *          NOTE: The return value is undefined for an empty queue.
 */
void *queue_front(const queue *q)
{
    return list_inspect(q->elements, list_first(q->elements));
}

/**
 * queue_kill() - Destroy a given queue.
 * @q: Queue to destroy.
 *
 * Return all dynamic memory used by the queue and its elements. If a
 * kill_func was registered at queue creation, also calls it for each
 * element to kill any user-allocated memory occupied by the element values.
 *
 * Returns: Nothing.
 */
void queue_kill(queue *q)
{
    list_kill(q->elements);
    free(q);
}

/**
 * queue_print() - Iterate over the queue elements and print their values.
 * @q: Queue to inspect.
 * @print_func: Function called for each element.
 *
 * Iterates over the queue and calls print_func with the value stored
 * in each element.
 *
 * Returns: Nothing.
 */
void queue_print(const queue *q, inspect_callback print_func)
{
    printf("{ ");
    list_pos pos = list_first(q->elements);
    while (pos != list_end(q->elements)) {
        print_func(list_inspect(q->elements, pos));
        pos = list_next(q->elements, pos);
        if (pos != list_end(q->elements)) {
            printf(", ");
        }
    }
    printf(" }\n");
}

// ===========INTERNAL FUNCTIONS USED BY queue_print_internal ============

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
 * print_head_node() - Print a node corresponding to the queue head struct.
 * @il: Indentation level.
 * @q: Queue to inspect.
 *
 * Returns: Nothing.
 */
static void print_head_node(int indent_level, const queue *q)
{
    iprintf(indent_level, "m%04lx [shape=record "
            "label=\"<e>elements\\n%04lx\" xlabel=\"%04lx\"]\n",
            PTR2ADDR(q), PTR2ADDR(q->elements), PTR2ADDR(q));
}

// Print edges from the queue head to the elements list.
static void print_head_edge(int indent_level, const queue *q)
{
    print_edge(indent_level, q, q->elements, "e", "elements", NULL);
}

// Print the node for the memory block at p using the user-supplied
// print_func to print the label.
static void print_value_node(int indent_level, const void *p, inspect_callback print_func)
{
    iprintf(indent_level, "m%04lx [label=\"", PTR2ADDR(p));
    if (print_func != NULL) {
        print_func(p);
    }
    printf("\" xlabel=\"%04lx\"]\n", PTR2ADDR(p));
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
 * queue_print_internal() - Print the queues internal structure in dot format.
 * @l: Queue to inspect.
 * @print_func: Function called for each element value.
 * @desc: String with a description/state of the queue, or NULL for no description.
 * @indent_level: Indentation level, 0 for outermost
 *
 * Iterates over the queue and outputs dot code that shows the internal
 * structure of the queue.  The dot code can be visualized by
 * Graphviz.
 *
 * On linux system, the output can be parsed by the dot program, e.g.
 *
 *   <queue_program> | dot -Tsvg > /tmp/dot.svg; firefox /tmp/dot.svg
 *
 * where <queue_program> is the name of the executable
 *
 * The output may also be possible to visualize online on
 * https://dreampuf.github.io/GraphvizOnline/ or google "graphviz
 * online".
 *
 * For documention of the dot language, see graphviz.org.
 *
 * Returns: Nothing.
 */
void queue_print_internal(const queue *q, inspect_callback print_func, const char *desc,
                          int indent_level)
{
    static int graph_number = 0;
    graph_number++;
    int il = indent_level;

    if (indent_level == 0) {
        // If this is the outermost datatype, start a graph...
        printf("digraph QUEUE_%d {\n", graph_number);

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
            iprintf(il, "\tcluster_queue_%d_description [label=\"%s\"]\n", graph_number, escaped);
        }
        // Return the memory used by the escaped string
        free(escaped);
    }

    if (indent_level == 0) {
        // Use a single "pointer" edge as a starting point for the
        // outermost datatype
        iprintf(il, "q [label=\"%04lx\" xlabel=\"q\"]\n", PTR2ADDR(q));
        iprintf(il, "q -> m%04lx\n", PTR2ADDR(q));
    }

    // Output the payload nodes before asking the list to print them
    // to render them in their own subgraph.

    if (indent_level == 0) {
        // Put the user nodes in userspace
        iprintf(il, "subgraph cluster_userspace { label=\"User space\"\n");
        il++;

        // Iterate over the list to print the payload nodes
        list_pos p = list_first(q->elements);
        while (!list_pos_is_equal(q->elements, p, list_end(q->elements))) {
            print_value_node(il, list_inspect(q->elements, p), print_func);
            // Advance
            p = list_next(q->elements, p);
        }

        // Close the subgraph
        il--;
        iprintf(il, "}\n");
    }

    // Print the subgraph to surround the DList content
    iprintf(il, "subgraph cluster_queue_%d { label=\"Queue\"\n", graph_number);
    il++;

    // Output the head node
    print_head_node(il, q);

    if (q->elements) {
        list_print_internal(q->elements, print_func, NULL, il);
    }

    // Close the subgraph
    il--;
    iprintf(il, "}\n");

    // Output the edges from the head
    print_head_edge(il, q);

    if (indent_level == 0) {
        // Termination of graph
        printf("}\n");
    }
}
