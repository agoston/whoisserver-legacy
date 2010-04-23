/***************************************
  $Revision: 1.7 $

  rollback(), commit(), delete() - rollback, commit update transaction, delete an object

  Status: NOT REVUED, NOT TESTED

 Author(s):       Andrei Robachevsky

  ******************/ /******************
  Modification History:
        andrei (17/01/2000) Created.
  ******************/ /******************
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

#include "config.h"
#include "rip.h"
#include "UD_comrol.def"
#include "syntax_api.h"
#include "lg.h"


/* logging (defined in ud_main.c) */
extern LG_context_t *ud_context;


/************************************************************
* int ud_transaction_support()                              *
*                                                           *
* Returns 1 if the tr's database type supports transactions,*
* else it returns 0. This data is taken from configuration  *
* file                                                      *
*                                                           *
************************************************************/

int ud_transaction_support(Transaction_t *tr){
  char * supports_transaction;
  int support;

  supports_transaction = ca_get_srctrx_support(tr->source_hdl);
  /* if there's transaction support, then ca_get_srctrx_support would return 'y' (yes) */
  if( strcmp(supports_transaction, "y") == 0 ) {
    support = 1;
  }else{
    support = 0;
  }

  UT_free(supports_transaction); 
  return support; 
  
}/* ud_transaction_support */


/************************************************************
*  void _collect_tables()                                   *
*                                                           *
* This function is only called by ud_build_lock_query()    ,*
*                                                           *
************************************************************/

void _collect_tables(gpointer key, gpointer value, gpointer table_list) {
	GSList **ret = (GSList **)table_list;
	*ret = g_slist_prepend(*ret, (gpointer)key);
}

/************************************************************
* char *ud_build_lock_query()                               *
*                                                           *
* Expects a Transaction_t and assembles a query which locks *
* the affected tables.  It takes the table names from       *
* the definition in ud_comrol.h.                            * 
*                                                           *
* The second argument, common_tables is optional.  If it's  *
* not null, the tables listed there will also be included   *
* in the LOCK TABLES statement.                             *
*                                                           *
* The query will be build up inside the Transaction_t,      *
* tr->query.                                                *
*                                                           *
* Returns 1 if there was a problem, 0 if successful. Doesn't*
* do much error checking.                                   *
*                                                           *
************************************************************/

int ud_build_lock_query(Transaction_t *tr, char **common_tables) {

	int i;
	GSList *lock_tables;
	GSList *cur_table;
	GHashTable *tables_unique;

	/* Initialize hash -- this will be used to drop duplicate table names */
	tables_unique = g_hash_table_new(g_str_hash, g_str_equal);

	/* The base table for this class should always be locked */
	g_hash_table_insert(tables_unique, DF_get_class_sql_table(tr->class_type), (gpointer)1);

	/* If we got extra tables to be locked, put them on the list */
	if (common_tables != NULL) {
		for (i = 0; common_tables[i] != NULL; i++)
			g_hash_table_insert(tables_unique, common_tables[i], (gpointer)1);
	}

	/* Add tables defined in .../include/UD_comrol.def */
	for (i=0; tables[tr->class_type][i] != NULL && i < TAB_START; i++)
		g_hash_table_insert(tables_unique, tables[tr->class_type][i], (gpointer)1);

	for (i=TAB_START; tables[tr->class_type][i] != NULL; i++)
		g_hash_table_insert(tables_unique, tables[tr->class_type][i], (gpointer)1);

	/* No tables to lock -> nothing to do.
	 * Should this be an error condition?  I'm not sure. */
	if (!g_hash_table_size(tables_unique))
		return 1;

	/* Initialize a linked list which we're going to traverse when actually
	 get to build up the query */
	lock_tables = NULL;

	/* Copy all table names from the hash to this list */
	g_hash_table_foreach(tables_unique, _collect_tables, &lock_tables);

	/* Start building the query */
	g_string_sprintf(tr->query, "LOCK TABLES %s WRITE", (char*)(lock_tables->data));

	/* Add all tables on our list to the query */
	for (cur_table = g_slist_next(lock_tables); cur_table != NULL; cur_table = g_slist_next(cur_table)) {
		g_string_sprintfa(tr->query, ", %s WRITE", (char *)cur_table->data);
	}
	
	/* free the temp list and hash */
	g_slist_free(lock_tables);
	g_hash_table_destroy(tables_unique);

	return (0);
}


/************************************************************
* int UD_rollback()                                         *
*                                                           *
* Rolls back the transaction                                *
*                                                           *
* It locks all relevant tables and processes the rollback   *
* General approach is to delete all new records related     *
* to the transaction (thread_id==thread_ins) and clean up   *
* old ones (thread_id==thread_upd)                          *
*                                                           *
************************************************************/
 
int UD_rollback(Transaction_t *tr) {
int i, j;
int sql_err;
int transaction_support;

/* tables we'll have to LOCK regardless of object class */
char *common_tables[] = { "last", "history", NULL };

 if(ACT_DELETE(tr->action)) return(0);

 /* check if there's transaction support */	
 transaction_support = ud_transaction_support(tr);

 /* if our database's table type supports transactions, then we'll use "BEGIN - COMMIT". If not, we have to
    lock all relevant tables */
 if(transaction_support == 1)
   g_string_sprintf(tr->query, "BEGIN ");
      
 else
    ud_build_lock_query(tr, common_tables);

 sql_err=SQ_execute_query(tr->sql_connection, tr->query->str, NULL);

 if (sql_err) {
      LG_log(ud_context, LG_ERROR, "%s[%s]", SQ_error(tr->sql_connection), tr->query->str);
      tr->succeeded=0;
      tr->error |=ERROR_U_DBS;
      die;
 }

/* Process AUX and LEAF tables */
    for (i=TAB_START; tables[tr->class_type][i] != NULL; i++) {
    /* Delete what has been inserted */
    g_string_sprintf(tr->query, "DELETE FROM %s WHERE object_id=%ld AND thread_id=%d", tables[tr->class_type][i], tr->object_id, tr->thread_ins);
    sql_err=SQ_execute_query(tr->sql_connection, tr->query->str, NULL);

    /* Normalize what has been updated/touched */
    g_string_sprintf(tr->query, "UPDATE %s SET thread_id=0 WHERE object_id=%ld AND thread_id=%d", tables[tr->class_type][i], tr->object_id, tr->thread_upd);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
  }

/* Process MAIN tables */
/* Delete if a record was created */
    g_string_sprintf(tr->query, "DELETE FROM %s WHERE  object_id=%ld AND thread_id=%d", 
                             DF_get_class_sql_table(tr->class_type), tr->object_id, tr->thread_ins);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
    
    /* This is needed only for objects with possible dummy type, as they are updated with TR_UPDATE */
    /* We use this tag when committing the update to set dummy==0 */
    /* XXX may be later this should be reconsidered */
    g_string_sprintf(tr->query, "UPDATE %s SET thread_id=0 WHERE  object_id=%ld AND thread_id=%d", 
                             DF_get_class_sql_table(tr->class_type), tr->object_id, tr->thread_upd);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

/* Now tables  that might be affected by dummies */
    for(j=0; j < tr->ndummy; j++) 
    for (i=0; tables[tr->class_type][i] != NULL; i++) {
    	g_string_sprintf(tr->query, "DELETE FROM %s WHERE object_id=%ld ", tables[tr->class_type][i], tr->dummy_id[j]);
    	sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
    } 

  /* if dummies have been created - get rid of them */
  for(j=0; j < tr->ndummy; j++){
	 g_string_sprintf(tr->query, "DELETE FROM last WHERE object_id=%ld ", tr->dummy_id[j]);
	 sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
  }
  
/* Rollback last and history tables */

    /* Delete what has been inserted */
    g_string_sprintf(tr->query, "DELETE FROM history WHERE object_id=%ld AND thread_id=%d", tr->object_id, tr->thread_ins);
    sql_err=SQ_execute_query(tr->sql_connection, tr->query->str, NULL);

    /* Normalize what has been updated/touched */
    g_string_sprintf(tr->query, "UPDATE history SET thread_id=0 WHERE object_id=%ld AND thread_id=%d", tr->object_id, tr->thread_upd);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

    /* Delete what has been inserted */
    g_string_sprintf(tr->query, "DELETE FROM last WHERE object_id=%ld AND thread_id=%d", tr->object_id, tr->thread_ins);
    sql_err=SQ_execute_query(tr->sql_connection, tr->query->str, NULL);

    /* Normalize what has been updated/touched */
    g_string_sprintf(tr->query, "UPDATE last SET thread_id=0 WHERE object_id=%ld AND thread_id=%d", tr->object_id, tr->thread_upd);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

  /* If there is transaction support in database, just "COMMIT". Else, UNLOCK TABLES */
  if(transaction_support == 1){
    /* commit changes */
    g_string_sprintf(tr->query, "COMMIT ");
  }else{
    /* Unlock all tables */
    g_string_sprintf(tr->query, "UNLOCK TABLES ");
  }
  sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

  if (sql_err) {
	LG_log(ud_context, LG_ERROR, "%s[%s]", SQ_error(tr->sql_connection), tr->query->str);
        tr->succeeded=0;
        tr->error |= ERROR_U_DBS;
	die;
  }


  return(0);
} /* rollback() */

/************************************************************
* int UD_commit_I()                                         *
*                                                           *
* Performs I phase of the commit - deletions                *
*                                                           *
* General approach is to delete untouched rec (thread_id==0)*
*                                                           *
************************************************************/

int UD_commit_I(Transaction_t *tr) {
int err=0;
int i;
int sql_err;



/* Commit the transaction for AUX and LEAF tables that may be affected (taken from object template) */
  for (i=TAB_START; tables[tr->class_type][i] != NULL; i++) {
 /* Delete old records from the tables */  
    g_string_sprintf(tr->query, "DELETE FROM %s WHERE object_id=%ld AND thread_id=0 ", tables[tr->class_type][i], tr->object_id);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
  }

 /* Delete old record from the last table */  
    g_string_sprintf(tr->query, "DELETE FROM last WHERE object_id=%ld AND thread_id=0 ", tr->object_id);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

  
 return(err); 	
}

/************************************************************
* int UD_commit_II()                                        *
*                                                           *
* Performs I phase of the commit - deletions                *
* General approach is to clean up all new and updated       *
* records related to the transaction                        *
* (thread_id==thread_ins) and (thread_id==thread_upd)       *
*                                                           *
************************************************************/
int UD_commit_II(Transaction_t *tr) {
int err=0;
int i,j;
A_Type_t attr_type;
int sql_err;

 
/* Commit the transaction for AUX and LEAF tables that may be affected (taken from object template) */
  for (i=TAB_START; tables[tr->class_type][i] != NULL; i++) {
 /* Set thread_id to 0 to commit the transaction */    
    g_string_sprintf(tr->query, "UPDATE %s SET thread_id=0 WHERE object_id=%ld", tables[tr->class_type][i], tr->object_id);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
  }
  
/* Commit changes to the last table */  
   g_string_sprintf(tr->query, "UPDATE last SET thread_id=0 WHERE object_id=%ld ", tr->object_id);
   sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

/* Commit changes to the history table */  
   g_string_sprintf(tr->query, "UPDATE history SET thread_id=0 WHERE object_id=%ld ", tr->object_id);
   sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
   
/* Commit the transaction for the MAIN tables */

/* Commit the transaction for person_role, mntner, as_set, route_set tables */
/* They require different handling because of dummies */
/* The rule is: Update: dummy->0, Insert: preserve dummy value */
/* These tables do not require deletions since we cannot have such condition (object_id==0 AND thread_id==0) */
 if((tr->class_type==C_PN) || (tr->class_type==C_RO) || 
   (tr->class_type==C_AS)  || (tr->class_type==C_RS) ||
   (tr->class_type==C_MT)  || (tr->class_type==C_IT)){

 /* Process the rows updated/touched */
    g_string_sprintf(tr->query, "UPDATE %s SET thread_id=0, dummy=0 WHERE object_id=%ld AND thread_id=%d ",  DF_get_class_sql_table(tr->class_type), tr->object_id, tr->thread_upd);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
 }
 
 switch (tr->class_type) {
   case C_IR:
   case C_IN:
   case C_I6:
/*   case C_FS: */ /* filter_set table deos not have 'filter' column anymore. */
/*    if((tr->save)){ */
    /* Some special processing for tables with the second attribute */
     /* Update the second field of the table with query like one below */
     /* UPDATE %s SET thread_id=%d, local_as='%s' WHERE object_id=%ld */

     /* XXX if second attribute is missing - make it empty */
     if(tr->save==NULL) tr->save=g_strdup("");
     switch(tr->class_type) {
      /* Local-as for inet-rtr */
      case C_IR: attr_type=A_LA;
                 break;
      /* netname for inetnum and inet6num */           
      case C_IN: 
      case C_I6: attr_type=A_NA;
                 break;
      /* filter for filter-set */           
/*      case C_FS: attr_type=A_FI;
                 break; */ /* filter_set table deos not have 'filter' column anymore. */
      default:
                 LG_log(ud_context, LG_SEVERE, "not valid class type\n");
                 attr_type=A_END;
                 die;
     }
     g_string_sprintf(tr->query, DF_get_update_query(attr_type), DF_get_class_sql_table(tr->class_type), 0, (char *)tr->save, tr->object_id);
     sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
/*    }
    else {
     ER_perror(FAC_UD, UD_BUG, "second attribute is not saved\n");
     die;
    }
*/    
    break;
   
   default:  
 /* Process all other MAIN tables for updates/inserts and person_role, mntner, as_set, route_set tables for rows inserts */
    g_string_sprintf(tr->query, "UPDATE %s SET thread_id=0 WHERE object_id=%ld AND thread_id>0", DF_get_class_sql_table(tr->class_type), tr->object_id);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
    break;
 }  


/* for tables that might be affected by dummies */
 for(j=0; j < tr->ndummy; j++)/* if dummies have been created */
   for (i=0; tables[tr->class_type][i] != NULL; i++) {
    g_string_sprintf(tr->query, "UPDATE %s SET thread_id=0 WHERE object_id=%ld ", tables[tr->class_type][i], tr->dummy_id[j]);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
 }


   for(j=0; j < tr->ndummy; j++){/* if dummies have been created*/
	 g_string_sprintf(tr->query, "UPDATE last SET thread_id=0 WHERE object_id=%ld ", tr->dummy_id[j]);
	 sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
  }
 
 return(err);		
}


/************************************************************
* int UD_commit()                                           *
*                                                           *
* Commits the transaction                                   *
*                                                           *
* It locks all relevant tables and processes the 2 phases of*
* commit. It also performs checkpointing of phases and      * 
* radix tree update                                         * 
*                                                           * 
* We need to split commit into 2 because otherwise it is    *
* hard to distinguish between commited records and untouched*
* ones (both have thread_id==0). Splitting and checkpointing*
* solves this problem                                       *
*                                                           *
************************************************************/

int UD_commit(Transaction_t *tr) {
int err=0;
int sql_err;
int transaction_support;

char *common_tables[] = {
   "last", "history", "transaction_rec", NULL
};

if(ACT_DELETE(tr->action)) return(0);

 /* check if there's transaction support */	
 transaction_support = ud_transaction_support(tr);


 /* if our database's table type supports transactions, 
    then we'll use "BEGIN - COMMIT". 
    If not, we have to lock all relevant tables */
 if(transaction_support == 1)
   g_string_sprintf(tr->query, "BEGIN ");

 else
   ud_build_lock_query(tr, common_tables);
    
  sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
  if (sql_err) {
    LG_log(ud_context, LG_ERROR, "%s[%s]", SQ_error(tr->sql_connection), tr->query->str);
    tr->succeeded=0;
    tr->error |=ERROR_U_DBS;
    die;
  }

  /* Perform first phase - deletions */
  UD_commit_I(tr);
  /* checkpoint this step */
  CP_COMMIT_I_PASSED(tr->action); TR_update_status(tr);
  /* Perform first phase - updates */
  UD_commit_II(tr);
  /* checkpoint this step */
  CP_COMMIT_II_PASSED(tr->action); TR_update_status(tr);

  /* If there is transaction support in database, just "COMMIT". Else, UNLOCK TABLES */
  if(transaction_support == 1){
    /* commit changes */
    g_string_sprintf(tr->query, "COMMIT ");
  }else{
    /* Unlock all tables */
    g_string_sprintf(tr->query, "UNLOCK TABLES ");
  }
  sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

  if (sql_err) {
	LG_log(ud_context, LG_ERROR, "%s[%s]", SQ_error(tr->sql_connection), tr->query->str);
        tr->succeeded=0;
        tr->error |= ERROR_U_DBS;
	die;
  }


  /* Update radix tree for route, inetnum and inaddr-arpa domain*/
  err = UD_update_rx(tr, RX_OPER_CRE);
 
  return(err);
} /* commit() */

/************************************************************
* int UD_check_ref()                                        *
*                                                           *
* Checks if the object to be deleted is referenced from     *
* anywhere                                                  *
*                                                           *
* 0 - go ahead                                              *
* -1 - deletion will compromise ref.integrity               *
* Result is also reflected in tr->succeeded                 *
************************************************************/
int UD_check_ref(Transaction_t *tr) 
{
int i;
long ref_id;
long num_rec;

char sobject_id[STR_M];
char *sql_str;


/* Check for referential integrity of deletion */

   sprintf(sobject_id, "%ld", tr->object_id);

   switch(tr->class_type){
    case C_PN:
    case C_RO:
        
       /* Check that this person/role object is not referenced */
        
       for (i=0; t_ipn[i] != NULL; i++) { 
        /* Calculate number of references */
        sql_str= get_field_str(tr->sql_connection, "COUNT(*)", t_ipn[i], "pe_ro_id", sobject_id, NULL);
        if(sql_str) {
         num_rec = atol(sql_str);  UT_free(sql_str);
         ref_id=tr->object_id;
         /* Check if it is a self reference (for role objects) */
         if(num_rec==1) {
          sql_str= get_field_str(tr->sql_connection, "object_id", t_ipn[i], "pe_ro_id", sobject_id, NULL);
          if(sql_str) {
           ref_id = atol(sql_str);  UT_free(sql_str);
          } else {
	   /* this is not possible unless it is an sql error */
	   /*XXX probably we need to die */	  
           tr->succeeded=0; tr->error |= ERROR_U_DBS; break;
          }
         }
         /* If there are references (and not the only self reference) we cannot delete */
         if((num_rec>1) || (ref_id!=tr->object_id)) {
           g_string_sprintfa(tr->error_script,"E[%d][%ld]:ref integrity: %s\n" ,ERROR_U_OBJ, num_rec, t_ipn[i]);
           tr->succeeded=0; tr->error |= ERROR_U_OBJ;
         }
        } else {
        /* SQL error occured */
         tr->succeeded=0; tr->error |= ERROR_U_DBS;
         g_string_sprintfa(tr->error_script,"E[%d][%s]:%s\n", ERROR_U_DBS, t_ipn[i], SQ_error(tr->sql_connection));
        }
       }
       
       /* Check that this person/role object is not referenced by name (legacy stuff) */
       /* But allow overriding this check in NRTM mode and with override_integrity    */
       if(IS_DUMMY_ALLOWED(tr->mode))break;
       else { /*XXX legacy stuff */
      
        /* get the splitted words of the name (done in ud_split_names()) and compose the full name */
        /* then compare this reconstructed value with the stored legacy "nic-handle" */
        GList *person, *p;
        GString *reconstructed_value;

        if (tr->class_type == C_PN) 
           person = rpsl_object_get_attr(tr->object, "person");
        else
           person = rpsl_object_get_attr(tr->object, "role");
           
        reconstructed_value = g_string_new(rpsl_attr_get_value(person->data));

        for (p = g_list_next(person); p!=NULL; p = g_list_next(p)) {
          g_string_sprintfa(reconstructed_value, " %s", rpsl_attr_get_value(p->data));
        }
        rpsl_attr_delete_list(person);
        
        for (i=0; t_ipn[i] != NULL; i++) { 
        /* Calculate number of references */
        
         g_string_sprintf(tr->query, "SELECT COUNT(*) FROM %s, person_role "
                                "WHERE person_role.object_id=%s.pe_ro_id "
                                "AND person_role.nic_hdl='%s' ", t_ipn[i], t_ipn[i], reconstructed_value->str);
        

         sql_str= get_qresult_str(tr->sql_connection, tr->query->str);

         if(sql_str) {
          num_rec = atol(sql_str);  UT_free(sql_str);
          /* If there are references (no self reference is possible in this case) we cannot delete */
          if(num_rec>0) {
           g_string_sprintfa(tr->error_script,"E[%d][%ld]:ref integrity: %s\n" ,ERROR_U_OBJ, num_rec, t_ipn[i]);
           tr->succeeded=0; tr->error |= ERROR_U_OBJ;
          }
         } else {
         /* SQL error occured */
          tr->succeeded=0; tr->error |= ERROR_U_DBS;
          g_string_sprintfa(tr->error_script,"E[%d][%s]:%s\n", ERROR_U_DBS, t_ipn[i], SQ_error(tr->sql_connection));
         }
        }
        g_string_free(reconstructed_value, TRUE);

       }   
       break;
        
    case C_MT:
    
        /* Check that this mntner object is not referenced */
        
       for (i=0; t_imt[i] != NULL; i++) { 
       /* Calculate number of references */
        sql_str= get_field_str(tr->sql_connection, "COUNT(*)", t_imt[i], "mnt_id", sobject_id, NULL);
        if(sql_str) {
         num_rec = atol(sql_str);  UT_free(sql_str);
         ref_id=tr->object_id;
         /* Check if it is a self reference  */
         if(num_rec==1) { 
            sql_str= get_field_str(tr->sql_connection, "object_id", t_imt[i], "mnt_id", sobject_id, NULL);
            if(sql_str) {
              ref_id = atol(sql_str);  UT_free(sql_str);
            } else {
              tr->succeeded=0; tr->error |= ERROR_U_DBS; break;
            } 
         }
         /* If there are references (and not the only self reference) we cannot delete */ 
         if((num_rec>1) || (ref_id!=tr->object_id)) {
           g_string_sprintfa(tr->error_script,"E[%d][%ld]:ref integrity: %s\n" ,ERROR_U_OBJ, num_rec, t_imt[i]);
           tr->succeeded=0; tr->error |= ERROR_U_OBJ;
         }
        } else {
         tr->succeeded=0; tr->error |= ERROR_U_DBS;
        }
       }   
       break;

    case C_IT:
        /* Check that this irt object is not referenced */
        
       for (i=0; t_iit[i] != NULL; i++) { 
       /* Calculate number of references */
        sql_str= get_field_str(tr->sql_connection, "COUNT(*)", t_iit[i], "irt_id", sobject_id, NULL);
        if(sql_str) {
         num_rec = atol(sql_str);  UT_free(sql_str);
         ref_id=tr->object_id;
         /* Check if it is a self reference  */
         /* IRT object cannot have self references */
         
         /* If there are references (and not the only self reference) we cannot delete */ 
         if(num_rec>0) {
           g_string_sprintfa(tr->error_script,"E[%d][%ld]:ref integrity: %s\n" ,ERROR_U_OBJ, num_rec, t_iit[i]);
           tr->succeeded=0; tr->error |= ERROR_U_OBJ;
         }
        } else {
         tr->succeeded=0; tr->error |= ERROR_U_DBS;
        }
       }   
       break;
        
    case C_RS:
    case C_AS:
    case C_IS:
        /* Check that this set object is not referenced */
        /* Calculate number of references */
        sql_str= get_field_str(tr->sql_connection, "COUNT(*)", "member_of", "set_id", sobject_id, NULL);
        if(sql_str) {
         num_rec = atol(sql_str);  UT_free(sql_str);
         /* XXX though set may contain other sets as memebers, */
         /* there is no member-of attribute in these objects. */
         /* So no self-reference is possible */
         if(num_rec!=0) {
           /* Throw ref integ error whenever we still have references */
           g_string_sprintfa(tr->error_script,"E[%d][%ld]:ref integrity: %s\n" ,ERROR_U_OBJ, num_rec, "member_of");
           tr->succeeded=0; tr->error |= ERROR_U_OBJ;
         }
        } else {
         tr->succeeded=0; tr->error |= ERROR_U_DBS;
        }
        break;
    case C_OA:
    
        /* Check that this organisaition object is not referenced */
        
       for (i=0; t_ioa[i] != NULL; i++) { 
       /* Calculate number of references */
        sql_str= get_field_str(tr->sql_connection, "COUNT(*)", t_ioa[i], "org_id", sobject_id, NULL);
        if(sql_str) {
         num_rec = atol(sql_str);  UT_free(sql_str);
         ref_id=tr->object_id;
         /* Check if it is a self reference  */
         if(num_rec==1) { 
            sql_str= get_field_str(tr->sql_connection, "object_id", t_ioa[i], "org_id", sobject_id, NULL);
            if(sql_str) {
              ref_id = atol(sql_str);  UT_free(sql_str);
            } else {
              tr->succeeded=0; tr->error |= ERROR_U_DBS; break;
            } 
         }
         /* If there are references (and not the only self reference) we cannot delete */ 
         if((num_rec>1) || (ref_id!=tr->object_id)) {
           g_string_sprintfa(tr->error_script,"E[%d][%ld]:ref integrity: %s\n" ,ERROR_U_OBJ, num_rec, t_ioa[i]);
           tr->succeeded=0; tr->error |= ERROR_U_OBJ;
         }
        } else {
         tr->succeeded=0; tr->error |= ERROR_U_DBS;
        }
       }   
       break;

    case C_PF:

        /* Check that this poetic-form object is not referenced */

       for (i=0; t_ipf[i] != NULL; i++) {
       /* Calculate number of references */
        sql_str= get_field_str(tr->sql_connection, "COUNT(*)", t_ipf[i], "form_id", sobject_id, NULL);
        if(sql_str) {
         num_rec = atol(sql_str);  UT_free(sql_str);
         /* If there are references (and not the only self reference) we * cannot delete */
         if(num_rec>0) {
           g_string_sprintfa(tr->error_script,"E[%d][%ld]:ref integrity: %s\n" ,ERROR_U_OBJ, num_rec, t_ipf[i]);
           tr->succeeded=0; tr->error |= ERROR_U_OBJ;
         }
        } else {
         tr->succeeded=0; tr->error |= ERROR_U_DBS;
        }
       }
       break;

    default:
        break;    
   } 
   
 /* Check if we have passed referential integrity check */  
 if(tr->succeeded) return(0); else return(-1);
 
} 
	
/************************************************************
* int UD_delete()                                              *
*                                                           *
* Deletes the object                                        *
*                                                           *
* It deletes the object from all relevant tables. 
* Then it updates the radix tree for routes, inetnums 
* and rev.domains           *
*                                                           *
************************************************************/
int UD_delete(Transaction_t *tr) 
{
int err=0;
int i;
long timestamp;
int sql_err;
int ref_set;
int transaction_support;

/* tables we'll have to LOCK regardless of object class */
char *common_tables[] = { "last", "history", NULL };

 /* check if there's transaction support */	
 transaction_support = ud_transaction_support(tr);


/* if we are deleting referenced set, we need to  perform delete a bit differently */
/* no deletions of aux tables */
/* dummy main, instead of del */
/* dummy last instead of empty */
/* So let's determine if we are deleting referenced set */
if ((tr->class_type==C_AS || tr->class_type==C_RS) && ACT_UPD_DUMMY(tr->action)) ref_set = 1; else ref_set = 0;

    /* if our database's table type supports transactions, then we'll use "BEGIN - COMMIT". If not, we have to
       lock all relevant tables */

    if(transaction_support == 1)
        g_string_sprintf(tr->query, "BEGIN ");

    else 
        ud_build_lock_query(tr, common_tables);

    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
    if (sql_err) {
         LG_log(ud_context, LG_ERROR, "%s[%s]", SQ_error(tr->sql_connection), tr->query->str);
         tr->succeeded=0;
         tr->error |=ERROR_U_DBS;
    	 die;
    }

/* Update the history table */
/* XXX Crash recovery: */
/* If history was not updated - we will create a record */
/* If history was already updated but last wasn't - we will just replace the record */
/* If history and last were already updated - we will have an empty query - 0 rows should be affected */
    g_string_sprintf(tr->query, "REPLACE history "
				"SELECT 0, object_id, sequence_id, timestamp, object_type, object, pkey, serial, prev_serial "
       				"FROM last "
       				"WHERE object_id=%ld AND sequence_id=%ld ", tr->object_id, tr->sequence_id);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
    if (sql_err) {
         LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
         tr->succeeded=0;
         tr->error |=ERROR_U_DBS;
	 die;
    }

/* Delete records from the leaf and aux tables */
    for (i=TAB_START; tables[tr->class_type][i] != NULL; i++) {
     g_string_sprintf(tr->query, "DELETE FROM %s WHERE object_id=%ld ", tables[tr->class_type][i], tr->object_id);
     sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
       if (sql_err) {
         LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
         tr->succeeded=0;
         tr->error |=ERROR_U_DBS;
	 die;
       }
    }  
     

 /* For all object except as-sets and route-sets we need to empty MAIN table */
 /* For referenced sets, however, we transform them to dummy, not delete */
 if (ref_set == 0) {

/* Process the MAIN table  */
    g_string_sprintf(tr->query, "DELETE FROM %s WHERE object_id=%ld ", DF_get_class_sql_table(tr->class_type), tr->object_id);
   

    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
    if (sql_err) {
         LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
         tr->succeeded=0;
         tr->error |=ERROR_U_DBS;
	 die;
    }
 
 } else { /* this is the referenced set */
 /* we need to 'dummy' MAIN */
    g_string_sprintf(tr->query, "UPDATE %s SET dummy=1 WHERE object_id=%ld ", DF_get_class_sql_table(tr->class_type), tr->object_id);
               
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
    if (sql_err) {
         LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
         tr->succeeded=0;
         tr->error |= ERROR_U_DBS;
         die;
    }
 }
       
  /* insert new version into the last */
  timestamp=time(NULL);
  
 if(ref_set == 0) 
 {
 /* empty the contents, but leave in the table to restrict re-use of object_id */ 
 /* XXX change sequence_id=0 so it is easy to say that the object was deleted */
  g_string_sprintf(tr->query, "UPDATE last SET object='', timestamp=%ld, sequence_id=0  WHERE object_id=%ld ", timestamp, tr->object_id);

  sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
  if (sql_err) {
         LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
         tr->succeeded=0;
         tr->error |= ERROR_U_DBS;
	 die;
  }
 } else {/* this is the referenced set */
 /* 'dummy' the contents, but leave in the table to prevent re-use of object_id */ 
 g_string_sprintf(tr->query, "UPDATE last SET object='DUMMY SET', object_type=%d, sequence_id=%ld, timestamp=%ld  WHERE object_id=%ld ", DUMMY_TYPE, tr->sequence_id+1, timestamp, tr->object_id);

 sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
 if (sql_err) {
     LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
     tr->succeeded=0;
     tr->error |= ERROR_U_DBS;
    die;
   }
 }


  /* If there is transaction support in database, just "COMMIT". Else, UNLOCK TABLES */
  if(transaction_support == 1){
    /* commit changes */
    g_string_sprintf(tr->query, "COMMIT ");
  }else{
   /* Unlock all tables */
   g_string_sprintf(tr->query, "UNLOCK TABLES ");
  }
  
  sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
  if (sql_err) {
        LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
        tr->succeeded=0;
        tr->error |= ERROR_U_DBS;
	die;
  }

  return(err);

} /* delete() */ 



     	       /* Do more in the forest
   * Update radix tree for route and inetnum
   */

int UD_update_rx(Transaction_t *tr, rx_oper_mt mode)
{
rp_upd_pack_t *packptr = tr->packptr;
int err=0;

 
  if(!IS_STANDALONE(tr->mode)) { /* only if server */
  

    /* Only for these types of objects and only if we have collected data (tr->save != NULL) */
    if( (   (tr->class_type==C_RT) 
   || (tr->class_type==C_R6)
	 || (tr->class_type==C_IN) 
	 || (tr->class_type==C_I6)
	 || (tr->class_type==C_DN))) {
      /* Collect some data for radix tree and NH repository update for deletes*/
      if(mode == RX_OPER_DEL) {
        g_list_foreach((GList *)rpsl_object_get_all_attr(tr->object), get_rx_data, tr);
      }
     
      /* Except for regular domains we need to update radix tree */
      if(ACT_UPD_RX(tr->action)){
       packptr->key = tr->object_id;
       if( RP_pack_node(mode, packptr, tr->source_hdl) == RX_OK ) {
	err = 0;
       } else {
	err = (-1);
        LG_log(ud_context, LG_SEVERE, "cannot update radix tree\n");
	die;
       }
      } /* update radix tree */
    }
  }
  return(err);
}
   
	       

