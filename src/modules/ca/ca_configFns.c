/***************************************
 $Revision:

 CA module: definitions of most functions.

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

/* FIXME: This is a horrible piece of code. There is no sense of even trying to optimize or fix it - it's fubar.
 * Should be thrown out as it is.
 * agoston, 2007-11-16 */

#define DICT_INIT
#include "rip.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <unistd.h>

int num_sources=0;

/* #define DEBUG */

/**********************************************
 * This file contains the definitions of all  *
 * the functions.                    *
 **********************************************/

void stringPack(char *dest, const char *source)
/****************************************************************
 * stringPack -- function to rewrite a line of text with only   *
 *           one blankspace between each word.          *
 *                                          *
 * Parameters                                  *
 *  dest -- destination character, the character to be       *
 *        outputted                              *
 *  source -- the 'source' character, the original character.  *
 *                                          *
 * Returns                                    *
 *    Nothing (may change this to the number of characters    *
 *    read or copied).                            *
 *                                          *
 ****************************************************************/
{
#ifdef DEBUG
    printf("\nInside stringPack function\n");
#endif	/* DEBUG */

    /*
     * This while loop continues until the NULL character is copied into
     * the destination string.  If a tab character is copied into the
     * destination string, it is replaced with a blank-space character.
     *
     * Multiple blank-space and/or tab characters are skipped in the source
     * string until any other character is found.
     */

    while (1) {
        *dest = *source;

        if (*dest == '\t')
            (*dest = ' ');

        /* Exit if have copied the end of the string. */
        if (*dest == '\0')
            return;

        /*
         * If the source character was a blank-space or a tab, move
         * to the next source character.  While the source character
         * is a blank-space or a tab, move to the next character
         * (i.e. ignore these characters).  When any other character
         * is found in the source string, move to the next element of
         * the destination string.
         *
         * Otherwise, simultaneously, move to the next elements of the
         * destination and the source strings.
         */

        if ((*source == ' ') || (*source == '\t')) {
            ++source;
            while ((*source == ' ') || (*source == '\t')) {
                ++source;
            }

            ++dest;
        } else {
            ++dest;
            ++source;
        }
    }
}

void ca_populateDictionary(dict_t woordenboek[], int size)
/*******************************************************************
 * ca_populateDictionary -- Parses dictionary file, initializes    *
 *                  the dictionary structure and writes    *
 *                  the file of dictionary symbols,       *
 *                  ca_dictSyms.h                  *
 *                                            *
 * Parameters                                    *
 *    woordenboek -- the dictionary to be populated          *
 *    size -- the total number of variables i.e. the size of the  *
 *           array of dict_t structures.  See D. & D., p.276    *
 *                                            *
 * Returns                                      *
 *    Nothing ?  (may change this later)                  *
 *                                            *
 *******************************************************************/

{
    const char *blankLine = "\n";
    const char *comment = "#";
    char line[120];
    char input[120];
    int entry = 0;
    FILE *dictPtr;
#ifdef DEBUG
    int i;
    FILE *defnPtr;
#endif	/* DEBUG */

    gchar **tokens; /* Pointer to an array of strings. */

    /*
     * Try to open the dictionary file for reading.  If it cannot be
     * opened, exit with an error.
     */
    if ((dictPtr = fopen("dictionary.txt", "r")) == NULL) {
        fprintf(stderr, "Error: Unable to open 'dictionary.txt'\n");
        die;
    }

    /*
     * DEBUG mode only. Try to open the definitions file for writing.  If
     * it cannot be opened,exit with an error
     */
#ifdef DEBUG
    if ((defnPtr = fopen("defs.txt", "w")) == NULL) {
        fprintf(stderr, "Error: Unable to open 'defs.txt'\n");
        die;
    }
#endif	/* DEBUG */

    /*
     * Read the file one line at a time; if the line begins with a
     * comment, ignore it; otherwise, split each line into tokens; print
     * each token. Assign each token to the appropriate member of the
     * appropriate element of the dictionary array.
     */

    fgets(input, sizeof(input), dictPtr);

    if ((strncmp(input, comment, 1) != 0) && (strncmp(input, blankLine, 1) != 0)) {
        /*
         * First remove the newline character. Then replace multiple
         * tab and space characters with single space characters.
         */

        /*
         * Remove the newline character, if present. Replace the last
         * character of the string array with with '\0'.
         */

        input[strlen(input) - 1] = '\0';

        /*
         * Now, remove the multiple space and tab characters.
         */

        stringPack(line, input);

        g_strchomp(line); /* Remove trailing w-space. */
#ifdef DEBUG
        puts(line);
#endif	/* DEBUG */

        tokens = ut_g_strsplit_v1(line, " ", 0);

#ifdef DEBUG
        for (i = 0; tokens[i] != NULL; i++)
        printf("tokens[%d] = %s\n", i, tokens[i]);
#endif	/* DEBUG */

        /*
         * We no longer need a variable for scope
         * woordenboek[entry].varScope = atoi(tokens[1]);
         */

        strcpy(woordenboek[entry].varName, tokens[0]);
        strcpy(woordenboek[entry].varSym, tokens[1]);
        strcpy(woordenboek[entry].varType, tokens[2]);
        woordenboek[entry].varNum = entry;

        /*
         * DEBUG mode only. Write the dictionary symbol and the entry
         * number to the definitions file.
         */
#ifdef DEBUG
        fprintf(defnPtr, "%s\t%d\n", tokens[1], entry);
#endif	/* DEBUG */

        ++entry;
        g_strfreev(tokens);
    }
    /*
     * Get the 2nd and subsequent line of the file.
     */

    fgets(input, sizeof(input), dictPtr);

    while (!feof(dictPtr)) {
        /*
         * Process the line if it is not a comment.
         */

        if ((strncmp(input, comment, 1) != 0) && (strncmp(input, blankLine, 1) != 0)) {
            /*
             * First remove the newline character. Then replace
             * multiple tab and space characters with single
             * space characters.
             */

            /*
             * Remove the newline character, if present. Replace
             * the last character of the string array with with
             * '\0'.
             */

            input[strlen(input) - 1] = '\0';

            /*
             * Now, remove the multiple space and tab characters.
             */

            stringPack(line, input);

            g_strchomp(line); /* Remove trailing w/space. */

#ifdef  DEBUG
            puts(line);
#endif	/* DEBUG */
            tokens = ut_g_strsplit_v1(line, " ", 0);

#ifdef DEBUG
            for (i = 0; tokens[i] != NULL; i++)
            printf("tokens[%d] = %s\n", i, tokens[i]);
#endif	/* DEBUG */

            /*
             * We no longer need to know the scope of a variable
             * woordenboek[entry].varScope = atoi(tokens[1]);
             */

            strcpy(woordenboek[entry].varName, tokens[0]);
            strcpy(woordenboek[entry].varSym, tokens[1]);
            strcpy(woordenboek[entry].varType, tokens[2]);
            woordenboek[entry].varNum = entry;

#ifdef DEBUG
            fprintf(defnPtr, "%s\t%d\n", tokens[1], entry);
#endif	/* DEBUG */

            ++entry;

            g_strfreev(tokens);
        }
        fgets(input, sizeof(input), dictPtr);
    }

    fclose(dictPtr);

#ifdef DEBUG
    fclose(defnPtr);
#endif	/* DEBUG */

} /* End of ca_populateDictionary() function. */

void opSplitsen(FILE * filePtr, gchar ** tokenArray)
/*******************************************************************
 * opSplitsen -- reads a file and splits it into  tokens.        *
 *                                            *
 * Parameters                                    *
 *  filePtr -- a text file                            *
 *  tokenArray -- pointer to an array of strings              *
 *                                            *
 * Returns                                      *
 *  Nothing                                      *
 *                                            *
 *******************************************************************/
{
    /*
     * Declaring character constants is safer than using #define.
     */

    const char *blankLine = "\n"; /* Declared as a string, not a
     * character. */
    const char *comment = "#"; /* Declared as a string. */
    char line[99];
    char input[99];
#ifdef DEBUG
    int lineNo = 0;
    int j;
#endif	/* DEBUG */

    fgets(input, sizeof(input), filePtr); /* Get the (first) line from
     * the */
    /* file to which filePtr points. */

#ifdef DEBUG
    printf("\nFIRST INPUT >>> %s\n", input);
#endif	/* DEBUG */

    /* Compare the first character of the input */
    /* to the comment and the newline strings. */

    if ((strncmp(input, comment, 1) != 0) && (strncmp(input, blankLine, 1) != 0)) {
        /* Remove the newline character, if present. */
        /* Replace the last character */
        /* of the string array with '\0'. */

        input[strlen(input) - 1] = '\0';
#ifdef DEBUG
        printf("First Input >>> %s\n", input);
#endif	/* DEBUG */

        strcpy(line, input);
#ifdef DEBUG
        printf("First Line after copy >>> %s\n", line);
#endif	/* DEBUG */

        stringPack(line, input);
#ifdef DEBUG
        printf("Line: %s\n", line);
#endif	/* DEBUG */

        g_strchomp(line);
        /*
         * g_strdelimit(line, " ", ':'); g_strdelimit(line, "\t",
         * '*');
         */

#ifdef DEBUG
        printf("%3d> %s\n", ++lineNo, line);
#endif	/* DEBUG */

        /*
         * g_strsplit() is a GLib function; it returns an array of
         * strings.
         *
         * Here, we split on two spaces, "  ". We set max_tokenArray to
         * be 0.  We want the first token to be the name of the
         * variable and the other tokens to be the value of the
         * variable, qualifiers, etc.
         */

        tokenArray = ut_g_strsplit_v1(line, " ", 0);

#ifdef DEBUG
        for (j = 0; tokenArray[j] != NULL; j++)
        printf("token[%d] = %s\n", j, tokenArray[j]);
#endif	/* DEBUG */

    } /* End of processing the first line, if not commented. */

    /* End of getting the first line. */

    /* Get the 2nd line of the file. */
    fgets(input, sizeof(input), filePtr);

    while (!feof(filePtr)) {

        /* Process the line if it is not commented. */
        if ((strncmp(input, comment, 1) != 0) && (strncmp(input, blankLine, 1) != 0)) {
            /* Remove the newline character, if present. */
            input[strlen(input) - 1] = '\0';
#ifdef DEBUG
            printf("Subsequent Input >>> %s\n", input);
#endif	/* DEBUG */

            strcpy(line, input);
#ifdef DEBUG
            printf("Subsequent Line after copy >>> %s\n", line);
#endif	/* DEBUG */

            stringPack(line, input);
#ifdef DEBUG
            printf("Line: %s\n", line);
#endif	/* DEBUG */

            g_strchomp(line);

#ifdef DEBUG
            printf("%3d> %s\n", ++lineNo, line);
#endif	/* DEBUG */

            /*
             * See the comment above about the maximum number of
             * tokens being set to 0.
             */

            tokenArray = ut_g_strsplit_v1(line, " ", 0);

#ifdef DEBUG
            for (j = 0; tokenArray[j] != NULL; j++) {
                printf("token[%d] = %s\n", j, tokenArray[j]);
                /* Can also use puts(tokenArray[j]) here. */
            }
#endif	/* DEBUG */
        } /* Processed uncommented lines. */

        fgets(input, sizeof(input), filePtr);
    } /* Processed the 2nd & subsequent lines of the file. */

} /* End of processing the opened file. */

void ca_readConfig(const char *configFile, values_t confVars[], int size)
/*******************************************************************
 *                                            *
 * ca_readConfig -- parses the config file and writes the values   *
 *              into memory.                        *
 *                                            *
 * Parameters                                    *
 *    configFile -- the configuration file
 *    confVars[] -- the array of values structures            *
 *    size -- the number of configuration variables          *
 *                                             *
 * Returns                                      *
 *    Nothing -- perhaps make this return 0 on successful exit ?  *
 *                                            *
 * Note:   Should we make the name of the config file a global    *
 *      variable ?                                *

 *
 * Using UT_calloc; no need to check that a NULL pointer is
 * returned.
 *
 *
 *******************************************************************/
{
    FILE *confPtr; /* Pointer to config file. */
    char name[STRLENGTH_M]; /* The name of the config variable */
    /* 80 characters */
    char value[STRLENGTH_XXL]; /* The value of the variable */
    /* 640 characters */
    int location; /* Storage Location of the variable's value. */
    int type; /* Data type of the variable, represented by an
     * integer. */

    const char *blankLine = "\n"; /* Declared as a string, not a
     * character. */
    const char *comment = "#"; /* Declared as a string. */

    char source[16]; /* The name of a source. */
    char database[STRLENGTH_M]; /* The elements of a database. */
    /* 80 characters */

    /*
     * UPDSOURCE variables: whoisd host, query-port, update-port.
     */
    char updDetails[STRLENGTH_M]; /* The details of the update host: */
    /* the name of the qry & upd machine; */
    /* the query port; */
    /* the update port. */

    gchar **dbcomps; /* Pointer to an array of strings that
     * represents */
    /* the components of a db. */

    gchar **updDbcomps; /* Pointer to an array of strings that */
    /* represents the components of an UPD Source. */

    ca_ripadmin_t *newAdminPtr; /* A pointer to a new instance of */
    /* a ca_ripadmin_t variable.   */

    ca_database_t *newUpdDbPtr; /* A pointer to a new instance of */
    /* ca_database_t, for UPDSOURCE. */

    ca_updDbSource_t *newUpdSrc; /* A pointer to a new instance of */
    /* ca_updDbSource_t structure. */

#ifdef DEBUG
    int i; /* A counting variable used for debugging. */
#endif	/* DEBUG */

    /*
     * Function Prototype for ca_getStorageLocation() We put it here;
     * thus it can only be called from within ca_readConfig()
     *
     * This function finds the location in the values_t array where we store
     * pointers to the string value and the actual value of the variable.
     * It returns this location as an integer.
     *
     */
    int ca_getStorageLocation(char[], dict_t[], int);

    /*
     * Function Prototype for ca_getType() We put it here so that it can
     * only be called from within ca_readConfig()
     *
     * This function returns the type of the configuration variable.  It
     * returns it as a string.
     *
     */
    int ca_getType(char[], dict_t[], int);

#ifdef  DEBUG
    printf("\nInside readConfig() function.\n");
    printf("Configuration file is: %s\n", configFile);
#endif	/* DEBUG */

    /* create an aray for the multiple source data pointers */
    confVars[CA_UPDSOURCE].valPtr = (ca_updDbSource_t **) UT_malloc(CA_MAXSOURCES);
    /* first entry used to check for at least one valid source found */
    ((ca_updDbSource_t **)confVars[CA_UPDSOURCE].valPtr)[0] = NULL;

    /*
     * Open the configuration file for reading .....
     */
    if ((confPtr = fopen(configFile, "r")) == NULL) {
        printf("Error: file %s could not be opened.\n", configFile);
        die;
    }

    /*
     * Read the first record in the configuration file ..... We read the
     * _name_ of the variable using fscanf into a string array.  We read
     * the _value_ of the variable using fgets into an array; thus, we
     * can handle values of variables with qualifiers (e.g. SPLIT after
     * DBLIST) and values with blank characters (e.g. REPLYBANNER).
     */
    fscanf(confPtr, "%s", name);
    fgets(value, sizeof(value), confPtr);
    g_strstrip(value);

    /*
     * While there are records to be read in the config file. write the
     * current record into memory, read the next record in the config
     * file
     */

    while (!feof(confPtr)) {

        /*
         * From the variable name, find the dictionary number. The
         * dictionary number is defined as the place in the values
         * array in which to store the value of the variable.
         *
         */

        /*
         * Process the line only when/if it is not a comment or a
         * blankline.
         */
        if ((strncmp(name, comment, 1) != 0) && (strncmp(name, blankLine, 1) != 0)) {
            /*
             * If the last character of "value" is '\n', replace
             * it with '\0'.
             */
            if (value[strlen(value) - 1] == '\n') {
                value[strlen(value) - 1] = '\0';
            }

            /*
             * From the variable name, find the element of the
             * values array in which to store the value of the
             * variable.
             *
             */
            location = ca_getStorageLocation(name, dictionary, CA_NUMBEROFSYMBOLS);

#ifdef DEBUG
            printf("The location is: %d\n", location);
#endif	/* DEBUG */

            /*
             * See if the string value has already been stored;
             * if it has, then concatenate the new value to it;
             * if not, then allocate some memory and copy the
             * string into it.
             */

            /*
             * If this variable already exists, it has a non-zero
             * value and this 'if' statement returns a "true"
             * value. Otherwise, it returns a "zero" or "false"
             * value.
             */
            if (confVars[location].strPtr) {
                /*
                 * strcat(confVars[location].strPtr, "\n");
                 * strcat(confVars[location].strPtr, value);
                 */
                g_string_append(confVars[location].strPtr, "\n");
                g_string_append(confVars[location].strPtr, value);
            } else {
                /*
                 * Store a pointer to the string that
                 * contains the value This is not necessarily
                 * the actual value itself.
                 */

                confVars[location].strPtr = g_string_new(value);
            }

            /*
             * Now, store a pointer to the _value_ of the
             * variable.  Do this as follows: (a) get the _type_
             * of the variable (b) store a pointer to the value
             * of the variable in a way that depends on the
             * _type_ of the variable.
             */
#ifdef DEBUG
            printf("Variable \"%s\" is data-type \"%d\"\n", name, ca_getType(name, dictionary, CA_NUMBEROFSYMBOLS));
#endif	/* DEBUG */

            type = ca_getType(name, dictionary, CA_NUMBEROFSYMBOLS);

            /*
             * Given the _type_ of the variable, store the value
             * of the variable in the appropriate way.
             */
            switch (type) {
                case 11:

#ifdef DEBUG
                    puts("Data type is Integer");
#endif	/* DEBUG */

                    confVars[location].valPtr = UT_malloc(sizeof(int));

                    sscanf(value, "%d", (int *) confVars[location].valPtr);
                    break;

                case 12:

#ifdef DEBUG
                    puts("Data type is String !!! *** !!!");
#endif	/* DEBUG */

                    /*
                     * Test if this variable has already been
                     * created. Look for a non-zero i.e. true
                     * value.
                     *
                     * First put a '\n' character at the end of the
                     * existing string. Then, concatenate the
                     * additional string.
                     */
                    if (confVars[location].valPtr) {
#ifdef DEBUG
                        printf("\n%s variable already exists\n", name);
#endif	/* DEBUG */
                        g_string_append(confVars[location].valPtr, value);
                        g_string_append(confVars[location].valPtr, "\n");
                    } else {
                        /*
                         * If the variable has not already
                         * been created, then create it.
                         */
#ifdef DEBUG
                        printf("\n%s variable does not exist\n", name);
#endif	/* DEBUG */

                        /*
                         * We use g_string_new() to create a
                         * new GString. This is a _structure_
                         * of str and len.  The actual string
                         * is stored in the str component.
                         * Thus, when we want to access the
                         * string, we must look into
                         * structure.
                         */
                        confVars[location].valPtr = g_string_new(value);
                        g_string_append(confVars[location].valPtr, "\n");
                    }

                    break;

                case 13:
#ifdef DEBUG
                    puts("Data type is Dirlist");
#endif	/* DEBUG */
                    confVars[location].valPtr = (char *) UT_malloc(STRLENGTH);

                    strcpy(confVars[location].valPtr, value);
                    break;

                case 14:
#ifdef DEBUG
                    puts("Data type is Boolean");
#endif	/* DEBUG */

                    confVars[location].valPtr = UT_malloc(sizeof(int));

                    sscanf(value, "%d", (int *) confVars[location].valPtr);
                    break;

                case 16:
#ifdef DEBUG
                    puts("Found the CA_ADMIN stuff !!!");
#endif	/* DEBUG */
                    /*
                     * The elements of the Admin-DB have already
                     * been read in.
                     * Now, split up the elements and assign them
                     * to the
                     * components of the Admin-DB structure.
                     *
                     * First, separate the values in "value",
                     * using ',' as a
                     * delimiting character.
                     */
                    dbcomps = ut_g_strsplit_v1(value, ",", 0);

#ifdef DEBUG
                    for (i = 0; dbcomps[i] != NULL; i++)
                    printf("dbcomps[%d] = %s\n", i, dbcomps[i]);
#endif	/* DEBUG */

                    /*
                     * Now, allocate some memory to the
                     * newAdminPtr.
                     */
                    newAdminPtr = UT_calloc(1, sizeof(ca_ripadmin_t));

                    /*
                     * Now, assign the elements of the dbcomps
                     * array to the appropriate components of the
                     * structure to which newAdminPtr points.
                     */

                    /*
                     * Strip leading and trailing whitespace from
                     * dbcomps[0]
                     */
                    /*
                     * g_strstrip( dbcomps[0] );
                     */

                    strcpy(newAdminPtr->host, dbcomps[0]);
                    newAdminPtr->port = atoi(dbcomps[1]);
                    strcpy(newAdminPtr->user, dbcomps[2]);
                    strcpy(newAdminPtr->password, dbcomps[3]);
                    strcpy(newAdminPtr->tableName, dbcomps[4]);

                    g_strfreev(dbcomps);

#ifdef DEBUG
                    puts("Testing the population of the rip-admin db structure:");
                    printf("\n%s::%d::%s::%s::%s\n", newAdminPtr->host, newAdminPtr->port, newAdminPtr->user, newAdminPtr->password, newAdminPtr->tableName);
#endif	/* DEBUG */

                    /*
                     * Now, assign these values into the correct
                     * long-term storage.
                     */

                    confVars[location].valPtr = (ca_ripadmin_t *) UT_calloc(1, sizeof(ca_ripadmin_t));

                    memcpy(confVars[location].valPtr, newAdminPtr, sizeof(ca_ripadmin_t));

                    UT_free(newAdminPtr);

#ifdef DEBUG
                    printf("The ripadmin machine is: %s\n", ((ca_ripadmin_t *) confVars[location].valPtr)->host);
#endif	/* DEBUG */

                    break;

                case 17:
                    /*
                     * Found Update_Source variable.
                     There may be multiple instances of this
                     */
#ifdef DEBUG
                    printf("Found Update_Source variable !!!\n");
#endif	/* DEBUG */

#ifdef DEBUG
                    puts(name);
                    puts(value);
#endif	/* DEBUG */

                    /*
                     * Split the value into DB-name, DB-details,
                     * updDetails. Use blankspace as the
                     * delimiter between each of these variables.
                     */
                    sscanf(value, "%s %s %s", source, database, updDetails);
#ifdef  DEBUG
                    puts(source);
                    puts(database);
                    puts(updDetails);
#endif	/* DEBUG */

                    /*
                     * Using the values in "database", populate a
                     * ca_database_t structure. Give this
                     * variable a name.
                     */

                    /*
                     * First, separate the values in "database",
                     * using "," as as a delimiting  character.
                     */
                    dbcomps = ut_g_strsplit_v1(database, ",", 0);

#ifdef DEBUG
                    for (i = 0; dbcomps[i] != NULL; i++)
                    printf("dbcomps[%d] = %s\n", i, dbcomps[i]);
#endif	/* DEBUG */

                    /*
                     * Create a structure for this database.
                     */
                    newUpdDbPtr = UT_calloc(1, sizeof(ca_database_t));

                    strcpy(newUpdDbPtr->host, dbcomps[0]);
                    newUpdDbPtr->port = atoi(dbcomps[1]);
                    strcpy(newUpdDbPtr->user, dbcomps[2]);
                    strcpy(newUpdDbPtr->password, dbcomps[3]);
                    strcpy(newUpdDbPtr->dbName, dbcomps[4]);

                    g_strfreev(dbcomps);

#ifdef DEBUG
                    puts("Testing the population of the UPD db structure:");
                    printf("\n%s::%d::%s::%s::%s\n", newUpdDbPtr->host, newUpdDbPtr->port, newUpdDbPtr->user, newUpdDbPtr->password, newUpdDbPtr->dbName);
#endif	/* DEBUG */

                    /*
                     * Now, store the values contained in the
                     * updDetails string.
                     */

                    /*
                     * First, separate the values in the
                     * 'updDetails' string, using "," as a
                     * delimiting character.
                     */
                    updDbcomps = ut_g_strsplit_v1(updDetails, ",", 0);

#ifdef DEBUG
                    for (i = 0; updDbcomps[i] != NULL; i++)
                    printf("updDbcomps[%d] = %s\n", i, updDbcomps[i]);
#endif	/* DEBUG */

                    /*
                     * Using the above ca_database_t structure,
                     * the "source" value and the values of
                     * updDbcomps, populate the ca_updDbSource_t
                     * structure.
                     *
                     */

                    /*
                     * Create a new structure for this UPD
                     * Source.
                     */
                    newUpdSrc = UT_calloc(1, sizeof(ca_updDbSource_t));

#ifdef DEBUG
                    puts("Created a structure for the UPD Source variable");
#endif	/* DEBUG */

                    /*
                     * Now, populate this structure.
                     */

                    strcpy(newUpdSrc->name, source);
                    newUpdSrc->updDb = *newUpdDbPtr;
                    strcpy(newUpdSrc->whoisd_host, updDbcomps[0]);
                    newUpdSrc->qryPort = atoi(updDbcomps[1]);
                    newUpdSrc->updPort = atoi(updDbcomps[2]);

                    UT_free(newUpdDbPtr); /* Was copied */
                    g_strfreev(updDbcomps);

#ifdef DEBUG
                    puts("Testing the population of the ca_updDbSource_t structure:");
                    printf("Update Source name: %s\n", newUpdSrc->name);
                    printf("\nUPD-DB == %s::%d::%s::%s::%s\n", (newUpdSrc->updDb).host, (newUpdSrc->updDb).port, (newUpdSrc->updDb).user, (newUpdSrc->updDb).password, (newUpdSrc->updDb).dbName);
                    printf("\nUpdate Source Machine Details: %s::%d::%d\n", newUpdSrc->whoisd_host, newUpdSrc->qryPort, newUpdSrc->updPort);
#endif	/* DEBUG */

                    /*
                     * Now, assign these values into the correct
                     * long-term storage.
                     */

                    if (num_sources < CA_MAXSOURCES) {
                        ((ca_updDbSource_t **)confVars[location].valPtr)[num_sources]
                                = (ca_updDbSource_t *) UT_calloc(1, sizeof(ca_updDbSource_t));

                        memcpy(((ca_updDbSource_t **)confVars[location].valPtr)[num_sources++], newUpdSrc,
                                sizeof(ca_updDbSource_t));
                    } else {
                        fprintf(stderr, "Max number of Update Sources exceeded\n");
                        die;
                    }

                    /* No longer needed. */
                    UT_free(newUpdSrc);

#ifdef DEBUG
                    //				printf("UPD-Source/DB-details/user: %s\n", (((ca_updDbSource_t *) (confVars[location].valPtr)[num_sources-1])->updDb).user);
#endif	/* DEBUG */

                    break;

                default:
                    fprintf(stderr, "Data type not found for variable \"%s\".\n", name);
                    die;
                    break;
            }
        }

        fscanf(confPtr, "%s", name);
        fgets(value, sizeof(value), confPtr);
        g_strstrip(value);

    } /* End of processing the config file. */

} /* End of readConfig() function */

void ca_getDictionary(dict_t woordenboek[], int size) {
    int k;

    for (k = 0; k < size; k++) {
        printf("\nj = %d\n", k);
        /*
         * printf("%s\t%d\t%s\n", woordenboek[k].varName,
         * woordenboek[k].varScope, woordenboek[k].varType);
         */
        printf("%s\t%s\t%s\t%d\n", woordenboek[k].varName, woordenboek[k].varSym, woordenboek[k].varType,
                woordenboek[k].varNum);

    }
}

int ca_get_int(int symbol) {
    int *xPtr= NULL;

    /*
     * First print a message saying that the ca_get_int() function is
     * being called.
     */
#ifdef DEBUG
    printf("\nDEBUG: ca_get_int() function is called .....\n");
    printf("DEBUG: New value of StringPtr: %s\n",
            confVars[symbol].strPtr->str ? (char *)(confVars[symbol].strPtr->str) : "NULL");
#endif	/* DEBUG */

    /*
     * Look at the appropriate place in the dictionary; e.g. C_BINDPORT
     * => the first element, index = 0.
     *
     * if the varType is not an integer, exit with an error;
     *
     * otherwise, return an integer.
     *
     */

    /* Look at the appropriate place in the dictionary. */

#ifdef DEBUG
    printf("\nDEBUG: Variable type: %s\n", dictionary[symbol].varType);
    printf("variable: %s\n", dictionary[symbol].varName);
#endif	/* DEBUG */

    /* If the variable type is not an integer, exit with an error. */
    if (strcmp(dictionary[symbol].varType, "CA_INT") != 0) {
        fprintf(stderr, "Error: unexpected variable type.\n");
        die;
    } else {
        /*
         * Lock the value of the variable before reading it.
         */

        pthread_mutex_lock(&Lock);

        xPtr = confVars[symbol].valPtr;
        /*
         * Unlock the value of the variable after reading it.
         */
        pthread_mutex_unlock(&Lock);
    }

    if (xPtr == NULL) {
        printf("Error: undefined integer variable: %s\n ", dictionary[symbol].varName);

        die;
    }
    return (*xPtr);
}

char * ca_get_dirlist(int symbol) {
    /*
     * This function returns a pointer to a character array.  Thus, we
     * need to declare such a pointer.
     *
     */

    char *xPtr= NULL;
#ifdef  DEBUG
    printf("\nca_get_dirlist() function is called .....\n");
#endif	/* DEBUG */

    /*
     * Look at the appropriate place in the dictionary; e.g. CA_HELP =>
     * the second element, index = 1.
     *
     * if the varType is not CA_DIRLIST, exit with an error;
     *
     * otherwise, return a pointer to the value.
     *
     */

    /* Look at the appropriate place in the dictionary. */
#ifdef DEBUG
    printf("\nVariable type: %s\n", dictionary[symbol].varType);
#endif	/* DEBUG */

    /* If the variable type is not CA_DIRLIST, exit with an error. */
    if (strcmp(dictionary[symbol].varType, "CA_DIRLIST") != 0) {
        fprintf(stderr, "Error: unexpected variable type.\n");
        die;
    } else {
        pthread_mutex_lock(&Lock);
        /*
         * Test if a value for this variable has been defined.  If
         * yes, return a copy of it.  If not, print an error message
         * and die.
         */
        if (confVars[symbol].valPtr) {
            xPtr = (UT_strdup(confVars[symbol].valPtr));
#ifdef DEBUG
            printf("Value: %s\n", xPtr);
#endif	/* DEBUG */
        } else {
            printf("Error: undefined DIRLIST variable: %s\n", dictionary[symbol].varName);
            die;
        }
        pthread_mutex_unlock(&Lock);
    }
    return (xPtr);
}

char * ca_get_string(int symbol) {
    /*
     * This function returns a pointer to a character array.  Thus, we
     * need to declare such a pointer.
     *
     */

    char *xPtr= NULL;
#ifdef  DEBUG
    printf("\nca_get_text() function is called .....\n");
#endif	/* DEBUG */

    /*
     * Look at the appropriate place in the dictionary; e.g.
     * CA_REPLYBANNER => the third element, index = 2.
     *
     * if the varType is not CA_STRING, exit with an error;
     *
     * otherwise, return the value.
     *
     */

    /* Look at the appropriate place in the dictionary. */

#ifdef DEBUG
    printf("\nVariable type: %s\n", dictionary[symbol].varType);
#endif	/* DEBUG */

    /* If the variable type is not CA_STRING, exit with an error. */
    if (strcmp(dictionary[symbol].varType, "CA_STRING") != 0) {
        fprintf(stderr, "Error: unexpected variable type.\n");
        printf("Error: variable: %s\n", dictionary[symbol].varName);
        die;
    } else {
        pthread_mutex_lock(&Lock);

        /*
         * Test if a value for this variable has been defined.  If
         * yes, return a copy of it.  If not, return a NULL pointer.
         */
        if (((GString *) confVars[symbol].valPtr)) {
            xPtr = (UT_strdup(((GString *) confVars[symbol].valPtr)->str));
#ifdef DEBUG
            printf("Value: %s\n", xPtr);
#endif	/* DEBUG */
        } else {
#ifdef DEBUG
            printf("STRING value is undefined !!!\n");
#endif	/* DEBUG */
            xPtr = NULL;
        }
        pthread_mutex_unlock(&Lock);
    }
    return (xPtr);
}

int ca_get_boolean(int symbol) {
    /**********************************************
     * ca_get_boolean()                  *
     *                               *
     *                              *
     * Parameters                      *
     *                              *
     *  symbol -- the symbol for the variable    *
     *                              *
     *                              *
     * Returns                        *
     *                              *
     *  1 if true, 0 if false.              *
     *                              *
     * Remarks                        *
     *                              *
     *   Is there a better way to implement     *
     *   Boolean values in C ?              *
     *                              *
     *********************************************/

    int *xPtr= NULL;

    /*
     * Print this message if in debug mode.
     *
     */
#ifdef DEBUG
    printf("\nca_get_boolean() function is called .....\n");
    //	printf("DEBUG 5: New value of StringPtr: %s\n", globals[symbol].strPtr);
#endif	/* DEBUG  */

    /**********************************************\
         *                              *
     * Here is how this works:              *
     *                               *
     * (a) Check that the type of variable whose   *
     *     value is being read is CA_BOOLEAN.    *
     *                              *
     * (b) Lock the value of the variable before  *
     *     reading it.                    *
     *                              *
     * (c) Depending on the scope of the variable  *
     *     look for it in the appropriate array.  *
     *                              *
     * (d) Read the value of the variable.      *
     *                              *
     * (e) Unlock the value of the variable after *
     *    reading it.                    *
     *                              *
     *                              *
     * Returns                        *
     *
     *  an integer value as follows:          *
     *    1 if the db is in testmode (true),              *
     *    0 if the db is not in testmode (false).          *
     \*********************************************/

    /*
     * Look at the appropriate place in the dictionary; e.g. CA_BOOLEAN =
     * the fifth element of the dict_t array, => index = 4.
     *
     * If the varType is not Boolean, exit with an error
     *
     * Otherwise,
     *
     */

#ifdef DEBUG
    /* Look in the appropriate place in the dictionary. */
    printf("\nVariable type: %s\n", dictionary[symbol].varType);
#endif	/* DEBUG */

    /* If the variable type is not Boolean, exit with an error. */

    if (strcmp(dictionary[symbol].varType, "CA_BOOLEAN") != 0) {
        fprintf(stderr, "Error: Boolean type expected.\n");
        die;
    }

    else {

        /*
         * Otherwise, return an integer value.
         *
         */

        /*
         * Lock the value of the variable before reading it.
         *
         */

        pthread_mutex_lock(&Lock);
        xPtr = confVars[symbol].valPtr;
        /*
         * Unlock the value of the variable after reading it.
         */
        pthread_mutex_unlock(&Lock);

    }
    if (xPtr == NULL) {
        printf("Undefined Boolean variable: %s\n", dictionary[symbol].varName);
        die;
    }
    return (*xPtr);
}

void ca_set_int(int symbol) {
    /*********************************************
     * ca_set_int()                    *
     *                              *
     * Parameters                      *
     *    symbol -- the symbol for the variable.  *
     *                              *
     * Returns                        *
     *    1 if successful 0 if not ?          *
     *                              *
     * Remarks                        *
     *   Needs a better way to check for valid  *
     *    values from the keyboard.          *
     *                              *
     *********************************************/

    /*
     * void *tempPtr;
     *//* Temp pointer to point to the value pointer in the appropriate
     * values array. */
    char newPort[16];
    int invalid;
    int portNr;

    /*
     * Function to change the value in a given values array. This
     * function can only be called from within ca_set_int().
     */
    int *ca_change_int_value(char[]);
    void testFunction(values_t values[]);

    /*
     * Using the symbol, look at the appropriate place in the dictionary.
     */
#ifdef DEBUG
    printf("\nca_set_int() function called .....\n");
    printf("Variable type: %s\n", dictionary[symbol].varType);
#endif	/* DEBUG */

    /*
     * Make sure that a reasonable, sensible value of bind-port has been
     * read from the keyboard.
     */

    do {

        /*
         * First, flush input stream.
         */
        fflush(stdin);

        /*
         * Prompt for the new value of the bind-port.
         */

        printf("\nNew value of bind-port (non-zero positive integer) >>> ");
        scanf("%s", newPort);
        /*
         * gets(newPort);
         */
#ifdef DEBUG
        printf("\nDEBUG: Value of newPort variable: %s\n", newPort);
#endif	/* DEBUG */

        sscanf(newPort, "%d", &portNr);

#ifdef DEBUG
        printf("\nDEBUG: Value of integer variable, portNr: %d\n", portNr);
#endif	/* DEBUG */

        if (portNr < 0) {
            invalid = 1;
            puts("Only non-zero positive integer values accepted for bind-port");
        } else {
            invalid = 0;
        }

    } while (invalid);

    /*
     * Check that the function is attempting to set the correct type of
     * value.  If not, do not set the value and exit.
     */

    if (strcmp(dictionary[symbol].varType, "CA_INT") != 0) {
        fprintf(stderr, "Error: unexpected variable type.\n");
        die;
    }

    /*
     * Choose the appropriate values array.
     */
    switch (dictionary[symbol].varScope) {
        /*
         * If the variable has global scope, write it into the
         * globals array. If it has local scope, write it into the
         * local array. If the scope cannot be found, then report an
         * error.
         */
        case 1:
            globals[symbol].valPtr = ca_change_int_value(newPort);

            /************************************************************
             *																				*
             * We comment out this code.  We use the GLib string 			*
             * now.  It also checks if we got the memory :-)				*
             *																				*
             ************************************************************/

            /*
             * globals[symbol].strPtr = newPort;
             *
             * globals[symbol].strPtr = (char *) calloc(1, sizeof(newPort));
             */

            /*
             * Check the return value of calloc() to make sure that we
             * actually got the memory.
             */

            /*
             * if (globals[symbol].strPtr == NULL) {
             *		fprintf(stderr, "Cannot allocate memory for globals[symbol].strPtr.\n");
             *		die;
             * }
             */

#ifdef DEBUG
            printf("DEBUG: New value of StringPtr: %s\n", globals[symbol].strPtr);
#endif	/* DEBUG */

            /*
             * strcpy(globals[symbol].strPtr, newPort);
             */

            g_string_assign(globals[symbol].strPtr, newPort);

#ifdef DEBUG
            printf("DEBUG 2: New value of StringPtr: %s\n", globals[symbol].strPtr);
#endif	/* DEBUG */
            break;

        case 99:
            locals[symbol].valPtr = ca_change_int_value(newPort);
            /*
             * First allocate some memory and then copy the value of the
             * new Port into it.
             */

            /************************************************************
             *																				*
             * We comment out this code.  We use the GLib string 			*
             * now.  It also checks if we got the memory :-)				*
             *																				*
             ************************************************************/

            /*
             * locals[symbol].strPtr = (char *) calloc(1, sizeof(newPort));
             */

            /*
             * Now, check that the memory was actually allocated.
             */

            /*
             * if (locals[symbol].strPtr == NULL) {
             *		fprintf(stderr, "Cannot allocate memory for locals[symbol].strPtr\n");
             *	 exit(8);
             * }
             *
             * strcpy(locals[symbol].strPtr, newPort);
             */

            g_string_assign(locals[symbol].strPtr, newPort);

            /*
             * locals[symbol].strPtr = newPort;
             */
            break;

        default:
            fprintf(stderr, "Error; unknown scope: %d\n", dictionary[symbol].varScope);
            break;
    }

    /*
     * Write the new value of the variable to the correct place in this
     * array.  (First, set a mutex lock ???).
     */

    /*
     * Write the new value of this variable back to the config. file
     */

    ca_writeNewValue(symbol, newPort);

    printf("DEBUG 3: New value of StringPtr: %s\n", (globals[symbol].strPtr)->str);

}

int * ca_change_int_value(char value[]) {
    void *tempPtr;

    tempPtr = UT_malloc(sizeof(int));

    /*
     * No need to check the return value of UT_malloc() in case we did not
     * actually get the memory.
     *
     *if (tempPtr == NULL) {
     *	fprintf(stderr, "Cannot allocate memory for tempPtr\n");
     *	die;
     * }
     */

    sscanf(value, "%d", (int *) tempPtr);
    return (tempPtr);
}

void testFunction(values_t array[]) {
    printf("\nInside the Test function.\n");
}

void ca_getDatabase(ca_database_t db) {
    printf("\n%s\t%d\t%s\t%s\t%s\n", db.host, db.port, db.user, db.password, db.dbName);
}

void ca_getSource(ca_database_list_t src) {
    printf("\n%s\t%s\t%d\t%s\t%s\t%s\n", src.name, (src.db).host, (src.db).port, (src.db).user, (src.db).password, (src.db).dbName);
}

void ca_getAllSources(GSList * sources) {

    GSList *currentPtr; /* Pointer to the structure at which we look. */

    /*
     * Look at the first member of the linked-list of sources.
     */
    currentPtr = sources;

    /*
     * Look at each data component of the source list, untill we reach
     * the end of the list.
     */
    while (currentPtr != NULL) {
        ca_database_list_t *srcPtr = currentPtr->data;
        printf("\n%s\t%s\t%d\t%s\t%s\t%s\n", srcPtr->name, (srcPtr->db).host, (srcPtr->db).port, (srcPtr->db).user, (srcPtr->db).password, (srcPtr->db).dbName);
        currentPtr = currentPtr->next;
    }
}

void ca_getAsource(char *sourceName, GSList * sources)
/*******************************************************************
 * ca_getAsource -- looks for a source in the linked list        *
 *                                            *
 * Parameters                                    *
 *  sourceName -- the name of a source for which to look         *
 *  sources -- the list of sources in which to look            *
 *                                            *
 * Returns                                      *
 *  nothing, so far.                                *
 *                                            *
 *******************************************************************/
{

    GSList *currentPtr = sources;

#ifdef DEBUG
    printf("\nLooking for source: %s\n", sourceName);
#endif	/* DEBUG */

    /*
     * Look at each data component of the source list, compare the name
     * of the source with the sourceName untill we find the source o we
     * reach the end of the list
     */
    { /* Begin special block I got a syntax error when I defined
     * "ca_database_list_t *srcPtr = currentPtr->data;" in the
     * usual way, with all the other local variables.
     *
     * However, if I define it inside this block, I do not get any
     * syntax errors.
     *
     */

        ca_database_list_t *srcPtr = currentPtr->data;
#ifdef DEBUG
        printf("FirstSource is: %s\n", srcPtr->name);
#endif	/* DEBUG */
        while ((currentPtr != NULL) && (strcmp(srcPtr->name, sourceName) != 0)) {
#ifdef DEBUG
            puts("Now printing the current source .....");
            printf("CurrentSource is: %s\n", srcPtr->name);
            printf("%d\n", strcmp(srcPtr->name, sourceName));
            if (strcmp(srcPtr->name, sourceName) == 0) {
                printf("Found it !!! Source: %s\n", srcPtr->name);
            }
#endif	/* DEBUG */
            currentPtr = currentPtr->next;
            puts("currentPtr = currentPtr->next");
            if (currentPtr != NULL) {
                srcPtr = currentPtr->data;
                puts("srcPtr = currentPtr->data");
            }
#ifdef DEBUG
            puts("At the end of the while loop inside ca_getAsource function .....");
            printf("The NewSource is: %s\n", srcPtr->name);
#endif	/* DEBUG */
        }
#ifdef DEBUG
        puts("Exited from while loop in ca_getAsource function .....");
#endif	/* DEBUG */

        if (currentPtr != NULL) {
            printf("\nFound the source: %s\n", srcPtr->name);
            /*
             * printf("\n%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
             * srcPtr->name, (srcPtr->db).host,
             * (srcPtr->db).port, (srcPtr->db).user,
             * (srcPtr->db).password, (srcPtr->db).canupd,
             * (srcPtr->db).deflook, (srcPtr->db).dbName);
             */
        } else {
            printf("\nCould not find source: %s\n", sourceName);
        }
    } /* End special block */

}

ca_dbSource_t * ca_getSourceDetails(char *sourceName, GSList * sources)
/*******************************************************************
 * ca_getSourceDetails                              *
 *   -- A function that compares each 'name' component of every     *
 *    ca_database_list_t element in the linked-list of sources    *
 *    (the first element of which is a parameter of this function)*
 *    with the name of the source to be found.  If the required  *
 *    source is found, a pointer to the structure representing   *
 *     this source is returned.                        *
 *                                            *
 *  Parameters                                    *
 *  --  sourceName - the name of the required source            *
 *  --  sources  - the list of sources in which to look          *
 *                                            *
 *   Returns                                      *
 *  -- srcPtr - a pointer to the structure representing the source  *
 *            - or a pointer to NULL, if we cannot find the source *
 *                                            *
 *******************************************************************/
{
    /*
     * Define a pointer to the current element in the linked list.
     * Initialise it to the start of the list;
     */
    GSList *currentPtr = sources;

    /*
     * Define and initialise a pointer that points to the 'data'
     * component of the GSList struct; i.e. a pointer to a variable of
     * type ca_dbSource_t.
     */
    ca_dbSource_t *srcPtr = currentPtr->data;

    /*
     * Look at each data component of list of sources; (each data
     * component is a structure of type ca_dbSource_t i.e.
     * ca_database_list_t).  Compare the 'name' component of of each
     * ca_dbSource_t structure with the value of sourceName untill we get
     * a match or we reach the end of the list.
     */
    /*
     * We first check if currentPtr is pointing to NULL; if yes, we exit
     * the while loop; if no, we make srcPtr point to the data component
     * of the current dbSource structure; then, we check if this is the
     * source name that we want; if yes, we _break_ from the while loop.
     */
    while (currentPtr != NULL) {
        srcPtr = currentPtr->data;
        if (strcasecmp(srcPtr->name, sourceName) == 0)
            break;
        currentPtr = currentPtr->next;
    }

    /*
     * We return a pointer.  If we found the source, this pointer points
     * to the ca_dbSource_t structure which represents the source. If we
     * did not find the source, we return a pointer to NULL.
     */
    if (currentPtr == NULL) {
        srcPtr = NULL;
        return (srcPtr);
    } else {
        return (srcPtr);
    }

} /* End of ca_getSourceDetails function */

ca_SrcHdl_t * ca_get_SourceHandleByPosition(int position)
/*******************************************************************
 * ca_get_SourceHandleByPosition                        *
 *  -- retrieves the a handle to a Source                  *
 *                                            *
 * Parameters                                    *
 *  -- the position in the linked list of sources            *
 *                                            *
 *                                            *
 * Returns                                      *
 *  -- a pointer to the source or NULL                    *
 *    i.e. a pointer to the data component of the appropriate    *
 *    element in the linked list of sources.                *
 *******************************************************************/
{
    ca_dbSource_t *mySource;

    mySource = g_slist_nth_data(sourceList, position);
    return (mySource);
}

ca_SrcHdl_t * ca_get_SourceHandleByName(char *srcName)
/*******************************************************************
 * ca_get_SourceHandleByName                          *
 *  -- retrieves the a handle to a source                  *
 *                                            *
 * Parameters                                    *
 *  -- the name of the required source
 *                                            *
 *                                            *
 * Returns                                      *
 *  -- a pointer to the source or NULL                    *
 *    i.e. a pointer to the data component of the appropriate    *
 *    element in the linked list of sources.                *
 *******************************************************************/

{
    ca_dbSource_t *mySource;

    mySource = ca_getSourceDetails(srcName, sourceList);
    return (mySource);
}

char * ca_srchandle2Strelement(ca_SrcHdl_t * ah, int srcAttrib)
/*******************************************************************
 * ca_srchandle2Strelement                              *
 *  -- returns a string which represents the attribute of a source *
 *    e.g. returns the name of a source                  *
 *    It allocates the required memory;                  *
 *    but it returns NULL if the required memory cannot be       *
 *    allocated.
 *                                            *
 * Parameters                                     *
 *  --  source name - the name of the source
 *     ca_get_SourceHandleByName or ca_get_SourceHandleByPosition *
 *                                            *
 *  -- srcAttrib - an integer which represents the required       *
 *    attribute of the source.  We use #define statments to make  *
 *    a mapping between the attributes and the integers.        *
 *                                            *
 * Returns                                      *
 * -- a string or NULL                              *
 *******************************************************************/
{
    char *myStr;

    if (ah == NULL) {
        fprintf(stderr, "ca_srchandle2Strelement(): Cannot dereference NULL pointer\n");
        die;
    }

    pthread_mutex_lock(&Lock);
    switch (srcAttrib) {
        case 0:
            /* source name */
            myStr = UT_strdup(ah->name);
            break;

        case 1:
            /* canupd */
            myStr = UT_strdup(ah->canupd);
            break;

        case 2:
            /* deflook */
            /*
             * ca_malloc(myStr, 2); strcpy(myStr, (ah->db).deflook);
             */
            myStr = UT_strdup(ah->deflook);
            break;

        case 3:
            /* machine */
            myStr = UT_strdup((ah->db).host);
            break;

        case 5:
            /* user */
            myStr = UT_strdup((ah->db).user);
            break;

        case 6:
            /* password */
            myStr = UT_strdup((ah->db).password);
            break;

        case 7:
            /* dbName */
            myStr = UT_strdup((ah->db).dbName);
            break;

        case 9:
            /* Near-Real-Time Mirror host */
            if (ah->nrtm.host[0]) {
                myStr = UT_strdup((ah->nrtm).host);
            } else {
                fprintf(stderr, " *** Source %s does not have nrtm defined\n", ah->name);
                die;
            }
            break;

        case 11:
            /* NRTM Log */
            if (ah->nrtm.host[0]) {
                myStr = UT_strdup((ah->nrtm).log);
            } else {
                fprintf(stderr, " *** Source %s does not have nrtm defined\n", ah->name);
                die;
            }
            break;

        case 18: /* added 24.10.2002 EG */
            /* trx_support */
            myStr = UT_strdup((ah->db).trx_support);
            break;

        default:
            puts("Cannot find this source attribute");
            myStr = NULL;
    }
    pthread_mutex_unlock(&Lock);

    return (myStr);
}

int ca_srchandle2Intelement(ca_SrcHdl_t * ah, int srcAttrib)
/*******************************************************************
 * ca_srchandle2Intelement                            *
 *   -- a function that returns the integer value of the requested  *
 *     attribute of the given source.                    *
 *                                            *
 * Parameters                                    *
 *  --  source name - the name of the source
 *     ca_get_SourceHandleByName or ca_get_SourceHandleByPosition *
 *                                            *
 *  -- srcAttrib - an integer which represents the required       *
 *    attribute of the source.  We use #define statments to make  *
 *    a mapping between the attributes and the integers.        *
 *                                            *
 * Returns                                      *
 *  -- an integer.
 *******************************************************************/
{
    int myInt = 0; /* The value of this integer is returned. */

    if (ah == NULL) {
        fprintf(stderr, "ca_srchandle2Intelement(): Cannot dereference NULL pointer\n");
        die;
    }

    pthread_mutex_lock(&Lock);
    switch (srcAttrib) {

        case 4:
            /* DB Port */
            myInt = (ah->db).port;
            break;

        case 8:
            /* Mode of Operation of the Source. */
            myInt = ah->opMode;
            break;

        case 10:
            /* Near-Real-Time Mirror port */
            if (ah->nrtm.host[0]) {
                myInt = (ah->nrtm).port;
            } else {
                fprintf(stderr, " *** Source %s does not have nrtm defined\n", ah->name);
                die;
            }
            break;

        case 12:
            /* NRTM Delay */
            if (ah->nrtm.host[0]) {
                myInt = (ah->nrtm).delay;
            } else {
                fprintf(stderr, " *** Source %s does not have nrtm defined\n", ah->name);
                die;
            }
            break;

        case 13:
            /* NRTM Protocol Version. */
            if (ah->nrtm.host[0]) {
                myInt = (ah->nrtm).protocolVer;
            } else {
                 /* we need to know the nrtm version number on every source, but if there is no NRTM defined for a source,
                  * this call would fail.
                  * Returning '2' in such cases, as that is the default for local databases.
                  * agoston, 2009-01-20
                  */
                myInt = 2;
            }
            break;

        case 14:
            /* Source Update Port */
            myInt = ah->updPort;
            break;

        default:
            fprintf(stderr, "Could not find source-attribute %d\n", srcAttrib);
            die;
    }

    pthread_mutex_unlock(&Lock);
    return (myInt);
}

char * ca_get_adminStrElement(int symbol, int adminAttrib)
/*******************************************************************
 * ca_adminStrElement
 *  -- returns a string which represents the attribute of a admin  *
 *     db
 *    e.g. returns the name of a host machine.               *
 *    It allocates the required memory;                  *
 *    but it returns NULL if the required memory cannot be       *
 *    allocated.
 *                                            *
 * Parameters                                     *
 *  -- symbol - the symbol of the variable
 *                                            *
 *  -- adminAttrib - an integer which represents the required       *
 *    attribute of the Admin db.  We use #define statements to   *
 *    make a mapping between the attributes and the integers.    *
 *                                            *
 * Returns                                      *
 * -- a string or NULL                              *
 *******************************************************************/
{
    char *myStr= NULL;

    /*
     * Make sure that we are calling the correct function.
     */
    if (strcmp(dictionary[symbol].varType, "CA_ADMIN") != 0) {
        fprintf(stderr, "Error: unexpected variable type (%s/%s).\n",
        dictionary[symbol].varType, dictionary[symbol].varName);
        die;
    } else {
        pthread_mutex_lock(&Lock);
        switch (adminAttrib) {
            case 0:
                /* admin host */
                myStr = UT_strdup(((ca_ripadmin_t *) confVars[symbol].valPtr)->host);
                break;

            case 2:
                /* User */
                myStr = UT_strdup(((ca_ripadmin_t *) confVars[symbol].valPtr)->user);
                break;

            case 3:
                /* password */
                myStr = UT_strdup(((ca_ripadmin_t *) confVars[symbol].valPtr)->password);
                break;

            case 4:
                /* tableName */
                myStr = UT_strdup(((ca_ripadmin_t *) confVars[symbol].valPtr)->tableName);
                break;

            default:
                puts("Cannot find this admin attribute");
                die;
        }
        pthread_mutex_unlock(&Lock);

    }
    return (myStr);
}

int ca_get_adminIntElement(int symbol, int adminAttrib)
/*
 * Returns an int element of the admin db structure.
 */
{
    int myInt = 0; /* The value of this integer is returned. */

    pthread_mutex_lock(&Lock);
    switch (adminAttrib) {
        case 1:
            /* Port number */
            myInt = ((ca_ripadmin_t *) confVars[symbol].valPtr)->port;
            break;

        default:
            puts("Cannot find this admin attribute");
            die;
    }
    pthread_mutex_unlock(&Lock);

    return (myInt);
}

void ca_set_boolean(int symbol) {
    /*************************************************************
     *                                        *
     * ca_set_boolean()                            *
     *                                         *
     *                                        *
     * Parameters                                *
     *                                        *
     *   symbol -- the symbol for the variable.              *
     *                                        *
     *                                        *
     * Returns                                  *
     *                                        *
     *     nothing                                *
     *                                        *
     *                                        *
     * Remarks                                  *
     *                                        *
     *   Must check that a sensible value is given as input.    *
     *                                        *
     *                                        *
     *************************************************************/

    char newTestmodeStr[2];
    int newTestmodeVal; /* The new value of the testmode variable. */
    int invalid; /* Flag to indicate an invalid new value.  */

    FILE *testPtr, *tempPtr; /* The pointer to the files. */
    char name[STRLENGTH]; /* The name of the variable. */
    char value[STRLENGTH]; /* The value of the variable. */

    /*
     * Function to change the value in a given values array. This
     * function can only be called from within ca_set_boolean().
     */
    int *ca_change_int_value(char[]);

    /*
     * Using the symbol, look at the appropriate place in the dictionary.
     */
#ifdef DEBUG
    printf("\nca_set_int() function called .....\n");
    printf("Variable type: %s\n", dictionary[symbol].varType);
#endif	/* DEBUG */

    /*
     * Check that the function is attempting to set the correct type of
     * value.  If not, do not set the value, but exit instead.
     */

    if (strcmp(dictionary[symbol].varType, "CA_BOOLEAN") != 0) {
        fprintf(stderr, "Error: CA_BOOLEAN data type expected.\n");
        die;
    }

    /*
     * First, flush the input stream.
     */
    fflush(stdin);

    /*
     * Make sure that a reasonable, sensible value of bind-port has been
     * read from the keyboard.
     */

    do {
        /*
         * Prompt for the new value of the testmode.
         */

        printf("\nNew value of testmode (0 or 1) >>> ");
        scanf("%s", newTestmodeStr);

        /*
         * We scanf() the value as a string, but we want it to be an
         * integer.  Thus, we use sscanf() to scanf the value from
         * the string-variable and store it as an integer in an
         * integer variable.
         */
        sscanf(newTestmodeStr, "%d", &newTestmodeVal);

        /*
         * We only change the testmode when the user is absolutely
         * sure that they want to change.  Thus, we only accept two
         * possible values for testmode.
         */

        if ((newTestmodeVal < 0) || (newTestmodeVal > 1)) {
            invalid = 1;
            puts("Only '0' or '1' accepted as value for testmode.");
        } else {
            invalid = 0;
        }
    } while (invalid);

    /*
     * Lock the value of the variable before changing it.
     */

    pthread_mutex_lock(&Lock);

    /*
     * Choose the appropriate values array.
     */

    switch (dictionary[symbol].varScope) {
        /*
         * If the variable has global scope, write it into the
         * globals array. If it has local scope, write it into the
         * local array. If the scope cannot be found, then report an
         * error.
         */

        /************************************************************
         *																				*
         * We comment out this code.  We use the GLib string 			*
         * now.  It also checks if we got the memory :-)				*
         *																				*
         ************************************************************/

        case 1:
            globals[symbol].valPtr = ca_change_int_value(newTestmodeStr);
            /*
             * globals[symbol].strPtr = newTestmodeStr;
             */
            g_string_assign(globals[symbol].strPtr, newTestmodeStr);
            break;

        case 99:
            locals[symbol].valPtr = ca_change_int_value(newTestmodeStr);
            /*
             * locals[symbol].strPtr = newTestmodeStr;
             */
            g_string_assign(locals[symbol].strPtr, newTestmodeStr);
            break;

        default:
            fprintf(stderr, "Error: unknown scope: %d\n", dictionary[symbol].varScope);
            break;
    }

    /*
     * Write the new value of this variable back to the config file.
     *
     * To be implemented.
     */

    /*
     * Find the actual name of the variable from the dictionary structure
     * (use the variable symbol as an index into the array of dictionary
     * structures.
     */

    printf("Name of variable to be changed: %s\n", dictionary[symbol].varName);
    printf("Type of variable to be changed: %s\n", dictionary[symbol].varType);

    /*
     * Open the test config file for reading .....
     */
    if ((testPtr = fopen(testFile, "r")) == NULL) {
        printf("File \"%s\" could not be opened.\n", testFile);
        die;
    }

    /*
     * Open the temporary file for writing .....
     */
    if ((tempPtr = fopen(tempFile, "w")) == NULL) {
        printf("File \"%s\" could not be opened.\n", tempFile);
        die;
    }

    /*
     * Read the first record in the test config file.
     */

    fscanf(testPtr, "%s", name);
    fgets(value, sizeof(value), testPtr);

    /*
     * If the last character of "value" is '\n', replace it with '\0'.
     */
    if (value[strlen(value) - 1] == '\n') {
        printf("The value string is %s", value);
        printf("Replacing last character of \"%s\" with the NULL character\n", name);
        value[strlen(value) - 1] = '\0';
        printf("The new value string is %s", value);
    }

    /*
     * While there are records to be read in the test config file: Write
     * the current record into the temporary file. Read the next record
     * in the config file. Repeat untill the EOF has been reached.
     */

    while (!feof(testPtr)) {
        fprintf(tempPtr, "%s %s\n", name, value);
        fscanf(testPtr, "%s", name);
        fgets(value, sizeof(value), testPtr);

        /*
         * If the last character of "value" is '\n', replace it with
         * '\0'.
         */
        if (value[strlen(value) - 1] == '\n') {
            printf("The last character of the value string is %c", value[strlen(value) - 1]);
            printf("The value string is %s", value);
            printf("Replacing last character of \"%s\" with the NULL character\n", name);
            value[strlen(value) - 1] = '\0';
            printf("The new value string is %s", value);
        }

        /*
         * if we read the variable that we want to change, stop
         * reading this file and print only the name of this variable
         * to the temporary file.
         */

        /*
         * If we read the variable that we want to change, replace
         * the value of this variable in the config file with the
         * value supplied from the keyboard.
         *
         */
        if (strcmp(name, dictionary[symbol].varName) == 0) {
            strcpy(value, newTestmodeStr);
            printf("The replacement string is %s", value);
        }
        /*
         * Flush the pointer to the test config file.
         */
        fflush(testPtr);

    }
    /*
     * Here ends the loop that writes the config file, with the new
     * variable, to the temporary file.
     */

    /*
     * While !(the record to be updated) BEGIN Write the record to the
     * temporary file Read the next record in the config file END
     *
     * Write the new value to the temporary file Read the next record in the
     * config file COMMENT: this is the record to be updated. COMMENT:
     * discard this record.
     *
     * Read the next record in the config file
     *
     * While !(EOF) BEGIN write the record to the temporary file read the
     * next record in the config file END
     *
     * Close Config file Close Temporary file
     *
     * Open Temporary file for reading Open Config file for writing
     *
     * Read the next record of the Temporary file
     *
     * While (!EOF of Temporary file) BEGIN write the record into the Config
     * file read the next record of the Temporary file END
     *
     * Close Temporary file Close Config file
     *
     */

    fclose(testPtr);
    fclose(tempPtr);

    /*
     * Now, flush the file pointers
     */
    fflush(testPtr);
    fflush(tempPtr);

    /*
     * Open the temporary file for reading. Open the config file for
     * writing. Write the contents of the temporary file into the config
     * file.
     */

    /*
     * Open the temporary file for reading .....
     */
    if ((tempPtr = fopen(tempFile, "r")) == NULL) {
        printf("File \"%s\" could not be opened for reading.\n", tempFile);
        die;
    }

    /*
     * Open the config file for writing .....
     */
    if ((testPtr = fopen(testFile, "w")) == NULL) {
        printf("File \"%s\" could not be opened for writing.\n", testFile);
        die;
    }

    /*
     * Read the first record in the temporary file.
     */

    fscanf(tempPtr, "%s", name);
    fgets(value, sizeof(value), tempPtr);
    printf("\nFIRST LINE: %s %s", name, value);

    /*
     * While there are records to be read in the temporary file: Write
     * the current record into the test config file. Read the next record
     * in the temporary file. Repeat untill the EOF has been reached.
     */

    while (!feof(tempPtr)) {
        fprintf(testPtr, "%s %s", name, value);
        fscanf(tempPtr, "%s", name);
        fgets(value, sizeof(value), tempPtr);
    }

    fclose(testPtr);
    fclose(tempPtr);

    /*
     * Unlock the value of the variable after setting it and writing the
     * new value back to the configuration (and the dictionary) file.
     *
     */
    pthread_mutex_unlock(&Lock);

}

void ca_set_dirlist(int symbol) {
    /****************************************************************
     * ca_set_dirlist()                              *
     *                                          *
     * Parameters                                    *
     *    symbol -- the symbol of the variable.              *
     *                                          *
     * Returns                                    *
     *    1 if successful, 0 if not successful.              *
     *                                          *
     * Remarks                                    *
     *    Writing the new value back to the config file has yet to *
     *    be implemented.                            *
     *                                          *
     ****************************************************************/

    char newDir[80];
    /*
     * Declare a pointer to a values_t variable. Later, we shall assign
     * this pointer to the first element of either the globals or the
     * locals array, as appropriate.
     */
    values_t *hereValues= NULL;

    /*
     * Using the symbol, look in the appropriate place in the dictionary.
     */
#ifdef DEBUG
    printf("\nca_set_dirlist() function called ..... \n");
    printf("Variable type: %s\n", dictionary[symbol].varType);
#endif

    /*
     * First, flush the input stream.
     */
    fflush(stdin);

    /*
     * Prompt for the new value of the directory.
     */
    printf("\nNew value of %s [80 characters, maximum] >>> ", dictionary[symbol].varName);
    scanf("%s", newDir);

    /*
     * Make sure that a reasonable, sensible value of the directory value
     * has been read from the keyboard.
     *
     * How do we implement this ???
     *
     */

    /*
     * Make sure that the function is attempting to set the correct type
     * of value.  If not, do not set the value - and exit.
     */

    if (strcmp(dictionary[symbol].varType, "CA_DIRLIST") != 0) {
        fprintf(stderr, "Error: unexpected variable type.\n");
        exit(51);
    }

    /*
     * Choose the appropriate values array. Assign a temporary pointer to
     * this array.
     */

    switch (dictionary[symbol].varScope) {
        /*
         * If the variable has global scope, write it into the
         * globals array. If it has local scope, write it into the
         * locals array. If the scope cannot be found, report an
         * error.
         */
        case 1:
            hereValues = globals;
            break;

        case 99:
            hereValues = locals;
            break;

        default:
            fprintf(stderr, "Error: Unknown scope: %d\n", dictionary[symbol].varScope);
            die;
    }

    /*
     * Check for the presence of the mutex lock: if present, wait until
     * it is available; else get the lock and proceed with the change of
     * value.
     */

    /*
     * Write the new value of the variable to the correct place in the
     * [appropriate] values array.
     *
     * Note that there is no need to check if UT_malloc() actually worked.
     *
     */

    hereValues[symbol].valPtr = (char *) UT_malloc(80);

    /*
     *if (hereValues[symbol].valPtr == NULL) {
     *	fprintf(stderr, "Cannot alllocate memory for hereValuesvlPtr\n");
     *	die;
     * }
     */

    strcpy(hereValues[symbol].valPtr, newDir);

    /************************************************************
     *																				*
     * We comment out this code.  We use the GLib string 			*
     * now.  It also checks if we got the memory :-)				*
     *																				*
     ************************************************************/
    /*
     * hereValues[symbol].strPtr = (char *) malloc(sizeof(newDir));
     * if (hereValues[symbol].strPtr == NULL) {
     *		fprintf(stderr, "Cannot alllocate memory for hereValuestPtr\n");
     *	die;
     * }
     * strcpy(hereValues[symbol].strPtr, newDir);
     */
    g_string_assign(hereValues[symbol].strPtr, newDir);

    /*
     * Free the temporary pointer, hereValues.
     *
     */
    UT_free(hereValues);
    hereValues = NULL;

    /*
     * Release the mutex lock.
     */

    /*
     * Write the new value of this variable back to the config file.
     */

}

#if 0

XXX: Unused function? Removed because it causes warnings due to gets()
invokation. - Shane

void
ca_set_string(int symbol)
{

    /****************************************************************
     * ca_set_string()                              *
     *                                          *
     * Parameters                                    *
     *    symbol -- the symbol of the variable.              *
     *                                          *
     * Returns                                    *
     *    1 if successful, 0 if not successful ?              *
     *                                          *
     * Remarks                                    *
     *    Writing the new value back to the config file has yet to *
     *    be implemented.                            *
     *                                          *
     ****************************************************************/

    char newString[80]; /* May need to make this bigger. */

    /*
     * Declare a pointer to a values_t variable. Later, we shall assign
     * this pointer to the first element of either the globals or the
     * locals array, as appropriate.
     */
    values_t *hereValues;

    /*
     * Using the symbol, look in the appropriate place in the dictionary.
     */
#ifdef DEBUG
    printf("\nca_set_string() function called ..... \n");
    printf("Variable type: %s\n", dictionary[symbol].varType);
#endif

    /*
     * First, flush the input stream.
     */
    fflush(stdin);

    /*
     * Prompt for the new value of the string.
     */
    printf("\nNew value of %s [80 characters, maximum] >>> ", dictionary[symbol].varName);
    gets(newString);

    /*
     * Make sure that a reasonable, sensible value of the string value
     * has been read from the keyboard.
     *
     * How do we implement this ???
     *
     */

    /*
     * Make sure that the function is attempting to set the correct type
     * of value.  If not, do not set the value - and exit.
     */

    if (strcmp(dictionary[symbol].varType, "CA_STRING") != 0) {
        fprintf(stderr, "Error: unexpected variable type.\n");
        exit(51);
    }

    /*
     * Choose the appropriate values array. Assign a temporary pointer to
     * this array.
     */

    switch (dictionary[symbol].varScope) {
        /*
         * If the variable has global scope, write it into the
         * globals array. If it has local scope, write it into the
         * locals array. If the scope cannot be found, report an
         * error.
         */
        case 1:
        hereValues = globals;
        break;

        case 99:
        hereValues = locals;
        break;

        default:
        fprintf(stderr, "Error: Unknown scope: %d\n", dictionary[symbol].varScope);
        die;
    }

    /*
     * Check for the presence of the mutex lock: if present, wait until
     * it is available; else get the lock and proceed with the change of
     * value.
     */
    pthread_mutex_lock(&Lock);

    /*
     * Write the new value of the variable to the correct place in the
     * [appropriate] values array. Note the check to the return value of
     * malloc() to see if the memory was actually obtained.
     */

    hereValues[symbol].valPtr = (char *) UT_malloc(80);

    /*
     * No need to check that NULL is not returned.
     *
     *if (hereValues[symbol].valPtr == NULL) {
     *	fprintf(stderr, "Cannot allocate memory for hereValues[symbol].valPtr\n");
     *	die;
     * }
     *
     */

    strcpy(hereValues[symbol].valPtr, newString);

    /************************************************************
     *																				*
     * We comment out this code.  We use the GLib string 			*
     * now.  It also checks if we got the memory :-)				*
     *																				*
     ************************************************************/
    /*
     * hereValues[symbol].strPtr = (char *) malloc(sizeof(newString));
     * if (hereValues[symbol].strPtr == NULL) {
     *	fprintf(stderr, "Cannot allocate memory for hereValues[symbol].strPtr\n");
     *		die;
     * }
     * strcpy(hereValues[symbol].strPtr, newString);
     */

    g_string_assign(hereValues[symbol].strPtr, newString);

    /*
     * Free the temporary pointer, hereValues.
     *
     */
    UT_free(hereValues);
    hereValues = NULL;

    /*
     * Release the mutex lock.
     */
    pthread_mutex_unlock(&Lock);

    /*
     * Write the new value of this variable back to the config file.
     * Implement this later ?
     */

}
#endif /* 0 */

int ca_writeNewValue(int dictSymbol, char *newValue) {

    FILE *confPtr; /* Pointer to config file */
    FILE *tempPtr; /* The pointer to temp file. */
    char name[STRLENGTH]; /* The name of the variable. */
    char value[STRLENGTH]; /* The value of the variable. */

    /*
     * Find the actual name of the variable from the dictionary structure
     * (use the variable symbol as an index into the array of dictionary
     * structures.
     */
#ifdef DEBUG
    printf("Name of variable to be changed: %s\n", dictionary[dictSymbol].varName);
    printf("Type of variable to be changed: %s\n", dictionary[dictSymbol].varType);
#endif	/* DEBUG */

    /*
     * Open the test config file for reading .....
     */
    if ((confPtr = fopen(testFile, "r")) == NULL) {
        printf("File \"%s\" could not be opened.\n", testFile);
        die;
    }

    /*
     * Open the temporary file for writing .....
     */
    if ((tempPtr = fopen(tempFile, "w")) == NULL) {
        printf("File \"%s\" could not be opened.\n", tempFile);
        die;
    }

    /*
     * Read the first record in the test config file.
     */

    fscanf(confPtr, "%s", name);
    fgets(value, sizeof(value), confPtr);

    /*
     * If the last character of "value" is '\n', replace it with '\0'.
     */
    if (value[strlen(value) - 1] == '\n') {
#ifdef DEBUG
        printf("The value string is %s", value);
        printf("Replacing last character of \"%s\" with the NULL character\n", name);
#endif	/* DEBUG */

        value[strlen(value) - 1] = '\0';

#ifdef DEBUG
        printf("The new value string is %s", value);
#endif	/* DEBUG */
    }

    /*
     * If we read the variable that we want to change, replace the value
     * of this variable in the config file with the value supplied from
     * the keyboard.
     *
     */
    if (strcmp(name, dictionary[dictSymbol].varName) == 0) {
        strcpy(value, newValue);

#ifdef DEBUG
        printf("The replacement string is %s", value);
#endif	/* DEBUG */
    }

    /*
     * While there are records to be read in the test config file: Write
     * the current record into the temporary file. Read the next record
     * in the config file. Repeat untill the EOF has been reached.
     */

    while (!feof(confPtr)) {
        fprintf(tempPtr, "%s %s\n", name, value);
        fscanf(confPtr, "%s", name);
        fgets(value, sizeof(value), confPtr);

        /*
         * If the last character of "value" is '\n', replace it with
         * '\0'.
         */
        if (value[strlen(value) - 1] == '\n') {
#ifdef DEBUG
            printf("The last character of the value string is %c", value[strlen(value) - 1]);
            printf("The value string is %s", value);
            printf("Replacing last character of \"%s\" with the NULL character\n", name);
#endif	/* DEBUG */

            value[strlen(value) - 1] = '\0';
#ifdef DEBUG
            printf("The new value string is %s", value);
#endif	/* DEBUG */
        }

        /*
         * If we read the variable that we want to change, replace
         * the value of this variable in the config file with the
         * value supplied from the keyboard.
         *
         */
        if (strcmp(name, dictionary[dictSymbol].varName) == 0) {
            strcpy(value, newValue);

#ifdef DEBUG
            printf("The replacement string is %s", value);
#endif	/* DEBUG */
        }

        /*
         * Flush the pointer to the test config file.
         */
        fflush(confPtr);

    }
    /*
     * Here ends the loop that writes the config file, with the new
     * variable, to the temporary file.
     */

    /*
     * While !(the record to be updated) BEGIN Write the record to the
     * temporary file Read the next record in the config file END
     *
     * Write the new value to the temporary file Read the next record in the
     * config file COMMENT: this is the record to be updated. COMMENT:
     * discard this record.
     *
     * Read the next record in the config file
     *
     * While !(EOF) BEGIN write the record to the temporary file read the
     * next record in the config file END
     *
     * Close Config file Close Temporary file
     *
     * Open Temporary file for reading Open Config file for writing
     *
     * Read the next record of the Temporary file
     *
     * While (!EOF of Temporary file) BEGIN write the record into the Config
     * file read the next record of the Temporary file END
     *
     * Close Temporary file Close Config file
     *
     */

    fclose(confPtr);
    fclose(tempPtr);

    /*
     * Now, flush the file pointers
     */
    fflush(confPtr);
    fflush(tempPtr);

    /*
     * Open the temporary file for reading. Open the config file for
     * writing. Write the contents of the temporary file into the config
     * file.
     */

    /*
     * Open the temporary file for reading .....
     */
    if ((tempPtr = fopen(tempFile, "r")) == NULL) {
        printf("File \"%s\" could not be opened for reading.\n", tempFile);
        die;
    }

    /*
     * Open the config file for writing .....
     */
    if ((confPtr = fopen(testFile, "w")) == NULL) {
        printf("File \"%s\" could not be opened for writing.\n", testFile);
        die;
    }

    /*
     * Read the first record in the temporary file.
     */

    fscanf(tempPtr, "%s", name);
    fgets(value, sizeof(value), tempPtr);
#ifdef DEBUG
    printf("\nFIRST LINE: %s %s", name, value);
#endif	/* DEBUG */

    /*
     * While there are records to be read in the temporary file: Write
     * the current record into the test config file. Read the next record
     * in the temporary file. Repeat untill the EOF has been reached.
     */

    while (!feof(tempPtr)) {
        fprintf(confPtr, "%s %s", name, value);
        fscanf(tempPtr, "%s", name);
        fgets(value, sizeof(value), tempPtr);
    }

    fclose(confPtr);
    fclose(tempPtr);
    unlink(tempFile);

    return (0);
}

int ca_getStorageLocation(char *confVar, dict_t woordenboek[], int size)
/*************************************************************
 * ca_getStorageLocation()                        *
 *  - takes the name of a config variable and searches the    *
 *    dictionary structure for the storage location for this *
 *     variable.                                *
 *                                        *
 * Parameters                                *
 *  confVar -- the string variable that contains the name    *
 *            of the variable.                    *
 *  woordenboek -- the dictionary structure to be searched  *
 *  size      -- the size of the dictionary structure to  *
 *                 searched.                      *
 *                                        *
 * Returns                                  *
 *  the location (integer) in the values array.          *
 *                                        *
 *************************************************************/
{
    int i, where, found = 0; /* Whether or not the symbol has been
     * found. */

#ifdef DEBUG
    printf("The variable name in ca_getStorageLocation is: %s\n", confVar);
#endif	/* DEBUG */

    /*
     * Compares each name in the dictionary with the one for which we are
     * looking.
     */
    i = 0;
    while (!found && i < size) {
        if (strcmp(woordenboek[i].varName, confVar) == 0) {
            found = 1;
        } else {
            ++i;
        }
    }

    /*
     * Returns the storage location for the given variable name or else
     * returns NOT_FOUND
     */
    if (found) {
        /* mySymbol = atoi(woordenboek[i].varSym);  */
#ifdef DEBUG
        printf("Symbol is %s\n", woordenboek[i].varSym);
        printf("Storage Location is: %d\n", woordenboek[i].varNum);
#endif	/* DEBUG */
        where = woordenboek[i].varNum;
    } else {
        fprintf(stderr, "Error: cannot find storage location for variable %s\n", confVar);
        where = NOT_FOUND;
    }
    return (where);

}

void ca_getConfig(values_t confVars[], int size)
/*************************************************************
 * ca_getConfig -- prints the strings representing the     *
 *              values of the configuration variables    *
 *                                        *
 * Parameters                                *
 *    confVars -- the values_t array which stores the     *
 *            values of the configuration variables.     *
 *    size -- the number of configuration variables,      *
 *            the number of elements in the confVars array  *
 *                                        *
 *                                        *
 *************************************************************/
{
    int i = 0; /* A counting variable. */

    puts("A dump of the strings of the values of the Config Vars:");
    puts("Number\t\tString");
    puts("----------");

    while (i < size) {
        printf("%d\t\t%s\n", i, (confVars[i].strPtr)->str);
        ++i;
    }

}

int ca_getType(char *confVar, dict_t woordenboek[], int size)
/****************************************************************
 * ca_getType -- returns the data type of the variable.      *
 *                                          *
 * Parameters                                  *
 *    confVar -- the name of the configuration variable.      *
 *    woordenboek -- the array of dict_t structures.        *
 *    size -- the number of configuration variables.        *
 *                                          *
 * Returns                                    *
 *    an integer representing the data type of the variable    *
 *                                          *
 ****************************************************************/
{
    int i = 0, /* Counter variable. */
    found = 0; /* Set this == 1 when we find the variable.  */
    int myType; /* Integer representing the type of the config
     * variable. */

    /*
     * Compare each name in the dictionary with the one for which we are
     * looking.
     */

    myType = 0;

#ifdef DEBUG
    printf("ca_getType function called for variable: %s\n", confVar);
#endif	/* DEBUG */

    while (!found && i <= size) {
        if (strcmp(woordenboek[i].varName, confVar) == 0) {
            found = 1;
#ifdef DEBUG
            printf("ca_getType function: %s, %s matched.\n", woordenboek[i].varName, confVar);
#endif	/* DEBUG */
        } else {
            ++i;
        }
    }

    /*
     * Return the type of the config variable or else return "NOT FOUND".
     */
    if (found) {
        if (strcmp(woordenboek[i].varType, "CA_INT") == 0) {
#ifdef DEBUG
            printf("ca_getType function: %s variable of type %s is Integer type\n", woordenboek[i].varName, woordenboek[i].varType);

            printf("ca_getType function: %s, %s\n", woordenboek[i].varName, woordenboek[i].varType);
#endif	/* DEBUG */
            myType = 11;
#ifdef DEBUG
            printf("For type CA_INT, myType is %d\n", myType);
            printf("ca_getType function: %s, %s, %d\n", woordenboek[i].varName, woordenboek[i].varType, myType);
#endif	/* DEBUG */
        } else {
            if (strcmp(woordenboek[i].varType, "CA_STRING") == 0) {
#ifdef DEBUG
                printf("ca_getType function: %s variable of type %s is String type\n", woordenboek[i].varName, woordenboek[i].varType);
                printf("ca_getType function: %s, %s\n", woordenboek[i].varName, woordenboek[i].varType);
#endif	/* DEBUG */
                myType = 12;
#ifdef DEBUG
                printf("ca_getType function: %s, %s, %d\n", woordenboek[i].varName, woordenboek[i].varType, myType);
#endif	/* DEBUG */
            } else {
                if (strcmp(woordenboek[i].varType, "CA_DIRLIST") == 0) {
#ifdef DEBUG
                    printf("ca_getType function: %s, %s\n", woordenboek[i].varName, woordenboek[i].varType);
#endif	/* DEBUG */
                    myType = 13;
#ifdef DEBUG
                    printf("ca_getType function: %s, %s, %d\n", woordenboek[i].varName, woordenboek[i].varType, myType);
#endif	/* DEBUG */
                } else {
                    if (strcmp(woordenboek[i].varType, "CA_BOOLEAN") == 0) {
#ifdef DEBUG
                        printf("ca_getType function: %s, %s\n", woordenboek[i].varName, woordenboek[i].varType);
#endif	/* DEBUG */
                        myType = 14;
#ifdef DEBUG
                        printf("ca_getType function: %s, %s, %d\n", woordenboek[i].varName, woordenboek[i].varType, myType);
#endif	/* DEBUG */
                    } else {
                        if (strcmp(woordenboek[i].varType, "CA_SOURCETYPE") == 0) {
#ifdef DEBUG
                            printf("ca_getType function: %s, %s\n", woordenboek[i].varName, woordenboek[i].varType);
#endif	/* DEBUG */
                            myType = 15;
#ifdef DEBUG
                            printf("ca_getType function: %s, %s, %d\n", woordenboek[i].varName, woordenboek[i].varType, myType);
#endif	/* DEBUG */
                        } else {
                            if (strcmp(woordenboek[i].varType, "CA_ADMIN") == 0) {
#ifdef DEBUG
                                printf("ca_getType function: %s, %s\n", woordenboek[i].varName, woordenboek[i].varType);
#endif	/* DEBUG */
                                myType = 16;
#ifdef DEBUG
                                printf("ca_getType function: %s, %s, %d\n", woordenboek[i].varName, woordenboek[i].varType, myType);
#endif	/* DEBUG */

                            } else {
                                if (strcmp(woordenboek[i].varType, "CA_UPDSOURCE") == 0) {
#ifdef  DEBUG
                                    printf("ca_getType function: %s, %s\n", woordenboek[i].varName, woordenboek[i].varType);
#endif	/* DEBUG */
                                    myType = 17;
#ifdef DEBUG
                                    printf("ca_getType function: %s, %s, %d\n", woordenboek[i].varName, woordenboek[i].varType, myType);
#endif	/* DEBUG */
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        myType = NOT_FOUND;
    }
    return (myType);
}

ca_updDbSource_t ** ca_get_UpdSourceHandle(int symbol)
/*******************************************************************
 * ca_get_UpdSourceHandle                                          *
 * -- returns the handle to the Update source                      *
 *                                                                 *
 * Parameters                                                      *
 *  -- none; there is only one Update Source in the Configuration  *
 *     file because a single DBupdate process cannot update more   *
 *     than one source.                                            *
 *     There are now multiple Update Sources in the Configuration  *
 *     file. This was a self imposed limit that is no longer!!     *
 *                                                                 *
 * Returns                                                         *
 *  -- a pointer to an array of the Update Source structure (type  *
 *     ca_updDbSource_t) or NULL.                                  *
 *                                                                 *
 *******************************************************************/
{
    ca_updDbSource_t **myUpdSourcePtr= NULL;

    /*
     * Make sure that we are calling the correct function.
     */
    if (strcmp(dictionary[symbol].varType, "CA_UPDSOURCE") != 0) {
        fprintf(stderr, "Error: unexpected variable type.\n");
        die;
    } else {
        /* get a pointer to the array of source data pointers */
        myUpdSourcePtr = (ca_updDbSource_t **) confVars[symbol].valPtr;
    }
    return (myUpdSourcePtr);
}

/* returns the number of sources read from the config file
 num_sources is declared at the start of this source module
 */

int ca_get_UpdSourceNum() {
    return num_sources;
}

char * ca_UpdSrcHandle2StrElement(ca_updDbSource_t * uh, int srcAttrib)
/*******************************************************************
 * ca_UpdSrcHandle2StrElement                          *
 *   -- returns a string which represents the attribute of an     *
 *     update source e.g. the name, the user, etc.            *
 *    It allocates the required memory, but it returns NULL if    *
 *    the required memory cannot be allocated.              *
 *                                            *
 *                                            *
 * Parameters                                    *
 *  -- the Update Source Handle, i.e. a pointer to the structure  *
 *    which contains the data about the Update Source.        *
 *                                            *
 *  -- srcAttrib - an integer which represents the required      *
 *    attribute of the source.  This is also used in the       *
 *    ca_srchandle2Strelement() function.                  *
 *                                            *
 * Returns                                      *
 *  -- a string or NULL                              *
 *                                            *
 *******************************************************************/
{
    char *myStr;

    if (uh == NULL) {
        fprintf(stderr, "ca_UpdSrcHandle2StrElement(): Cannot dereference NULL pointer.\n");
        die;
    }

    switch (srcAttrib) {
        case 0:
            /* Update Source Name */
            myStr = UT_strdup(uh->name);
            break;

        case 3:
            /* Machine */
            myStr = UT_strdup((uh->updDb).host);
            break;

        case 5:
            /* User */
            myStr = UT_strdup((uh->updDb).user);
            break;

        case 6:
            /* Password */
            myStr = UT_strdup((uh->updDb).password);
            break;

        case 7:
            /* Update DB Name */
            myStr = UT_strdup((uh->updDb).dbName);
            break;

        case 15:
            /* Update Source Whois Machine */
            myStr = UT_strdup((uh->whoisd_host));
            break;

        default:
            puts("Cannot find this Update source attribute");
            myStr = NULL;
    }

    return (myStr);
}

int ca_UpdSrcHandle2IntElement(ca_updDbSource_t * uh, int srcAttrib)
/*******************************************************************
 * ca_UpdSrcHandle2IntElement                          *
 *   -- a function that returns the integer value of the requested  *
 *    attribute of the given source.                    *
 *                                            *
 * Parameters                                    *
 *  -- the Update Source Handle, i.e. a pointer to the structure  *
 *    which contains the data about the Update Source.            *
 *
 *  -- srcAttrib - an integer which represents the required      *
 *    attribute of the source.  This is also used in the       *
 *    ca_srchandle2Strelement() function.                  *
 *                                            *
 * Returns                                      *
 *  -- an integer.
 *******************************************************************/
{

    int myInt = 0; /* The value of this integer is returned. */

    if (uh == NULL) {
        fprintf(stderr, "ca_srchandle2Intelement(): Cannot dereference NULL pointer\n");
        die;
    }

    switch (srcAttrib) {

        case 4:
            /* Update Source DB Port */
            myInt = (uh->updDb).port;
            break;

        case 16:
            /* Update Source QRY Port */
            myInt = (uh->qryPort);
            break;

        case 17:
            /* Update Source UPD Port */
            myInt = (uh->updPort);
            break;

        default:
            fprintf(stderr, "Could not find source-attribute %d\n", srcAttrib);
            die;
    }

    return (myInt);

}

int ca_sanityChk(values_t confVars[])
/*
 - does a simple sanity check
 - Parameters
 - confVars - the array of configuration variables
 - Returns
 - an integer: -1 or 0
 */
{
    int symbol; /* A counting variable */
    int status = 0; /* Assume that the Configuration File is complete. */
    int undefVars = 0; /* Number of undefined variables. */

    for (symbol = 0; symbol < CA_NUMBEROFSYMBOLS; symbol++) {
        if (!confVars[symbol].strPtr) {
            ++undefVars;
            fprintf(stderr, "%s %s\n", configWarningStr, dictionary[symbol].varName);
        }
    }

    if (undefVars) {
        status = INCOMPLETE;
    }

    fprintf(stderr, "%s\n", configError_1Str);
    fprintf(stderr, "%d%s\n", undefVars, configError_2Str);
    return (status);
}

int ca_mandVarChk(void)
/****************************************************************
 * ca_mandVarChk																*
 *	- Looks for undefined mandatory variables							*
 * Parameters																	*
 *	- confVars, the array of Configuration Variables				*
 * 	- dictionary, the dictionary of Configuration Variables		*
 *																					*
 * Returns																		*
 * an integer, -1 or 0.														*
 *																					*
 ****************************************************************/
{
    int symbol; /* A counting variable */
    int status = 0; /* Assume that the Configuration File is complete. */
    int undefVars = 0; /* Number of undefined variables. */

    /*
     * This output does not tell us anything useful.
     * Thus, we comment it out.
     *
     * puts("Running mandatory variables check .....");
     */

    for (symbol = 0; symbol < CA_NUMBEROFSYMBOLS; symbol++) {
        if (dictionary[symbol].varMandatory && (!confVars[symbol].strPtr)) {
            ++undefVars;
            fprintf(stderr, "%s %s\n", configWarningStr, dictionary[symbol].varName);
        }
    }

    if (undefVars) {
        status = INCOMPLETE;

        fprintf(stderr, "%s\n", configError_1Str);
        fprintf(stderr, "%d%s\n", undefVars, configError_2Str);
    } else {
        /*
         * This output does not give us new information.
         * Thus, we comment it out.
         *
         * fprintf(stderr, "%s\n", configVarChk_OK_Str);
         */
    }

    return (status);

}
