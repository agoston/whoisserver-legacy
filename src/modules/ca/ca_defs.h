/***************************************
  $Revision: 

  CA module: definitions header file for the configuration module.

  Status: NOT REVIEWED, NOT TESTED

  Author(s):       Ambrose Magee

******************/ /******************
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

#ifndef CA_DEFS
#define CA_DEFS

/************************************************************************
 * This is the definitions header file for the configuration module.  It
 * includes the definitions of data structures, external declarations and
 * definitions, definitions of sybolic constants.
 *
 ************************************************************************/

#include <pthread.h>
#include <glib.h>

/* Number of configurations variables. */
/* replaced by CA_NUMBEROFSYMBOLS in ca_dictionary.h
#define VARS 123 */

#define SCOPE_GLOBAL 1
#define SCOPE_LOCAL 99

/* 
 * Define the length of a string to be 160 to cope with the 
 * copyright statement.
 *
 */
#define STRLENGTH 256

/*
	* Define the length of strings to cope with the values of 
 * various types of string variables.
	*/
#define STRLENGTH_XS 40
#define STRLENGTH_S 80
#define STRLENGTH_M 160
#define STRLENGTH_L 320
#define STRLENGTH_XL 640
#define STRLENGTH_XXL 2560

/*
 *  Define the maximum number of sources
 *
 */
#define CA_MAXSOURCES  100


/**********************************************
 * Default values for the SOURCE variables		*
	*															*
 **********************************************/

 #define CA_DEFHOST "rowan"						
 #define CA_DEFPORT "4343"
 #define CA_DEFUSER "dbase"						
 #define CA_DEFPASSWORD "encrypt1"				
 #define CA_DEFDBNAME	"default-db"			
															


/**********************************************
 * Defintion of the dictionary structures.		*
	*															*
 **********************************************/

typedef struct dict_s	{
				char varName[STRLENGTH];
				char varSym[STRLENGTH];
				char varType[STRLENGTH];
				int varNum;
				int varMandatory;
				int varScope;
} dict_t;

extern dict_t dictionary[];




/**********************************************
 * Definition of the values structures.			*
 *															*
	**********************************************/

typedef struct values_s	{
			   GString *strPtr;	/* Pointer to the GString that contains the value. */
				void *valPtr;	/* Pointer to the actual value. */
} values_t;									

/*
 * "extern" definition of variables that are defined elsewhere.
 */


extern values_t globals[];
extern values_t locals[];

/*
 * "extern" definition of configuration variables, defined elsewhere.
 */
extern values_t confVars[];

/* Mutex lock; used for synchronising changes. */
pthread_mutex_t Lock;

/* 
	* New value of the bindport.
	* This must be a global variable.
 * This variable is no longer needed.
 * char newPort[16];
	*/

/*
 * The following is needed for the SOURCE variable.  First,
 * we define the "database" structure.  Then, we define the
 * structure of an element of the linked list.  Lastly, we 
 * define the linked list itself.
 */

typedef struct ca_database_s	{

			char host[64];
			int port;
      char trx_support[2]; /* added 24.10.2002 EG */
			char user[16];
			char password[64];
			char dbName[16];
		} ca_database_t;

typedef struct ca_mirror_s		{
			char host[64];
			int port;
			char log[64];
			int delay;
			int protocolVer;
			char mrName[16];
		} ca_mirror_t;

typedef struct ca_ripadmin_s	{
			char host[64];
			int port;
			char user[16];
			char password[64];
			char tableName[72];
		} ca_ripadmin_t;

typedef struct ca_database_list_s		{
			char name[16];	
			ca_database_t db;
			int opMode;
			ca_mirror_t nrtm;
			int updPort;
			char canupd[2];
			char deflook[2];
		} ca_database_list_t;

/*
	* Define the type of a source.
 * This is the name of a source and
 * the details of the database which 
 * makes this source.
 */
typedef struct ca_dbSource_s	{
			char name[16];
			ca_database_t db;
			int opMode;
			ca_mirror_t nrtm;
			int updPort;
			char canupd[2];
			char deflook[2];
		} ca_dbSource_t;

/*
 * Define the source handle:
 * this is a pointer to a source;
 * i.e. it is of type ca_dbSource_t.
 */
typedef ca_dbSource_t ca_SrcHdl_t;


/*
 * Define an updateSource.  This is used by dbupdate.
 *
 */
typedef struct ca_updDbSource_s	{
			char name[16];
			ca_database_t updDb;
			char whoisd_host[32];
			int qryPort;
			int updPort;
		} ca_updDbSource_t;



/*************************************************************
 * Definition of the default values for the SOURCE variable.	*
 *																				*
 *************************************************************/


/*
 * The linked-list of sources.
 *
 */
extern GSList *sourceList;

/*
	* The linked-list of databases and mirrors used by ca_readSources()
 */
extern GSList *dbList;
extern GSList *nrtmList;

/*
 * A varialbe of type GSList
 */
extern ca_dbSource_t *testSource;


/*
	* The test configuration file.
 * This is defined using a constant string, cf. Oualline, p.145.
 */
extern const char *testFile;
extern const char *tempFile;
extern const char *dictFile;
extern const char *confFile;
extern const char *sourcesFile;

/* 
 * Value returned by ca_getStorageLocation if the symbol for
 * a configuration variable cannot be found.
 *
	* This value is also returned by ca_getType, if it cannot map 
 * the name of a configuration variable to a data type.
 *
 */
#define NOT_FOUND -1 	
#define INCOMPLETE -1

/*
 * Definition of the identifiers used in the sources configuration file.
 */
#define DATABASE_KEY	"DATABASE"
#define NRTM_KEY	"NRTM"
#define SOURCE_KEY "SOURCE"


extern ca_dbSource_t *theSrc;

/*
	* Macros and const char * definitions for warning and error 
 * messages.
 */

extern const char *configWarningStr;
extern const char *configError_1Str;
extern const char *configError_2Str;
extern const char *configVarChk_OK_Str;


#endif /* CA_DEFS */
