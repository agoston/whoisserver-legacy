/***************************************
  $Revision: 1.1 $

  Plug-in module.

  Status: REVIEWED, TESTED

 Author(s):       Tiago Antao

  ******************/ /******************
  Modification History:
        tiago (10/04/2003) Created.
  ******************/ /******************
  Copyright (c) 2003               RIPE NCC
 
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

#ifndef _PG_H_
#define _PG_H_

#include <glib.h>

/*
  Possible status codes
  PG_INIT          - In pre exeuction stage
  PG_OK            - Executed OK
  PG_ERROR_PROCEED - Error, but its possible to proceed
  PG_ERROR_STOP    - Immedeate stop error
  PG_ERROR_PLUGIN_NAME - A unknown name was given
 */
typedef enum {
  PG_INIT,
  PG_OK,
  PG_ERROR_PROCEED,
  PG_ERROR_STOP,
  PG_ERROR_PLUGIN_NAME
} PG_status_t;

/*
  A transaction, transparent.
 */
typedef void PG_transaction_t;

/*
  A plugin, transparent.
 */
typedef void PG_plugin_t;

/*
  The signature for a manage function.

  A manage function prepares/frees some data structures for a transaction.
 */
typedef void (*PG_manage_function_t)(PG_transaction_t* trans);

/*
  The signature of a plugin exectution function.

  trans   - Transaction
  my_data - Pointer to pointer to plugin memory area

  return - Status of execution
*/
typedef PG_status_t (*PG_call_function_t)(PG_transaction_t* trans, gpointer* my_data);

/*
  The signature of a plugin cleanup function.

  trans   - Transaction
  my_data - Pointer to plugin memory area

*/
typedef void (*PG_termination_function_t)(PG_transaction_t* trans, gpointer my_data);



/*
  Plugin system wide initialization function
 */
void PG_init();

/*
  Plugin system wide initialization function
 */
void PG_end();


/*
  Creates a new transaction.

  manage_function - function to manage intialization/end of transaction

  return - Transaction
*/
PG_transaction_t* PG_transaction_new(PG_manage_function_t manage_function);

/*
  Destroys a transaction

  trans - Transaction
 */
void PG_transaction_end(PG_transaction_t* trans);



/*
  Creates a new plugin

  name                 - Plugin name
  call_function        - The execution function
  termination_function - The cleanup function

  return - Plugin

  The plugin still has to be registered.
 */
PG_plugin_t* PG_plugin_new(
  gchar* name,
  PG_call_function_t call_function,
  PG_termination_function_t termination_function );

/*
  Registers a new plugin

  plugin - plugin
 */
void PG_register(PG_plugin_t* plugin);



/*
  Executes a plugin list

  Only a transation in PG_INIT state can execute.
 */
void PG_execute(PG_transaction_t* trans, GList* plugins);



/*
  Returns a list of plugin names available.

  return - List of string (plugin) names
 */
GList* PG_get_plugins();

/*
  Returns the plugin info for a plugin name.

  name - Name of the plugin

  return - plugin structure
 */
PG_plugin_t* PG_get_plugin(gchar* name);




/*
  Returns the current execution status of a transaction.

  trans - Transaction

  return - Status

  This can be executed both by the user or a plugin function.
 */
PG_status_t PG_status_get(PG_transaction_t* trans);




/*
  Sets the global memory area pointer.

  trans - Transaction
  data  - Pointer to the data
 */
void PG_global_set(PG_transaction_t* trans, gpointer data);

/*
  Gets the global memory area pointer.

  trans - Transaction

  return - Pointer to the data
 */
gpointer PG_global_get(PG_transaction_t* trans);



/*
  Sets the local memory area pointer.

  trans - Transaction
  data  - Pointer to the data
 */
void PG_local_info_set(PG_transaction_t* trans, gpointer data);

/*
  Gets the local memory area pointer.

  trans - Transaction

  return - Pointer to the data
 */
gpointer PG_local_info_get(PG_transaction_t* trans);

#endif
