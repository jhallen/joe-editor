/*
 *	Directory and path functions
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#ifndef _JOE_PATH_H
#define _JOE_PATH_H 1

unsigned char *joesep PARAMS((unsigned char *path));

/* char *namprt(char *path);
 * Return name part of a path.  There is no name if the last character
 * in the path is '/'.
 *
 * The name part of "/hello/there" is "there"
 * The name part of "/hello/" is ""
 * The name part if "/" is ""
 */
unsigned char *namprt PARAMS((unsigned char *path));
unsigned char *namepart PARAMS((unsigned char *tmp, size_t tmpsiz, unsigned char *path));

/* char *dirprt(char *path);
 * Return directory and drive part of a path.  I.E., everything to the
 * left of the name part.
 *
 * The directory part of "/hello/there" is "/hello/"
 * The directory part of "/hello/" is "/hello/"
 * The directory part of "/" is "/"
 */
unsigned char *dirprt PARAMS((unsigned char *path));

/* char *begprt(char *path);
 * Return the beginning part of a path.
 *
 * The beginning part of "/hello/there" is "/hello/"
 * The beginning part of "/hello/" is "/"
 * The beginning part of "/" is "/"
 */
unsigned char *begprt PARAMS((unsigned char *path));

/* char *endprt(char *path);
 * Return the ending part of a path.
 *
 * The ending part of "/hello/there" is "there"
 * The ending part of "/hello/" is "hello/"
 * The ending part of "/" is ""
 */
unsigned char *endprt PARAMS((unsigned char *path));

/* int mkpath(char *path);
 * Make sure path exists.  If it doesn't, try to create it
 *
 * Returns 1 for error or 0 for success.  The current directory
 * and drive will be at the given path if successful, otherwise
 * the drive and path will be elsewhere (not necessarily where they
 * were before mkpath was called).
 */
int mkpath PARAMS((unsigned char *path));

/* char *mktmp(char *);
 * Create an empty temporary file.  The file name created is the string passed
 * to this function postfixed with /joe.tmp.XXXXXX, where XXXXXX is some
 * string six chars long which makes this file unique.
*/
unsigned char *mktmp PARAMS((unsigned char *where));

/* Change drive and directory */
#define chddir chdir

/* int rmatch(char *pattern,char *string);
 * Return true if string matches pattern
 *
 * Pattern is:
 *     *                 matches 0 or more charcters
 *     ?                 matches any 1 character
 *     [...]             matches 1 character in set
 *     [^...]            matches 1 character not in set
 *     [[]               matches [
 *     [*]               matches *
 *     [?]               matches ?
 *     any other         matches self
 *
 *  Ranges of characters may be specified in sets with 'A-B'
 *  '-' may be specified in sets by placing it at the ends
 *  '[' may be specified in sets by placing it first
 */
int rmatch PARAMS((unsigned char *a, unsigned char *b));
int isreg PARAMS((unsigned char *s));

/* char **rexpnd(char *path,char *pattern);
 * Generate array (see va.h) of file names from directory in 'path'
 * which match the pattern 'pattern'
 */
unsigned char **rexpnd PARAMS((unsigned char *word));
unsigned char **rexpnd_users PARAMS((unsigned char *word));

int chpwd PARAMS((unsigned char *path));
unsigned char *pwd PARAMS((void));
unsigned char *simplify_prefix PARAMS((unsigned char *path));

#endif
