
/* Declarations for getopt.
   Copyright (C) 1989-1994, 1996-1999, 2001, 2003-2007, 2009-2013 Free Software
   Foundation, Inc.
   This file is part of the GNU C Library.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
// Heavily modified by JJ Jordan.


#ifndef __GETOPT_H__
#define __GETOPT_H__


/* For communication from 'getopt' to the caller.
   When 'getopt' finds an option that takes an argument,
   the argument value is returned here.
   Also, when 'ordering' is RETURN_IN_ORDER,
   each non-option ARGV-element is returned here.  */

extern char *optarg;

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to 'getopt'.

   On entry to 'getopt', zero means this is the first call; initialize.

   When 'getopt' returns -1, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, 'optind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

extern int optind;

/* Callers store zero here to inhibit the error message 'getopt' prints
   for unrecognized options.  */

extern int opterr;

/* Set to an option character which was unrecognized.  */

extern int optopt;

/* Describe the long-named options requested by the application.
   The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
   of 'struct option' terminated by an element containing a name which is
   zero.

   The field 'has_arg' is:
   no_argument          (or 0) if the option does not take an argument,
   required_argument    (or 1) if the option requires an argument,
   optional_argument    (or 2) if the option takes an optional argument.

   If the field 'flag' is not NULL, it points to a variable that is set
   to the value given in the field 'val' when the option is found, but
   left unchanged if the option is not found.

   To have a long-named option do something other than set an 'int' to
   a compiled-in constant, such as set a value from 'optarg', set the
   option's 'flag' field to zero and its 'val' field to a nonzero
   value (the equivalent single-letter option character, if there is
   one).  For long options that have a zero 'flag' field, 'getopt'
   returns the contents of the 'val' field.  */

struct option
{
  const char *name;
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};


/* Get definitions and prototypes for functions to process the
   arguments in ARGV (ARGC of them, minus the program name) for
   options given in OPTS.

   Return the option character from OPTS just read.  Return -1 when
   there are no more options.  For unrecognized options, or options
   missing arguments, 'optopt' is set to the option letter, and '?' is
   returned.

   The OPTS string is a list of characters which are recognized option
   letters, optionally followed by colons, specifying that that letter
   takes an argument, to be placed in 'optarg'.

   If a letter in OPTS is followed by two colons, its argument is
   optional.  This behavior is specific to the GNU 'getopt'.

   The argument '--' causes premature termination of argument
   scanning, explicitly telling 'getopt' that there are no more
   options.

   If OPTS begins with '-', then non-option arguments are treated as
   arguments to the option '\1'.  This behavior is specific to the GNU
   'getopt'.  If OPTS begins with '+', or POSIXLY_CORRECT is set in
   the environment, then do not permute arguments.  */

#define __getopt_argv_const /*empty*/

extern int getopt (int ___argc, char *const *___argv, const char *__shortopts);

extern int getopt_long (int ___argc, char *__getopt_argv_const *___argv,
                        const char *__shortopts,
                        const struct option *__longopts, int *__longind);

extern int getopt_long_only (int ___argc, char *__getopt_argv_const *___argv,
                             const char *__shortopts,
                             const struct option *__longopts, int *__longind);

#endif
