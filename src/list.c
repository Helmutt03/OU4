#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <list.h>

/*
 * Implementation of a generic, undirected list for the
 * "Datastructures and algorithms" courses at the Department of
 * Computing Science, Umea University.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *          Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *          Lars Karlsson (larsk@cs.umu.se)
 *
 * Based on earlier code by: Johan Eliasson (johane@cs.umu.se).
 *
 * Version information:
 *   v1.0  2018-01-28: First public version.
 *   v1.1  2023-01-19: Added list_pos_equal and list_pos_is_valid functions.
 *   v1.2  2023-01-20: Renamed list_pos_equal to list_pos_are_equal.
 *   v1.3  2023-02-25: Renamed list header fields head and tail.
 *   v1.4  2023-03-23: Renamed list_pos_are_equal to list_pos_is_equal.
 *   v1.5  2024-03-13: Renamed free_* stuff to kill_*. Converted to 4-tabs.
 *   v2.0  2024-03-14: Added list_print_internal to output dot code for visualization.
 *   v2.1  2024-05-10: Added/updated print_internal with improved encapsulation.
 */

// ===========INTERNAL DATA TYPES ============

/*
 * The list elements are implemented as two-cells with forward and
 * backward links and a void * for the value. The list uses two border
 * cells at the start and end of the list.
 */
typedef struct cell {
    struct cell *next;
    struct cell *prev;
    void *val;
} cell;

struct list {
    cell *head;
    cell *tail;
    kill_function kill_func;
};

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * list_empty() - Create an empty list.
 * @kill_func: A pointer to a function (or NULL) to be called to
 *             de-allocate memory on remove/kill.
 *
 * Returns: A pointer to the new list.
 */
list *list_empty(kill_function kill_func)
{
    // Allocate memory for the list head.
    list *l = calloc(1, sizeof(list));

    // Allocate memory for the border cells.
    l->head = calloc(1, sizeof(cell));
    l->tail = calloc(1, sizeof(cell));

    // Set consistent links between border elements.
    l->head->next = l->tail;
    l->tail->prev = l->head;

    // Store the kill function.
    l->kill_func = kill_func;

    return l;
}

/**
 * list_is_empty() - Check if a list is empty.
 * @l: List to check.
 *
 * Returns: True if the list is empty, otherwise false.
 */
bool list_is_empty(const list *l)
{
    // List is empty if there are no cells between head and tail.
    return (l->head->next == l->tail);
}

/**
 * list_first() - Return the first position of a list, i.e. the
 *                position of the first element in the list.
 * @l: List to inspect.
 *
 * Returns: The first position in the given list.
 */
list_pos list_first(const list *l)
{
    // First position is position of first element.
    return l->head->next;
}

/**
 * list_end() - Return the last position of a list, i.e. the position
 *              after the last element in the list.
 * @l: List to inspect.
 *
 * Returns: The last position in the given list.
 */
list_pos list_end(const list *l)
{
    // Last position is position *after* last element.
    return l->tail;
}

/**
 * list_next() - Return the next position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the last in the list.
 *
 * Returns: The position in the list after the given position.
 *          NOTE: The return value is undefined for the last position.
 */
list_pos list_next(const list *l, const list_pos p)
{
    if (list_pos_is_equal(l, p, list_end(l))) {
        // This should really throw an error.
        fprintf(stderr,"list_next: Warning: Trying to navigate past end of list!");
    }
    return p->next;
}

/**
 * list_prev() - Return the previous position in a list.
 * @l: List to inspect.
 * @p: Any valid position except the first in the list.
 *
 * Returns: The position in the list before the given position.
 *          NOTE: The return value is undefined for the first position.
 */
list_pos list_prev(const list *l, const list_pos p)
{
    if (list_pos_is_equal(l, p, list_first(l))) {
        // This should really throw an error.
        fprintf(stderr,"list_prev: Warning: Trying to navigate "
                "past beginning of list!\n");
    }
    return p->prev;
}

/**
 * list_inspect() - Return the value of the element at a given
 *                  position in a list.
 * @l: List to inspect.
 * @p: Any valid position in the list, except the last.
 *
 * Returns: The value at the given position as a void pointer.
 *          NOTE: The return value is undefined for the last position.
 */
void *list_inspect(const list *l, const list_pos p)
{
    if (list_pos_is_equal(l, p, list_end(l))) {
        // This should really throw an error.
        fprintf(stderr,"list_inspect: Warning: Trying to inspect "
                "position at end of list!\n");
    }
    return p->val;
}

/**
 * list_insert() - Insert a new element with a given value into a list.
 * @l: List to manipulate.
 * @v: Value (pointer) to be inserted into the list.
 * @p: Position in the list before which the value should be inserted.
 *
 * Creates a new element and inserts it into the list before p.
 * Stores data in the new element.
 *
 * Returns: The position of the newly created element.
 */
list_pos list_insert(list *l, void *v, const list_pos p)
{
    // Allocate memory for a new cell.
    list_pos e = malloc(sizeof(cell));

    // Store the value.
    e->val = v;
    // Add links to/from the new cell.
    e->next = p;
    e->prev = p->prev;
    p->prev = e;
    e->prev->next = e;

    // Return the position of the new cell.
    return e;
}

/**
 * list_remove() - Remove an element from a list.
 * @l: List to manipulate.
 * @p: Position in the list of the element to remove.
 *
 * Removes the element at position p from the list. If a kill_func
 * was registered at list creation, calls it to deallocate the memory
 * held by the element value.
 *
 * Returns: The position after the removed element.
 */
list_pos list_remove(list *l, const list_pos p)
{
    // Remember return position.
    list_pos next_pos = p->next;
    // Link past this element.
    p->prev->next = p->next;
    p->next->prev = p->prev;

    // Call kill_func if registered.
    if (l->kill_func != NULL) {
        // Return any user-allocated memory for the value.
        l->kill_func(p->val);
    }
    // Return the memory allocated to the cell itself.
    free(p);
    // Return the position of the next element.
    return next_pos;
}

/**
 * list_kill() - Destroy a given list.
 * @l: List to destroy.
 *
 * Returns all dynamic memory used by the list and its elements. If a
 * kill_func was registered at list creation, also calls it for each
 * element to return any user-allocated memory occupied by the element
 * values.
 *
 * Returns: Nothing.
 */
void list_kill(list *l)
{
    // Use public functions to traverse the list.

    // Start with the first element (will be defined even for an
    // empty list).
    list_pos p = list_first(l);

    // Remove first element until list is empty.
    while (!list_is_empty(l)) {
        p = list_remove(l, p);
    }

    // Free border elements and the list head.
    free(l->head);
    free(l->tail);
    free(l);
}

/**
 * list_print() - Iterate over the list element and print their values.
 * @l: List to inspect.
 * @print_func: Function called for each element.
 *
 * Iterates over the list and calls print_func with the value stored
 * in each element.
 *
 * Returns: Nothing.
 */
void list_print(const list *l, inspect_callback print_func)
{
    // Start at the beginning of the list.
    list_pos p = list_first(l);

    printf("( ");
    while (!list_pos_is_equal(l, p, list_end(l))) {
        // Call print_func with the element value at the
        // current position.
        print_func(list_inspect(l, p));
        // Advance to next position.
        p = list_next(l, p);
        // Print separator unless at end.
        if (!list_pos_is_equal(l, p, list_end(l))) {
            printf(", ");
        }
    }
    printf(" )\n");
}

/**
 * list_pos_is_equal() - Return true if two positions in a list are equal.
 * @l: List to inspect.
 * @p1: First position to compare.
 * @p2: Second position to compare.
 *
 * Returns: True if p1 and p2 refer to the same position in l, otherwise False.
 *          NOTE: The result is defined only if p1 and p2 are valid positions in l.
 */
bool list_pos_is_equal(const list *l, const list_pos p1, const list_pos p2)
{
    // Since we don't need to check whether p1 or p2 are valid, we
    // only need to compare them directly.
    return p1 == p2;
}

/**
 * list_pos_is_valid() - Return true for a valid position in a list
 * @l: List to inspect.
 * @p: Any position.
 *
 * Returns: True if p is a valid position in the list, otherwise false.
 */
bool list_pos_is_valid(const list *l, const list_pos p)
{
    // Iterate over all positions in l.
    list_pos q = list_first(l);
    while (!list_pos_is_equal(l, q, list_end(l))) {
        if (list_pos_is_equal(l, p, q)) {
            // We found the position in the list.
            return true;
        }
        // Advance to the next valid position,
        q = list_next(l, q);
    }
    // p was not among valid positions in l.
    return false;
}

// ===========INTERNAL FUNCTIONS USED BY dlist_print_internal ============

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
 * print_head_node() - Print a node corresponding to the list head struct.
 * @indent_level: Indentation level.
 * @l: List to inspect.
 *
 * Returns: Nothing.
 */
static void print_head_node(int indent_level, const list *l)
{
    iprintf(indent_level, "m%04lx [shape=record "
            "label=\"kill\\n%04lx|<h>head\\n%04lx|<t>tail\\n%04lx\" xlabel=\"%04lx\"]\n",
            PTR2ADDR(l), PTR2ADDR(l->kill_func), PTR2ADDR(l->head), PTR2ADDR(l->tail),
            PTR2ADDR(l));
}

// Print edges from the list head to the head and tail cells.
static void print_head_edges(int indent_level, const list *l)
{
    print_edge(indent_level, l, l->head, "h", "head", NULL);
    print_edge(indent_level, l, l->tail, "t", "tail", NULL);
}

// Print a node corresponding to the cell at position p.
static void print_elem_node(int indent_level, const list_pos p)
{
    iprintf(indent_level, "m%04lx [shape=record "
            "label=\"<v>val\\n%04lx|<n>next\\n%04lx|<p>prev\\n%04lx\" xlabel=\"%04lx\"]\n",
            PTR2ADDR(p), PTR2ADDR(p->val),  PTR2ADDR(p->next), PTR2ADDR(p->prev), PTR2ADDR(p));
}

// Print edges from the cell at position p to the next and previous
// cells and the value.  The value pointer is red, the others are
// black. If the list owns the memory, the edge is solid, otherwise
// dashed.
static void print_elem_edges(int indent_level, const list *l, const list_pos p)
{
    print_edge(indent_level, p, p->next, "n", "next", NULL);
    print_edge(indent_level, p, p->prev, "p", "prev", NULL);

    // Ignore val ptr for head and tail nodes.
    if (p == l->head || p == l->tail) {
        return;
    }
    if (l->kill_func) {
        print_edge(indent_level, p, p->val, "v", "val", "color=red");
    } else {
        print_edge(indent_level, p, p->val, "v", "val", "color=red style=dashed");
    }
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
 * list_print_internal() - Print the lists internal structure in dot format.
 * @l: List to inspect.
 * @print_func: Function called for each element value.
 * @desc: String with a description/state of the list, or NULL for no description.
 * @indent_level: Indentation level, 0 for outermost
 *
 * Iterates over the list and outputs dot code that shows the internal
 * structure of the list.  The dot code can be visualized by
 * Graphviz.
 *
 * On linux system, the output can be parsed by the dot program, e.g.
 *
 *   <list_program> | dot -Tsvg > /tmp/dot.svg; firefox /tmp/dot.svg
 *
 * where <list_program> is the name of the executable
 *
 * The output may also be possible to visualize online on
 * https://dreampuf.github.io/GraphvizOnline/ or google "graphviz
 * online".
 *
 * For documention of the dot language, see graphviz.org.
 *
 * Returns: Nothing.
 */
void list_print_internal(const list *l, inspect_callback print_func, const char *desc,
                         int indent_level)
{
    static int graph_number = 0;
    graph_number++;
    int il = indent_level;

    if (indent_level == 0) {
        // If this is the outermost datatype, start a graph...
        printf("digraph LIST_%d {\n", graph_number);

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
            iprintf(il, "\tcluster_list_%d_description [label=\"%s\"]\n", graph_number, escaped);
        }
        // Return the memory used by the escaped string
        free(escaped);
    }

    if (indent_level == 0) {
        // Use a single "pointer" edge as a starting point for the
        // outermost datatype
        iprintf(il, "l [label=\"%04lx\" xlabel=\"l\"]\n", PTR2ADDR(l));
        iprintf(il, "l -> m%04lx\n", PTR2ADDR(l));
    }

    // Print the subgraph to surround the DList content
    iprintf(il, "subgraph cluster_list_%d { label=\"List\"\n", graph_number);
    il++;

    // Output the head node
    print_head_node(il, l);

    // Output the element nodes
    list_pos p = l->head;
    while (p != NULL) {
        print_elem_node(il, p);
        p = p->next;
    }

    // Close the subgraph
    il--;
    iprintf(il, "}\n");

    if (indent_level == 0) {
        // Put the user nodes in userspace
        iprintf(il, "subgraph cluster_userspace { label=\"User space\"\n");
        il++;
    }

    // Output the value nodes
    p = l->head;
    while (p != NULL) {
        if (p->val) {
            print_value_node(il, p->val, print_func);
        }
        p = p->next;
    }

    if (indent_level == 0) {
        // Close userspace
        il--;
        iprintf(il, "}\n");
    }

    // Output the edges from the head
    print_head_edges(il, l);

    // Output the edges from each element
    p = l->head;
    while (p != NULL) {
        print_elem_edges(il, l, p);
        p = p->next;
    }

    if (indent_level == 0) {
        // Termination of graph
        printf("}\n");
    }
}
