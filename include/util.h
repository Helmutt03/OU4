#ifndef __UTIL_H
#define __UTIL_H

/*
 * Utility function types for deallocating, printing and comparing
 * values stored by various data types.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *          Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *
 * Based on earlier code by: Johan Eliasson (johane@cs.umu.se).
 *
 * Version information:
 *   v1.0 2018-01-28: First public version.
 *   v1.1 2018-02-06: Updated explanation for the compare_function.
 *   v1.2 2023-01-14: Added version define constants and strings.
 *   v1.3 2024-03-13: Added PTR2ADDR macro.
 */

// Macros to create a version string out of version constants
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Version constants
#define CODE_BASE_MAJOR_VERSION 2
#define CODE_BASE_MINOR_VERSION 2
#define CODE_BASE_REVISION 2
#define CODE_BASE_PATCH 3

#define CODE_BASE_RELEASE_DATE "2026-01-20"

// Create a short version string
#define CODE_BASE_VERSION "v"                   \
    STR(CODE_BASE_MAJOR_VERSION)                \
    "."                                         \
    STR(CODE_BASE_MINOR_VERSION)                \
    "."                                         \
    STR(CODE_BASE_REVISION)                     \
    "."                                         \
    STR(CODE_BASE_PATCH)

// Create a version string
#define CODE_BASE_LONG_VERSION "Version: "      \
    STR(CODE_BASE_MAJOR_VERSION)                \
    "."                                         \
    STR(CODE_BASE_MINOR_VERSION)                \
    "."                                         \
    STR(CODE_BASE_REVISION)                     \
    "."                                         \
    STR(CODE_BASE_PATCH)

// Type definition for de-allocator function, e.g. free().
typedef void (*kill_function)(void *);

// For backwards compatibility with pre-v2.0 code.
typedef kill_function free_function;

// Type definition for read-only callback for single-value containers,
// used by e.g. print functions.
typedef void (*inspect_callback)(const void *);

// Ditto for dual-value containers.
typedef void (*inspect_callback_pair)(const void *, const void *);

// Type definition for comparison function, used by e.g. table.
//
// Comparison functions should return values that indicate the order
// of the arguments. If the first argument is considered less/lower
// than the second, a negative value should be returned. If the first
// argument is considered more/higher than the second, a positive value
// should be returned. If the arguments are considered equal, a zero
// value should be returned.
typedef int compare_function(const void *,const void *);

// Constant used by ptr2addr, used by various print_internal functions.
#define PTR2ADDR_MASK 0xffff

// Macro that acts as a function to convert the address in the pointer
// p into an unsigned long value, keeping only the part indicated by
// the mask.
#define PTR2ADDR(p) (((unsigned long)p) & PTR2ADDR_MASK)

#endif
