/***************************************
 $Revision: 

 CA module: function prototypes.

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

#ifndef CA_CONFIGFNS_H
#define CA_CONFIGFNS_H 

#include "ca_defs.h"
#include <memwrap.h>

/**********************************************************************
 * This file contains the all the functions prototypes.					*
 **********************************************************************/

void stringPack(char *, const char *);

/* 
 * This function prototype is for a function which reads a file and 
 * splits it into tokens.
 *
 */
void opSplitsen(FILE *, gchar **);

/*********************************************************************
 * ca_populateDictionary() function -- populates the dictionary with the 
 * contents of the dictionary file.
 * 
 * Parameters
 *    woordenbook[] -- an array of dictionary structures.
 *  size -- the size of the array.
 *
 * Returns
 *  Nothing.
 *
 *********************************************************************/

void ca_populateDictionary(dict_t [], int);

void getDictionary(dict_t [], int);

/*
 * readConfig()
 * Reads the configuration text file.
 */

void ca_readConfig(const char *, values_t [], int);

/*
 * void ca_init(values_t [], values_t []);
 */
int ca_init(const char *);

/*
 * The get functions.
 */
int ca_get_int(int);
char *ca_get_dirlist(int);
char *ca_get_string(int);
int ca_get_boolean(int symbol);

/*
 * The set functions.
 */
void ca_set_int(int);
void ca_set_dirlist(int);
void ca_set_string(int);
void ca_set_boolean(int symbol);

/*
 * The get functions for the database and SOURCE variables.
 */
void ca_getDatabase(ca_database_t);
void ca_getSource(ca_database_list_t);
void ca_getSourcePtr(ca_database_list_t *);
void ca_getAllSources(GSList *);

/*
 * The get function for the Dictionary.
 *
 */
void ca_getDictionary(dict_t [], int);

/*
 * The get function for the string elements of the
 * the configuration structure.       
 */
void ca_getConfig(values_t [], int);

/*
 * A function that walks through the linked list of sources
 * and compares the name of each source with a user-supplied
 * name.  If it finds the name of the source, it prints the 
 * details of the source.
 */
void ca_getAsource(char *, GSList *);

/*
 * A function that walks through the linked list of sources
 * and compares the name of each source with a user-supplied
 * name.  If it finds the name of the source, it returns a
 * pointer to the structure that represents the source.
 */
ca_dbSource_t *ca_getSourceDetails(char *, GSList *);

/*
 * A function that returns a handle to a Source, 
 * given its position in the linked-list of sources.
 *
 */
ca_SrcHdl_t *ca_get_SourceHandleByPosition(int);

/* ca_get_SourceHandleByName(char *);
 *
 * A function that returns a handle to a Source, 
 * given its name in the linked-list of sources.
 *
 */
ca_SrcHdl_t *ca_get_SourceHandleByName(char *);

/*
 * Given a source-handle and an attribute, returns a
 * pointer to a character variable.
 */
char *ca_srchandle2Strelement(const ca_SrcHdl_t *, int);

/*
 * Given a source-handle and an attribute, returns an
 * integer.
 */
int ca_srchandle2Intelement(const ca_SrcHdl_t *ah, int srcAttrib);

/*
 * Returns a string element of the admin db structure.
 */
char *ca_get_adminStrElement(int symbol, int adminAttrib);

/*
 * Returns an int element of the admin db structure.
 */
int ca_get_adminIntElement(int symbol, int adminAttrib);

/*
 * Writes a new value of a variable back to the configuration file;
 * returns 0 on successful completion; otherwise it dies.
 */
int ca_writeNewValue(int, char *);

/*
 * Given a symbol from the list of symbols of variable types,
 * this returns a handle (pointer) to the Update Sources array.
 */
ca_updDbSource_t **ca_get_UpdSourceHandle(int);

int ca_get_UpdSourceNum();

/*
 * Given a pointer to the Update Source and a symbol from the list
 * of symbols of attributes, this returns a copy of a string
 * attribute of the Update Source variable.
 */
char *ca_UpdSrcHandle2StrElement(ca_updDbSource_t *, int);

/*
 * Given a pointer to the Update Source and a symbol from the list
 * of symbols of attributes, this returns the value of an integer
 * attribute of the Update Source variable.
 */
int ca_UpdSrcHandle2IntElement(ca_updDbSource_t *, int);

/*
 * Given the name of the sources file and the array of values,
 * this function reads the definitions of databases, mirrors, etc.
 * and loads the SOURCE variable.
 */
void ca_readSources(const char *, values_t []);

/*
 * Given a pointer to the linked list of databases,
 * this function returns the details of every database 
 * in the list.
 */
void ca_getAllDatabases(GSList *);

/*
 * Given a pointer to the linked list of mirrors,
 * this function returns the details of every mirror
 * in the list.
 */
void ca_getAllMirrors(GSList *);

/* ca_get_DbHandleByName(char *);
 *
 * A function that returns a handle to a Database
 * given its name in the linked-list of databases.
 *
 */
ca_database_t *ca_getDbHandleByName(char *);
ca_SrcHdl_t *ca_get_SourceHandleByName(char *);
ca_mirror_t *ca_getNrtmHandleByName(char *);

/*
 * ca_sanityChk()
 * 	-- Checks if all variables in the dictionary have been defined 
 * 		in the configuration file.
 *
 */
int ca_sanityChk(values_t []);
int ca_sanityCheck(values_t []);

/*
 * ca_mandVarChk()
 * - checks if all the mandatory variables in the dictionary have 
 *   been defined in the configuration file.
 */
int ca_mandVarChk();

/*
 * ca_conifigCheck()
 * 	- wrapper for the ca_sanityCheck() function.
 */
int ca_conifigCheck();

#endif
