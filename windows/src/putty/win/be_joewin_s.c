/*
 *  This file is part of Joe's Own Editor for Windows.
 *  Copyright (c) 2014 John J. Jordan.
 *
 *  Joe's Own Editor for Windows is free software: you can redistribute it 
 *  and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2 of the 
 *  License, or (at your option) any later version.
 *
 *  Joe's Own Editor for Windows is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Joe's Own Editor for Windows.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

/*
 * Linking module for JoeWin.
 *
 * What we're doing here: joe_backend HIJACKS Putty's RAW
 * backend.  Supply that as the default and we'll intercept
 * its calls to pipe data to joe.
 */

#include <stdio.h>
#include "putty.h"

extern Backend joe_backend;


const int be_default_protocol = PROT_RAW;

const char *const appname = "joewin";

Backend *backends[] = {
    &joe_backend,
    NULL
};

/*
 * Stub implementations of functions not used in non-ssh versions.
 */
void random_save_seed(void)
{
}

void random_destroy_seed(void)
{
}

void noise_ultralight(unsigned long data)
{
}
