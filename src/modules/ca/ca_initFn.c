/***************************************
 $Revision:

 CA module: a wrapper for the functions which parse the
 dictionary, configuration and source files.

 Status: NOT REVIEWED, NOT TESTED

 Author(s):       Ambrose Magee

 ******************//******************
 Modification History:

 ******************/

/************************************
 Copyright (c) 2000                              RIPE NCC

 All Rights Reserved

 Permission to use, copy, modify, and distribute this software and its
 documentation for any purpose and without fee is hereby granted,
 provided that the above copyright notice appear in all copies and that
 both that copyright notice and this permission notice appear in
 supporting documentation, and that the name of the author not be
 used in advertising or publicity pertaining to distribution of the
 software without specific, written prior permission.

 THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
 AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ***************************************/

#include "rip.h"

#include <stdio.h>
#include <string.h>

/****************************************************************
 * ca_init() -- Initialisation function;                *
 *          - a wrapper for the ca_populateDictionary,`    *
 *            the ca_readConfig and the ca_readSources    *
 *            functions.                        *
 *                                          *
 * Parameters                                  *
 *      configFile -- the name of the configuration file    *
 *                                          *
 * Returns                                    *
 *    An integer; the return value of ca_mandVarChk().
 *                                          *
 ****************************************************************/
int ca_init(const char *configFile)
{

    /*
     * We do not need this function now; the dictionary is hardcoded.
     *
     * ca_populateDictionary(dictionary, varNo);
     */

#ifdef DEBUG
    ca_getDictionary(dictionary, CA_NUMBEROFSYMBOLS);
#endif /* DEBUG */

    /* initialize confvars array */
    memset(confVars, 0, sizeof(values_t) * CA_NUMBEROFSYMBOLS);

    ca_readConfig(configFile, confVars, CA_NUMBEROFSYMBOLS);

    if (ca_mandVarChk()) {
        fprintf(stderr, "Mandatory variable(s) undefined\n\n");
        fprintf(stderr, "<<<<<<<<<<<<<<<     >>>>>>>>>>>\n\n");
        fprintf(stderr, "Continuing ..... \n\n");

    }

    sourcesFile = ca_get_dirlist(CA_SOURCEFILE);
    ca_readSources(sourcesFile, confVars);
    return (ca_mandVarChk());
}
