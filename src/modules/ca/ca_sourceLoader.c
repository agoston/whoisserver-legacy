/***************************************
 $Revision:

 CA module: definitions of functions that read a file of databases and
 sources.

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
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <unistd.h>

/* #define DEBUG *//* Swich OFF Debugging. */

/*******************************************************
 * This file contains the definitions of functions     *
 * that read a file of databases and sources.        *
 *******************************************************/

void ca_readSources(const char *sourcesDefFile, values_t confVars[])
/*******************************************************************
 *                                             *
 * ca_readSources -- parses the Sources file and writes the values  *
 *              into memory.                        *
 *                                            *
 * Parameters                                    *
 *    sourcesFile  -- the file of databases, mirrors, sources    *
 *    confVars[]  --  the array of values structures          *
 *                                            *
 * Returns                                      *
 *    Nothing -- perhaps make this return 0 on successful exit ?  *
 *                                            *
 *******************************************************************/
{
    FILE *sourcesFilePtr; /* Pointer to Source file. */
    char line[STR_M]; /* The current line of input. */
    const char *comment = "#"; /* Declared as a string. */
#ifdef DEBUG
    char name[STRLENGTH_M]; /* The name of the config variable */
    char value[STRLENGTH_XXL]; /* The value of the variable */
    /* 640 characters */

    int location; /* Storage Location of the variable's value. */
    int type; /* Data type of the variable, represented by an
     * integer. */

    const char *blankLine = "\n"; /* Declared as a string, not a
     * character. */

    char dbcomp[16]; /* Component of a databse. */
    char nrtmcomp[16]; /* Component of an nrtm. */
    int mode; /* The mode of operation of the src */
    char srcOptions[16]; /* The options of a source. */
    char nrtMirror[STRLENGTH_M]; /* The elements of a NRTM */
    int updatePort; /* The update port of the source */
    /* N. B.  This is not the same as the */
    /* updPort in the UPDSOURCE variables. */
#endif	/* DEBUG */
    char source[STR_M]; /* The name of a source. */
    char database[STR_M]; /* The name of a database. */
    char mirror[STR_M]; /* The name of a mirror. */
    char varName[STR_M]; /* Temporary variable */

    ca_database_t *newDbPtr= NULL; /* A pointer to a new instance of */
    /* ca_database_t.                 */

    ca_mirror_t *newMirrPtr= NULL; /* A pointer to a new instance of */
    /* ca_mirror_t.                   */

    ca_dbSource_t *newSrcPtr= NULL; /* A pointer to a new instance of */
    /* ca_database_list_t.            */

    int in_DATABASE_def, in_NRTM_def, in_SOURCE_def;
    /* When we are reading the definition */
    /* of a database, nrtm, source, etc.  */
    /* this is set to 1.  Otherwise, it */
    /* is 0. */

    int no_source_defd;
    /* At least one source _must_ be defined.		*/
    /* If one or more sources are defined, then	*/
    /* this variable is set to 0.  Otherwise, 		*/
    /* this is 1.											*/

    /*
     * Function prototypes of ca_parseDbLine(), ca_parseNrtmLine() and
     * ca_parseSrcLine().  We put them here so that it can only be called
     * from within the ca_readSources() function.
     */
    void ca_parseDbLine(char *, ca_database_t *);
    void ca_parseNrtmLine(char *, ca_mirror_t *);
    void ca_parseSrcLine(char *, ca_dbSource_t *);

#ifdef DEBUG
    printf("\nInside ca_readSources() function.\n");
    printf("Sources file is: %s\n", sourcesDefFile);
#endif	/* DEBUG */

    /*
     * Open the sources file for reading .....
     */
    if ((sourcesFilePtr = fopen(sourcesDefFile, "r")) == NULL) {
        fprintf(stderr, "Error: the file %s could not be opened.\n", sourcesDefFile);
        die;
    }

    /* Before reading the file, we initialise all flags to 0. */
    in_DATABASE_def = 0;
    in_NRTM_def = 0;
    in_SOURCE_def = 0;
    no_source_defd = 1;

    /*
     * Read the first line of the file. Remove leading and trailing
     * blank-space characters. if the first character of the line is a
     * comment or if it is a blank-line, continue.
     * 
     */

    fgets(line, sizeof(line), sourcesFilePtr);
    g_strstrip(line);

    /*
     * While there are lines to be read in the sources file, process the
     * current line and read the next line.
     */

    while (!feof(sourcesFilePtr)) {
#ifdef DEBUG
        printf("line:%s:End of line\n", line);
        printf("Length of line: %d\n", strlen(line));
#endif	/* DEBUG */

        /*
         * Ignore comments and empty lines.
         */
        if ((strncmp(line, comment, 1) == 0) || (strlen(line) == 0)) {
#ifdef DEBUG
            printf("We are reading a comment or an empty line ..... \n");
#endif	/* DEBUG */
            fgets(line, sizeof(line), sourcesFilePtr);
            g_strstrip(line);
            continue;
        }

        /* Testing */
#ifdef DEBUG
        printf("LINE >>>%sEND_OF_LINE\n", line);
#endif	/* DEBUG */

        /*
         * if we are in a DATABASE definition then have we read all
         * of the definition ? if yes, then commit the definition,
         * reset the 'Database' flag and read the next line.
         * Otherwise parse the line and store the details in
         * temporary variables.
         */

        /* If we are in a DATABASE definition */
        if (in_DATABASE_def) {
            /*
             * If we have reached the end of a DATABASE
             * definition, commit the definition.
             */

            if (strcmp(line, "/DATABASE") == 0) {
                /* Commit the definition */
                /* Some code. */
#ifdef DEBUG
                puts("We have reached the end of a DATABASE definition");
                puts("Testing the population of the db structure:");
                printf("\n%s::%d::%s::%s::%s::%s\n", newDbPtr->host, newDbPtr->port, newDbPtr->trx_support, newDbPtr->user, newDbPtr->password, newDbPtr->dbName);
#endif	/* DEBUG */

                /*
                 * Check that the definition of the DATABASE is complete.
                 */
                if ( !(newDbPtr->host && newDbPtr->port && newDbPtr->trx_support && newDbPtr->user
                        && newDbPtr->password && newDbPtr->dbName)) {
                    fprintf(stderr, "Error: definition of database is incomplete.\n");
                    die;
                }

                /*
                 * Commit the definition to the linked list
                 * of Databases.
                 */

                dbList = g_slist_append(dbList, newDbPtr);

                /*
                 * We have reached the end of the DATABASE
                 * definition
                 */
                /* Thus, reset the flag and free some memory. */
                in_DATABASE_def = 0;

                /*
                 * Read the next line and do the conditional
                 * test.
                 */
                fgets(line, sizeof(line), sourcesFilePtr);
                g_strstrip(line);
                continue;
            }

            /*
             * Otherwise, parse the line and fill in the
             * structure of the Database.
             */
            ca_parseDbLine(line, newDbPtr);

        }

        /*
         * If we have found the _beginning_ of a Database definition,
         * then set the in_DATABASE_def flag and allocate space, etc.
         * for the database.
         */

        if ((!in_DATABASE_def) && (strncmp(line, DATABASE_KEY, strlen(DATABASE_KEY)) == 0)) {
            in_DATABASE_def = 1;

            /* Allocate space for the database */
            /* Current_db = fscanf etc.) */
            /* Fill in the defaults. */
#ifdef DEBUG
            puts("Beginning of a database defintion ..... ");
#endif	/* DEBUG */

            sscanf(line, "%s %s", varName, database);
            g_strstrip(database);

#ifdef DEBUG
            printf("Database name is: %s\n", database);
#endif	/* DEBUG */

            /*
             * Create a structure for the database.
             */
            newDbPtr = UT_calloc(1, sizeof(ca_database_t));
            /*
             *	if (newDbPtr == NULL) {
             *		fprintf(stderr, "Cannot allocate memory to new db structure\n");
             *		die;
             }
             */

            /* Assign the name of the database */
            strcpy(newDbPtr->dbName, database);

        }

        /********************
         * NRTM definition  *
         ********************/

        /*
         * if we are in a NRTM definition then have we read all of
         * the definition ? if yes, then commit the definition and
         * read the next line. otherwise parse the line and store the
         * details in temporary variables.
         */

        /* If we are in a NRTM definition */
        if (in_NRTM_def) {
            /*
             * If we have reached the end of a NRTM definition,
             * commit the definition.
             */
            if (strcmp(line, "/NRTM") == 0) {
                /* Commit the definition */
                /* Some code. */
#ifdef DEBUG
                puts("We have reached the end of a NRTM definition");
                puts("Testing the population of the mirror structure:");
                printf("\n%s::%d::%d::%d\n", newMirrPtr->host, newMirrPtr->port, newMirrPtr->delay, newMirrPtr->protocolVer);
#endif	/* DEBUG */

                /*
                 * Check that the definition of the NRTM is complete.
                 */
                if ( !(newMirrPtr->host && newMirrPtr->port && newMirrPtr->protocolVer) && (newMirrPtr->delay < 0)) {
                    fprintf(stderr, "Error: definition of near-real-time-mirror is incomplete.\n");
                    die;
                }
                /*
                 * Commit the definition to the linked list
                 * of nrt-mirrors.
                 */

                nrtmList = g_slist_append(nrtmList, newMirrPtr);

                /*
                 * We have reached the end of the NRTM
                 * definition
                 */
                /* Thus, reset the NRTM flag. */
                in_NRTM_def = 0;

                /*
                 * Read the next line and do the conditional
                 * test.
                 */
                fgets(line, sizeof(line), sourcesFilePtr);
                g_strstrip(line);
                continue;
            }

            /*
             * Otherwise, parse the line and fill in the
             * structure of the NRMT.
             */

            ca_parseNrtmLine(line, newMirrPtr);
        }

        /*
         * If we have found the beginning of a Near-Real-Time-Mirror
         * definition, then set the in_NRTM_def flag and allocate
         * space, etc. for the Near-Real-Time-Mirror.
         */

        if ((!in_NRTM_def) && (strncmp(line, NRTM_KEY, strlen(NRTM_KEY)) == 0)) {
            in_NRTM_def = 1;
            /* Allocate space for the Near-Real-Time-Mirror. */
            /* Current_db = fscanf etc.) */
            /* Fill in the defaults. */
#ifdef DEBUG
            puts("Beginning of a Near-Real-Time-Mirror defintion ..... ");
#endif	/* DEBUG */

            sscanf(line, "%s %s", varName, mirror);

            /*
             * Create a structure for the mirror.
             */
            newMirrPtr = UT_calloc(1, sizeof(ca_mirror_t));
            /*
             * No need to verify that NULL has not been returned.
             *
             *		if (newMirrPtr == NULL) {
             *				fprintf(stderr, "Cannot allocate memory to new nrtm structure\n");
             *			die;
             *			}
             */

            /* Assign the name of the mirror ? */
            strcpy(newMirrPtr->mrName, mirror);
        }

        /*********************
         * SOURCE Definition *
         *********************/

        /*
         * if we are in a SOURCE definition then have we read all of
         * the definition ? if yes, then commit the definition, reset
         * the 'Database' flag and read the next line. Otherwise
         * parse the line and store the details in temporary
         * variables.
         */

        /* If we are in a SOURCE definition */
        if (in_SOURCE_def) {
            /*
             * If we have reached the end of a SOURCE definition,
             * commit the definition.
             */

            if (strcmp(line, "/SOURCE") == 0) {
                /* Commit the definition */
                /* Some code. */
#ifdef DEBUG
                puts("We have reached the end of a SOURCE definition");
                puts("Testing the population of the new Source structure:");
                printf("Source name: %s\n", newSrcPtr->name);
                printf("\nDB == %s::%d::%s::%s::%s::%s\n", (newSrcPtr->db).host, (newSrcPtr->db).port, (newSrcPtr->db).trx_support, (newSrcPtr->db).user, (newSrcPtr->db).password, (newSrcPtr->db).dbName);
                printf("Mode: %d\n", newSrcPtr->opMode);
                printf("NRTM == %s::%d::%d:%d\n", (newSrcPtr->nrtm).host, (newSrcPtr->nrtm).port, (newSrcPtr->nrtm).delay, (newSrcPtr->nrtm).protocolVer);
                printf("UpdPort: %d\n", newSrcPtr->updPort);
                printf("New Source Options == %s::%s\n", newSrcPtr->canupd, newSrcPtr->deflook);
#endif	/* DEBUG */

                /*
                 * Check that the definition of the SOURCE is complete.
                 */
                if ( !(newSrcPtr->name && newSrcPtr->updPort && newSrcPtr->canupd && newSrcPtr->deflook)
                        && (newSrcPtr->opMode < 0)) {
                    fprintf(stderr, "Error: definition of source is incomplete.\n");
                    die;
                }
                /*
                 * Commit the definition to the linked list
                 * of Sources.
                 */

                sourceList = g_slist_append(sourceList, newSrcPtr);

                /*
                 * We have reached the end of the SOURCE
                 * definition
                 */
                /* Thus, reset the flag and free some memory. */
                in_SOURCE_def = 0;

                /*
                 * Read the next line and do the conditional
                 * test.
                 */
                fgets(line, sizeof(line), sourcesFilePtr);
                g_strstrip(line);
                continue;
            }

            /*
             * Otherwise, parse the line and fill in the
             * structure of the Database.
             */
            ca_parseSrcLine(line, newSrcPtr);

        }

        /*
         * If we have found the _beginning_ of a SOURCE definition,
         * then set the in_SOURCE_def flag and allocate space, etc.
         * for the database.
         * We also set the no_source_defd flag.
         */

        if ((!in_SOURCE_def) && (strncmp(line, SOURCE_KEY, strlen(SOURCE_KEY)) == 0)) {
            in_SOURCE_def = 1;
            no_source_defd = 0;

            /* Allocate space for the Source */
            /* Current_source = fscanf etc.) */
            /* Fill in the defaults. */
#ifdef DEBUG
            puts("Beginning of a Source defintion ..... ");
#endif	/* DEBUG */

            sscanf(line, "%s %s", varName, source);
            g_strstrip(source);

#ifdef DEBUG
            printf("Source name is: %s\n", source);
#endif	/* DEBUG */

            /*
             * Create a structure for the source.
             * 
             */
            newSrcPtr = UT_calloc(1, sizeof(ca_dbSource_t));

            /*
             * No need to check that NULL has not been returned.
             *
             * if (newSrcPtr == NULL) {
             *		fprintf(stderr, "Cannot allocate memory to new Source structure\n");
             *		die;
             * }
             */

            /* Assign the name of the Source */
            strcpy(newSrcPtr->name, source);

            /* FIXME: this could be set in the config as well, but for now, the '*-GRS' check is perfect - agoston, 2011-03-03 */
            g_strup(source);
            if (g_str_has_suffix(source, "-GRS")) {
                newSrcPtr->isGRS = 1;
            }
        }

        /* Read the next line. */
        fgets(line, sizeof(line), sourcesFilePtr);
        g_strstrip(line);

        /* End of while loop; i.e. end of processing a line. */
    }

    /* Close the sources definition file. */
    fclose(sourcesFilePtr);

    /* End of ca_readSources() function */

    if (no_source_defd != 0) {
        fprintf(stderr, "Error: No source defined !!!\n");
        die;
    }
}

void ca_getAllDatabases(GSList * databases) {
    GSList *currentPtr; /* Pointer to the structure at which we look. */

    /*
     * Look at the first member of the linked-list of sources.
     */
    currentPtr = databases;

    /*
     * Look at each data component of the source list, untill we reach
     * the end of the list.
     */
    while (currentPtr != NULL) {
        ca_database_t *dbPtr = currentPtr->data;
        printf("\n%s,%d,%s,%s,%s,%s\n", dbPtr->host, dbPtr->port, dbPtr->trx_support, dbPtr->user, dbPtr->password,
                dbPtr->dbName);
        currentPtr = currentPtr->next;
    }
}

void ca_getAllMirrors(GSList * mirrors) {
    GSList *currentPtr; /* Pointer to the structure at which we look. */

    /*
     * Look at the first member of the linked-list of sources.
     */
    currentPtr = mirrors;

    /*
     * Look at each data component of the source list, untill we reach
     * the end of the list.
     */
    while (currentPtr != NULL) {
        ca_mirror_t *nrtmPtr = currentPtr->data;
        printf("\n%s,%d,%d,%d, %s\n", nrtmPtr->host, nrtmPtr->port, nrtmPtr->delay, nrtmPtr->protocolVer,
                nrtmPtr->mrName);
        currentPtr = currentPtr->next;
    }
}

void ca_parseDbLine(char *lineStr, ca_database_t * dbStructPtr)
/*******************************************************************
 *                                             *
 * ca_parseLine  -- parses the a line in the Sources file and     *
 *             writes the values into temporary variables.    *
 *                                            *
 * Parameters                                    *
 *    lineStr     -- the current line of the Sources file       *
 *               -- a NULL terminated string            *
 *    dbStructPtr  -- the db we are filling                 *
 *              -- a pointer to a ca_database_t structure.  *
 *                                            *
 * Returns                                      *
 *    Nothing -- perhaps make this return 0 on successful exit ?  *
 *                                            *
 *******************************************************************/
{
    char dbComp[STR_M]; /* Component of a database. */
    char varName[STR_M]; /* The name of the variable. */

    gchar **tokens; /* Pointer to an array of strings. */

#ifdef DEBUG
    int i; /* A counting variable. */
#endif	/* DEBUG */

    /*
     * Split the line on the ':' character. Then, for both the name of
     * the variable and its value, remove leading and trailing
     * blank-space characters.
     * We set max_tokens to be 2 to allow ':' in the password.
     */
    tokens = g_strsplit(lineStr, ":", 2);

#ifdef DEBUG
    for (i = 0; tokens[i] != NULL; i++)
    printf("tokens[%d] = %s\n", i, tokens[i]);
#endif	/* DEBUG */

    strcpy(varName, tokens[0]);
    strcpy(dbComp, tokens[1]);

    /* Free the memory used by the tokens array. */
    g_strfreev(tokens);

    /* Remove leading and trailing blank-space characters. */
    g_strstrip(varName);
    g_strstrip(dbComp);

#ifdef DEBUG
    printf("VarName: %s; dbComp: %s\n", varName, dbComp);
#endif	/* DEBUG */

    if (strcmp(varName, "host") == 0) {
        strcpy(dbStructPtr->host, dbComp);
    } else {
        if (strcmp(varName, "port") == 0) {
            dbStructPtr->port = atoi(dbComp);
        } else {
            if (strcmp(varName, "user") == 0) {
                strcpy(dbStructPtr->user, dbComp);
            } else {
                if (strcmp(varName, "trx_support") == 0) { /* added 24.10.2002 EG */
                    strcpy(dbStructPtr->trx_support, dbComp);
                } else {
                    if (strcmp(varName, "password") == 0) {
                        strcpy(dbStructPtr->password, dbComp);
                    } else {
                        fprintf(stderr, "Unknown database component \"%s\".\n", dbComp);
                        die;
                    }
                }
            }
        }
    }
}

void ca_parseNrtmLine(char *lineStr, ca_mirror_t * mrStructPtr)
/*
 * */
{
    char nrtmComp[STR_M]; /* Component of a NRTM. */
    char varName[STR_M]; /* The name of the variable. */

    gchar **tokens; /* Pointer to an array of strings. */

#ifdef DEBUG
    int i; /* A counting variable. */
#endif	/* DEBUG */

    /*
     * Split the line on the ':' character. Then, for both the name of
     * the variable and its value, remove leading and trailing
     * blank-space characters.
     * We set max_tokens to be 2; thus the line is split after the first
     * ':'.
     */
    tokens = g_strsplit(lineStr, ":", 2);

#ifdef DEBUG
    for (i = 0; tokens[i] != NULL; i++)
    printf("tokens[%d] = %s\n", i, tokens[i]);
#endif	/* DEBUG */

    strcpy(varName, tokens[0]);
    strcpy(nrtmComp, tokens[1]);

    /* Free the memory used by the tokens array. */
    g_strfreev(tokens);

    /* Remove leading and trailing blank-space characters. */
    g_strstrip(varName);
    g_strstrip(nrtmComp);

#ifdef DEBUG
    printf("VarName: %s; nrtmComp: %s\n", varName, nrtmComp);
#endif	/* DEBUG */

    if (strcmp(varName, "host") == 0) {
        strcpy(mrStructPtr->host, nrtmComp);
    } else {
        if (strcmp(varName, "port") == 0) {
            mrStructPtr->port = atoi(nrtmComp);
        } else {
            if (strcmp(varName, "delay") == 0) {
                mrStructPtr->delay = atoi(nrtmComp);
            } else {
                if (strcmp(varName, "protocolVersion") == 0) {
                    mrStructPtr->protocolVer = atoi(nrtmComp);
                } else {
                    fprintf(stderr, "Unknown mirror component \"%s\".\n", nrtmComp);
                    die;
                }
            }
        }
    }
}

void ca_parseSrcLine(char *lineStr, ca_dbSource_t * srcStructPtr)
/*
 * ca_parseSrcLine() function.
 */
{
    char srcComp[STR_M]; /* Component of a database. */
    char varName[STR_M]; /* The name of the variable. */

    gchar **tokens; /* Pointer to an array of strings. */

#ifdef DEBUG
    int i; /* A counting variable. */
#endif	/* DEBUG */

    /*
     * Split the line on the ':' character. Then, for both the name of
     * the variable and its value, remove leading and trailing
     * blank-space characters.
     * We set the value of max_tokens to be 2; thus, the line is split at
     * the first ':' character.
     */
    tokens = g_strsplit(lineStr, ":", 2);

#ifdef DEBUG
    for (i = 0; tokens[i] != NULL; i++)
    printf("tokens[%d] = %s\n", i, tokens[i]);
#endif	/* DEBUG */

    strcpy(varName, tokens[0]);
    strcpy(srcComp, tokens[1]);

    /* Free the memory used by the tokens array. */
    g_strfreev(tokens);

    /* Remove leading and trailing blank-space characters. */
    g_strstrip(varName);
    g_strstrip(srcComp);

#ifdef DEBUG
    printf("VarName: %s; srcComp: %s\n", varName, srcComp);
#endif	/* DEBUG */

    /*
     * Parse each line of the SOURCE definition. If we find a database or
     * a mirror, search for it in the appropriate linked list and make
     * this source point to it.
     */

    if (strcmp(varName, "database") == 0) {
        /* Search for the appropriate database. */
        /* Make this source point to it. */
        /* Use ca_getDbHandleByName(). */

        /* Check that we did not get a NULL pointer.		*/
        /* If we did, then print an error message and	*/
        /* die.														*/

        if (ca_getDbHandleByName(srcComp) == NULL) {
            fprintf(stderr, "Error: Non-existent database \"%s\" referenced !!!\n", srcComp);
            die;
        } else {
            srcStructPtr->db = *ca_getDbHandleByName(srcComp);
        }

    } else {
        if (strcmp(varName, "opMode") == 0) {
            srcStructPtr->opMode = atoi(srcComp);
        } else {
            if (strcmp(varName, "updPort") == 0) {
                srcStructPtr->updPort = atoi(srcComp);
            } else {
                if (strcmp(varName, "canupd") == 0) {
                    strcpy(srcStructPtr->canupd, srcComp);
                } else {
                    if (strcmp(varName, "deflook") == 0) {
                        strcpy(srcStructPtr->deflook, srcComp);
                    } else {
                        if (strcmp(varName, "nrtm") == 0) {
                            /*
                             * Get Mirror Handle
                             * by Name
                             */
                            /*
                             * Assign this mirror
                             * to
                             */
                            /*
                             * 
                             * srcStructPtr->nrtm.
                             * 
                             */

                            /* We check that the mirror is defined.		*/

                            if (ca_getNrtmHandleByName(srcComp) == NULL) {
                                fprintf(stderr, "Error: non-existent mirror \"%s\" referenced !!!\n", srcComp);
                                die;
                            } else {
                                srcStructPtr->nrtm = *ca_getNrtmHandleByName(srcComp);
                            }
                        } else {
                            fprintf(stderr, "Unknown SOURCE component \"%s\".\n", srcComp);
                            die;
                        }
                    }
                }
            }
        }
    }
}

ca_database_t * ca_getDbHandleByName(char *databaseNameStr)
/*******************************************************************
 * ca_getDbHandleByName                              *
 *   -- A function that compares each 'name' component of every     *
 *    element in the linked-list of databases with the name of   *
 *    the database to be found.  If the required database is     *
 *    found, a pointer to the structure representing this       *
 *     database is  returned.                          *
 *                                            *
 *   Parameters                                    *
 *  -- databaseNameStr - the name of the required database        *
 *                                            *
 *  Returns                                      *
 *  -- dbasePtr  - a pointer to the structure representing the     *
 *            database or a pointer to NULL, if we cannot     *
 *            find the database.                      *
 *                                            *
 *******************************************************************/
{
    /*
     * Define a pointer to the current element in the linked list.
     * Later, initialise it to the start of the list.
     */
    GSList *currentPtr;

    /*
     * Define and later initialise a pointer that points to the 'data'
     * component of the GSList struct; i.e. a pointer to a variable of
     * type ca_database_t.
     */
    ca_database_t *dbasePtr;

    /*
     * Before we search the linked-list, we must first check that it is
     * not empty.  If it is, then print a error message and exit.
     */

    if (dbList == NULL) {
        fprintf(stderr, "Error: no database is defined in SOURCEFILE.\n");
        die;
    }

    currentPtr = dbList;
    dbasePtr = currentPtr->data;

    /*
     * Look at each data component of the list of databases; (each data
     * component is a structure of type ca_database_t). Compare the
     * 'name' component of each ca_database_t structure with the value of
     * databaseName untill we get a match or we reach the end of the
     * list.
     */

    /*
     * We first check if currentPtr is pointing to NULL; if yes, we exit
     * the while loop; if no, we make dbasePtr point to the data
     * component of the current ca_database_t structure; then, we check
     * if this is the database name that we want; if yes, we _break_ from
     * the while loop.
     */
    while (currentPtr != NULL) {
        dbasePtr = currentPtr->data;
        if (strcmp(dbasePtr->dbName, databaseNameStr) == 0)
            break;
        currentPtr = currentPtr->next;
    }

    /*
     * We return a pointer.  If we found the database, this pointer
     * points to the ca_database_t structure which represents the
     * database. If we did not find the database, we return a pointer to
     * NULL.
     */
    if (currentPtr == NULL) {
        dbasePtr = NULL;
        return (dbasePtr);
    } else {
        return (dbasePtr);
    }

}

ca_mirror_t * ca_getNrtmHandleByName(char *nrtmNameStr)
/*******************************************************************
 * ca_NrtmHandleByName                              *
 *   -- A function that compares each 'name' component of every     *
 *    element in the linked-list of databases with the name of   *
 *    the database to be found.  If the required database is     *
 *    found, a pointer to the structure representing this       *
 *     database is  returned.                          *
 *                                            *
 *   Parameters                                    *
 *  -- nrtmNameStr - the name of the required database        *
 *                                            *
 *  Returns                                      *
 *  -- nrtmPtr  - a pointer to the structure representing the     *
 *            database or a pointer to NULL, if we cannot     *
 *            find the database.                      *
 *                                            *
 *******************************************************************/
{
    /*
     * Define a pointer to the current element in the linked list.
     * Later, initialise it to the start of the list.
     */
    GSList *currentPtr;

    /*
     * Define and initialise a pointer that points to the 'data'
     * component of the GSList struct; i.e. a pointer to a variable of
     * type ca_database_t.
     */
    ca_mirror_t *nrtmPtr;

    /*
     * First check that the linked list of mirrors is not empty.  If it
     * is, print an error and exit.
     */
    if (nrtmList == NULL) {
        fprintf(stderr, "Error: no near-real-time mirror defined in SOURCEFILE.\n");
        die;
    }

    currentPtr = nrtmList;
    nrtmPtr = currentPtr->data;

    /*
     * Look at each data component of the list of databases; (each data
     * component is a structure of type ca_database_t). Compare the
     * 'name' component of each ca_database_t structure with the value of
     * databaseName untill we get a match or we reach the end of the
     * list.
     */

    /*
     * We first check if currentPtr is pointing to NULL; if yes, we exit
     * the while loop; if no, we make nrtmPtr point to the data component
     * of the current ca_database_t structure; then, we check if this is
     * the database name that we want; if yes, we _break_ from the while
     * loop.
     */
    while (currentPtr != NULL) {
        nrtmPtr = currentPtr->data;
        if (strcmp(nrtmPtr->mrName, nrtmNameStr) == 0)
            break;
        currentPtr = currentPtr->next;
    }

    /*
     * We return a pointer.  If we found the mirror, this pointer
     * points to the ca_mirror_t structure which represents the
     * mirror. If we did not find the database, we return a pointer to
     * NULL.
     */
    if (currentPtr == NULL) {
        nrtmPtr = NULL;
        return (nrtmPtr);
    } else {
        return (nrtmPtr);
    }

}
