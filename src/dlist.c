#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <dlist.h>

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
 *   v1.1  2023-01-19: Added dlist_pos_are_equal and dlist_pos_is_valid functions.
 *   v1.2  2023-03-23: Renamed dlist_pos_are_equal to dlist_pos_is_equal.
 *   v1.3  2023-03-23: Renamed dlist_pos_are_equal to dlist_pos_is_equal.
 *   v2.0  2024-03-14: Added dlist_print_internal to output dot code for visualization.
 *                     Renamed free_* stuff to kill_*. Converted to 4-tabs.
 *   v2.1  2024-05-10: Updated print_internal with improved encapsulation.
 */

// ===========INTERNAL DATA TYPES============

/*
 * The list elements are implemented as one-cells with a forward link.
 * The list position is a pointer to the internal cell before the cell
 * with the value.
 */
typedef struct cell {
    struct cell *next;
    void *val;
} cell;


struct dlist {
    cell *head;
    kill_function kill_func;
};

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * dlist_empty() - Create an empty dlist.
 * @kill_func: A pointer to a function (or NULL) to be called to
 *             de-allocate memory on remove/kill.
 *
 * Returns: A pointer to the new list.
 */
dlist *dlist_empty(kill_function kill_func)
{
    // Allocate memory for the list structure.
    dlist *l = calloc(1, sizeof(*l));

    // Allocate memory for the list head.
    l->head = calloc(1, sizeof(cell));

    // No elements in list so far.
    l->head->next = NULL;

    // Store the kill function.
    l->kill_func = kill_func;

    return l;
}

/**
 * dlist_is_empty() - Check if a dlist is empty.
 * @l: List to check.
 *
 * Returns: True if the list is empty, otherwise false.
 */
bool dlist_is_empty(const dlist *l)
{
    return (l->head->next == NULL);
}

/**
 * dlist_first() - Return the first position of a dlist, i.e. the
 *                 position of the first element in the list.
 * @l: List to inspect.
 *
 * Returns: The first position in the given list.
 */
dlist_pos dlist_first(const dlist *l)
{
    return l->head;
}

/**
 * dlist_next() - Return the next position in a dlist.
 * @l: List to inspect.
 * @p: Any valid position except the last in the list.
 *
 * Returns: The position in the list after the given position.
 *          NOTE: The return value is undefined for the last position.
 */
dlist_pos dlist_next(const dlist *l, const dlist_pos p)
{
    if (dlist_is_end(l, p)) {
        // This should really throw an error.
        fprintf(stderr,"dlist_next: Warning: Trying to navigate "
                "past end of list!");
    }
    return p->next;
}

/**
 * dlist_is_end() - Check if a given position is at the end of a dlist.
 * @l: List to inspect.
 * @p: Any valid position in the list.
 *
 * Returns: True if p is at the end of the list.
 */
bool dlist_is_end(const dlist *l, const dlist_pos p)
{
    return p->next == NULL;
}

/**
 * dlist_inspect() - Return the value of the element at a given
 *                   position in a list.
 * @l: List to inspect.
 * @p: Any valid position in the list, except the last.
 *
 * Returns: Returns the value at the given position as a void pointer.
 *          NOTE: The return value is undefined for the last position.
 */
void *dlist_inspect(const dlist *l, const dlist_pos p)
{
    if (dlist_is_end(l, p)) {
        // This should really throw an error.
        fprintf(stderr,"dlist_inspect: Warning: Trying to inspect "
                "position at end of list!\n");
    }
    return p->next->val;
}

/**
 * dlist_insert() - Insert a new element with a given value into a dlist.
 * @l: List to manipulate.
 * @v: Value (pointer) to be inserted into the list.
 * @p: Position in the list before which the value should be inserted.
 *
 * Creates a new element and inserts it into the list before p.
 * Stores data in the new element.
 *
 * Returns: The position of the newly created element.
 */
dlist_pos dlist_insert(dlist *l, void *v, const dlist_pos p)
{
    // Create new element.
    dlist_pos new_pos=calloc(1, sizeof(cell));
    // Set value.
    new_pos->val=v;

    // Set links.
    new_pos->next=p->next;
    p->next=new_pos;

    return p;
}

/**
 * dlist_remove() - Remove an element from a dlist.
 * @l: List to manipulate.
 * @p: Position in the list of the element to remove.
 *
 * Removes the element at position p from the list. If a kill_func
 * was registered at list creation, calls it to deallocate the memory
 * held by the element value.
 *
 * Returns: The position after the removed element.
 */
dlist_pos dlist_remove(dlist *l, const dlist_pos p)
{
    // Cell to remove.
    dlist_pos c=p->next;

    // Link past cell to remove.
    p->next=c->next;

    // Call kill_func if registered.
    if(l->kill_func != NULL) {
        // Return any user-allocated memory for the value.
        l->kill_func(c->val);
    }
    // Free the memory allocated to the cell itself.
    free(c);
    // Return the position of the next element.
    return p;
}

/**
 * dlist_kill() - Destroy a given dlist.
 * @l: List to destroy.
 *
 * Return all dynamic memory used by the list and its elements. If a
 * kill_func was registered at list creation, also calls it for each
 * element to return any user-allocated memory occupied by the element values.
 *
 * Returns: Nothing.
 */
void dlist_kill(dlist *l)
{
    // Use public functions to traverse the list.

    // Start with the first element (will be defined even for an
    // empty list).
    dlist_pos p = dlist_first(l);

    // Remove first element until list is empty.
    while(!dlist_is_empty(l)) {
        p = dlist_remove(l, p);
    }

    // Free the head and the list itself.
    free(l->head);
    free(l);
}

/**
 * dlist_print() - Iterate over the list element and print their values.
 * @l: List to inspect.
 * @print_func: Function called for each element.
 *
 * Iterates over the list and calls print_func with the value stored
 * in each element.
 *
 * Returns: Nothing.
 */
void dlist_print(const dlist *l, inspect_callback print_func)
{
    // Start at the beginning of the list.
    dlist_pos p = dlist_first(l);

    printf("( ");
    while (!dlist_is_end(l, p)) {
        // Call print_func with the element value at the
        // current position.
        print_func(dlist_inspect(l, p));
        // Advance to next position.
        p = dlist_next(l, p);
        // Print separator unless at element.
        if (!dlist_is_end(l, p)) {
            printf(", ");
        }
    }
    printf(" )\n");
}

/**
 * dlist_pos_is_equal() - Return true if two positions in a dlist are equal.
 * @l: List to inspect.
 * @p1: First position to compare.
 * @p2: Second position to compare.
 *
 * Returns: True if p1 and p2 refer to the same position in l, otherwise False.
 *          NOTE: The result is defined only if p1 and p2 are valid positions in l.
 */
bool dlist_pos_is_equal(const dlist *l, const dlist_pos p1, const dlist_pos p2)
{
    // Since we don't need to check whether p1 or p2 are valid, we
    // only need to compare them directly.
    return p1 == p2;
}

/**
 * dlist_pos_is_valid() - Return true for a valid position in a dlist.
 * @l: List to inspect.
 * @p: Any position.
 *
 * Returns: True if p is a valid position in the list, otherwise false.
 */
bool dlist_pos_is_valid(const dlist *l, const dlist_pos p)
{
    // Iterate over all positions in l.
    dlist_pos q = dlist_first(l);
    while (!dlist_is_end(l, q)) {
        if (dlist_pos_is_equal(l, p, q)) {
            // We found the position in the list.
            return true;
        }
        // Advance to the next valid position,
        q = dlist_next(l, q);
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
 * print_head_node() - Print a node corresponding to the dlist head struct.
 * @indent_level: Indentation level.
 * @l: List to inspect.
 *
 * Returns: Nothing.
 */
static void print_head_node(int indent_level, const dlist *l)
{
    iprintf(indent_level, "m%04lx [shape=record "
            "label=\"kill\\n%04lx|<h>head\\n%04lx\" xlabel=\"%04lx\"]\n",
            PTR2ADDR(l), PTR2ADDR(l->kill_func), PTR2ADDR(l->head), PTR2ADDR(l));
}

// Print edges from the list head to the head cell.
static void print_head_edges(int indent_level, const dlist *l)
{
    print_edge(indent_level, l, l->head, "h", "head", NULL);
}

// Print a node corresponding to the cell at position p.
static void print_elem_node(int indent_level, const dlist_pos p)
{
    iprintf(indent_level, "m%04lx [shape=record "
            "label=\"<v>val\\n%04lx|<n>next\\n%04lx\" xlabel=\"%04lx\"]\n",
            PTR2ADDR(p), PTR2ADDR(p->val),  PTR2ADDR(p->next), PTR2ADDR(p));
}

// Print edges from the cell at position p to the next and previous
// cells and the value.  The value pointer is red, the others are
// black. If the list owns the memory, the edge is solid, otherwise
// dashed.
static void print_elem_edges(int indent_level, const dlist *l, const dlist_pos p)
{
    print_edge(indent_level, p, p->next, "n", "next", NULL);

    // Ignore val ptr for head and tail nodes.
    if (p == l->head) {
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
 * dlist_print_internal() - Print the lists internal structure in dot format.
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
void dlist_print_internal(const dlist *l, inspect_callback print_func, const char *desc,
                          int indent_level)
{
    static int graph_number = 0;
    graph_number++;
    int il = indent_level;

    if (indent_level == 0) {
        // If this is the outermost datatype, start a graph...
        printf("digraph DLIST_%d {\n", graph_number);

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
            iprintf(il, "cluster_dlist_%d_description [label=\"%s\"]\n", graph_number, escaped);
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
    iprintf(il, "subgraph cluster_dlist_%d { label=\"DList\"\n", graph_number);
    il++;

    // Output the head node
    print_head_node(il, l);

    // Output the element nodes
    dlist_pos p = l->head;
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
