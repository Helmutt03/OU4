#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAXNODENAME 40
#define BUFSIZE 400

/**
 * @brief Find position of first non-whitespace character.
 *
 * @param s - string to search.
 * @return int - Returns the position of the first non-whitespace character, or -1 if not found.
 */
int first_non_white_space(const char *s)
{
    int i = 0; // Start at first char.

    // Advance as long as we're loooking at white-space until we hit EOL.
    while (s[i] && isspace(s[i])) {
        i++;
    }
    // We found a non-whitespace char.
    if (s[i]) {
        // It was a proper characted. Return its position.
        return i;
    } else {
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

bool line_is_blank(const char *s)
{
    // Line is blank if it only contained white-space chars.
    return first_non_white_space(s) < 0;
}

/**
 * @brief Determina if the string is a comment line.
 *
 * @param s - string to search.
 * @return true if the line is a comment line.
 * @return false if the line is not a comment line.
 *
 * A comment line has a hash sign '#' as the first non-whitespace char on the line.
 */
bool line_is_comment(const char *s)
{
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
int parse_map_line(const char *buf, char *n1, char *n2)
{
    // Create a format string the will do the work.
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
