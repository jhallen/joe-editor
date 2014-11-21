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
