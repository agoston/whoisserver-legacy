/***************************************
 $Revision:

 CA module: Values defined at compile time.

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

GSList *sourceList = NULL;

GSList *dbList = NULL;

GSList *nrtmList = NULL;

/*
 * The test configuration file.
 */
const char *testFile = "test.config";
const char *tempFile = "temp.config";
const char *dictFile = "dictionary.txt";
const char *confFile = "rip.config";

const char *sourcesFile = "sources.config";

values_t confVars[CA_NUMBEROFSYMBOLS];
values_t globals[CA_NUMBEROFSYMBOLS];
values_t locals[CA_NUMBEROFSYMBOLS];

ca_dbSource_t *theSrc;

const char *configWarningStr = "Warning: undefined configuration variable:";
const char *configError_1Str = "Error: Incomplete configuration file.  Was this intended ???\n";
const char *configError_2Str = " configuration variables undefined.\n";
const char *configVarChk_OK_Str = "..... mandatory variables check completed.\n";

