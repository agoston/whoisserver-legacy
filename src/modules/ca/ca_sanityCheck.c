/************************************
Copyright (c) 2001                              RIPE NCC

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
#include <stdlib.h>


int ca_sanityCheck(values_t confVars[])
/*
	- does a simple sanity check
 - Parameters
		- confVars - the array of configuration variables
 - Returns 
		- an integer: -1 or 0
 */
{
int symbol;	/* A counting variable */
int status = 0;	/* Assume that the Configuration File is complete. */
int undefVars = 0; /* Number of undefined variables. */
const char *configWarningStr = "Warning: undefined configuration variable:";
const char *configError_1Str = "Error: Incomplete configuration file.  Please check if this was intended.\n";
const char *configError_2Str = " configuration variables undefined.\n";

for(symbol = 0; symbol < CA_NUMBEROFSYMBOLS; symbol++)
	{
	if (!confVars[symbol].strPtr)
		{
		++undefVars;
		fprintf(stderr, "%s %s\n", configWarningStr, dictionary[symbol].varName);
		}
	}

if (undefVars)
	{
	status = INCOMPLETE;
	}

	fprintf(stderr, "\n%s\n", configError_1Str);
 fprintf(stderr, "%d%s\n", undefVars, configError_2Str);
 return(status);
}

int ca_conifigCheck(void)
/*
	* A wrapper for the ca_sanityCheck() function.A
 *
 * Parameters
	*	- none
 *
 * Returns
	* -nothing
 */
{
return(ca_sanityCheck(confVars));
}
