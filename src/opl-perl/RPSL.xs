#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <glib.h>
#include <syntax_api.h>

typedef rpsl_error_t Net__Whois__RIPE__RPSL__Error;

static int
not_here(char *s)
{
    croak("%s not implemented on this architecture", s);
    return -1;
}

static double
constant(char *name, int len, int arg)
{
    errno = EINVAL;
    return 0;
}

MODULE = Net::Whois::RIPE::RPSL			PACKAGE = Net::Whois::RIPE::RPSL PREFIX = rpsl_


double
constant(sv,arg)
    PREINIT:
	STRLEN		len;
    INPUT:
	SV *		sv
	char *		s = SvPV(sv, len);
	int		arg
    CODE:
	RETVAL = constant(s,len,arg);
    OUTPUT:
	RETVAL

Net::Whois::RIPE::RPSL::Error *
rpsl_error_alloc()
	CODE:
		rpsl_error_t * err;
		if (( err = (rpsl_error_t *) calloc ( 1, sizeof( rpsl_error_t )))
			== (rpsl_error_t *) NULL ) 
		{
			XSRETURN_UNDEF;
		}
		RETVAL = err;
    OUTPUT:
	RETVAL

rpsl_attr_t *
rpsl_attr_clean_copy(attr)
	rpsl_attr_t *	attr

rpsl_attr_t *
rpsl_attr_copy(attr)
	rpsl_attr_t *	attr

void
rpsl_attr_delete(attr)
	rpsl_attr_t *	attr

# used internally only
#void
#rpsl_attr_delete_list(attributes)
#	GList *	attributes

void
rpsl_attr_errors(attr)
	rpsl_attr_t *	attr
  PREINIT:
	const GList * errors; 
	const GList * item; 
	guint length = 0;
	SV* sv_err;
  PPCODE:

	errors = rpsl_attr_errors( attr );
	length = g_list_length( (GList *) errors );

	if (length > 0) 
	{
		EXTEND(SP,length);
		item = g_list_first( (GList *) errors );

		do {
			sv_err = sv_newmortal();
			sv_setref_pv(sv_err, "rpsl_error_tPtr", (void*)item->data);
			PUSHs( sv_err );
		} while (item = g_list_next( item ));
	}

SV *
rpsl_attr_get_clean_value(attr)
	rpsl_attr_t *	attr
	PREINIT:
		gchar * attr_text;
	CODE:
		attr_text = rpsl_attr_get_clean_value( attr );
		RETVAL = newSVpv( attr_text, 0 );
		free( attr_text );
	OUTPUT:
		RETVAL

const gchar *
rpsl_attr_get_name(attr)
	rpsl_attr_t *	attr

gint
rpsl_attr_get_ofs(attr)
	rpsl_attr_t *	attr

void
rpsl_attr_get_split_list(attr)
	rpsl_attr_t *	attr
  PREINIT:
	GList * attr_list;
	const GList * item; 
	SV* sv_attr;
	rpsl_attr_t * copy;
	guint length;
  PPCODE:

	attr_list = rpsl_attr_get_split_list( attr );
	if ( attr_list ) 
	{
		length = g_list_length( (GList *) attr_list );

		if (length > 0) 
		{
			EXTEND(SP,length);
	
			item = g_list_first( (GList *) attr_list );
	
			do {
				copy = rpsl_attr_copy( (rpsl_attr_t *) item->data );
				sv_attr = sv_newmortal();
				sv_setref_pv(sv_attr, "rpsl_attr_tPtr", (void*) copy);
				PUSHs( sv_attr );
			} while ( item = g_list_next( item ) );
		}
		rpsl_attr_delete_list( attr_list );
	}
	
const gchar *
rpsl_attr_get_value(attr)
	rpsl_attr_t *	attr

gboolean
rpsl_attr_has_error(attr, error_level)
	rpsl_attr_t *	attr
	int	error_level

rpsl_attr_t *
rpsl_attr_init(s, class = NO_INIT)
	gchar *	s
	gchar * class
	CODE:
		switch (items) {
		case 2:
			/* 
			 * this was a bitch, but this seems to work 
			 * I tried if ( ST(1) != &PL_sv_undef ) 
			 * but didn't catch undef. 
			 */
			if ( strlen(class) > 0 ) {
				RETVAL = rpsl_attr_init(s, class);
				break;
			}
			/* fall thru */
		case 1: 
			RETVAL = rpsl_attr_init(s, NULL);
			break;
		default:
			croak("rpsl_attr_init: bad arguments\n");
		}
	OUTPUT:
		RETVAL

gboolean
rpsl_attr_is_generated(object, attr)
	rpsl_object_t *	object
	gchar *	attr

gboolean
rpsl_attr_is_key(object, attr)
	rpsl_object_t *	object
	gchar *	attr

gboolean
rpsl_attr_is_lookup(object, attr)
	rpsl_object_t *	object
	gchar *	attr

gboolean
rpsl_attr_is_multivalued(object, attr)
	rpsl_object_t *	object
	gchar *	attr

gboolean
rpsl_attr_is_required(object, attr)
	rpsl_object_t *	object
	gchar *	attr

void
rpsl_attr_replace_value(attr, value)
	rpsl_attr_t *	attr
	const gchar *	value

gint
rpsl_get_attr_id(attr_name)
	gchar *	attr_name

gint
rpsl_get_class_id(class_name)
	gchar *	class_name

void
rpsl_load_dictionary(level)
	int	level

int
rpsl_object_add_attr(object, attr, ofs, error)
	rpsl_object_t *	object
	rpsl_attr_t *	attr
	gint	ofs
	Net::Whois::RIPE::RPSL::Error *	error

int
rpsl_object_append_attr(object, attr, error)
	rpsl_object_t *	object
	rpsl_attr_t *	attr
	Net::Whois::RIPE::RPSL::Error *	error

rpsl_object_t *
rpsl_object_copy(object)
	rpsl_object_t *	object

rpsl_object_t *
rpsl_object_copy_flattened(object)
	rpsl_object_t *	object

void
rpsl_object_delete(object)
	rpsl_object_t *	object

void
rpsl_object_errors(object)
	rpsl_object_t *	object
  PREINIT:
	const GList * errors; 
	const GList * item; 
	guint length = 0;
	SV* sv_err;
  PPCODE:

	errors = rpsl_object_errors( object );
	length = g_list_length( (GList *) errors );

	if (length > 0) 
	{
		EXTEND(SP,length);
		item = g_list_first( (GList *) errors );

		do {
			sv_err = sv_newmortal();
			sv_setref_pv(sv_err, "rpsl_error_tPtr", (void*)item->data);
			PUSHs( sv_err );
		} while (item = g_list_next( item ));
	}

void
rpsl_object_get_all_attr(object)
	rpsl_object_t *	object
  PREINIT:
	const GList * attributes;
	const GList * item; 
	SV* sv_attr;
	guint length;
  PPCODE:

	attributes = rpsl_object_get_all_attr( object );
	length = g_list_length( (GList *) attributes );

	if (length > 0) 
	{
		EXTEND(SP,length);

		item = g_list_first( (GList *) attributes );

		do {
			/* caller must NOT free these attributes */
			sv_attr = sv_newmortal();
			sv_setref_pv(sv_attr, "rpsl_attr_tPtr", (void*)item->data);
			PUSHs( sv_attr );
		} while ( item = g_list_next( item ) );
	}

void
rpsl_object_get_attr(object, name)
	rpsl_object_t *	object
	gchar *	name
  PREINIT:
	const GList * attributes;
	const GList * item; 
	SV* sv_attr;
	guint length;
  PPCODE:

	attributes = rpsl_object_get_attr( object, name );
	length = g_list_length( (GList *) attributes );

	if (length > 0) 
	{
		EXTEND(SP,length);

		item = g_list_first( (GList *) attributes );

		do {
			sv_attr = sv_newmortal();
			sv_setref_pv(sv_attr, "rpsl_attr_tPtr", (void*)item->data);
			PUSHs( sv_attr );
		} while ( item = g_list_next( item ) );
	}


#const rpsl_attr_t *

rpsl_attr_t *
rpsl_object_get_attr_by_ofs(object, ofs)
	rpsl_object_t *	object
	gint	ofs

const char *
rpsl_object_get_class(object)
	rpsl_object_t *	object

gint
rpsl_object_get_num_attr(object)
	rpsl_object_t *	object

SV *
rpsl_object_get_text(object, data_column)
	rpsl_object_t *	object
	guint data_column
	PREINIT:
		gchar * obj_text;
	CODE:
		obj_text = rpsl_object_get_text( object, data_column );

		if(obj_text != NULL){
			RETVAL = newSVpv( obj_text, 0 );
			free( obj_text );
		} else {
			RETVAL = &PL_sv_undef;
		}
	OUTPUT:
		RETVAL

void
rpsl_get_classes()
	INIT:
		const gchar* const *class_list = NULL;
		int i;
		const char* tmp;
		SV *sv_class;
 	PPCODE:
		class_list = rpsl_get_classes();

		if(class_list == NULL){
				croak("rpsl_get_classes() returned NULL.\n");
				XSRETURN_UNDEF;
		}

		for(i = 0; (tmp = class_list[i]) != NULL; i++){
			sv_class = sv_newmortal();
			sv_setpv(sv_class, tmp);
			XPUSHs(sv_class);
		}

gchar *
tmpl_name(THIS)
	rpsl_template_t * THIS
	CODE:
	RETVAL = THIS->name;
	OUTPUT:
	RETVAL

gchar *
tmpl_code(THIS)
	rpsl_template_t * THIS
	CODE:
	RETVAL = THIS->code;
	OUTPUT:
	RETVAL

gboolean
tmpl_is_required(THIS)
	rpsl_template_t * THIS
	CODE:
	/* code for tmpl_is_required() */

	RETVAL = THIS->is_required;
	OUTPUT:
	RETVAL
			
gboolean
tmpl_is_generated(THIS)
	rpsl_template_t * THIS
	CODE:
	/* code for tmpl_is_generated() */
	RETVAL = THIS->is_generated;
	OUTPUT:
	RETVAL

gboolean
tmpl_is_multivalued(THIS)
	rpsl_template_t * THIS
	CODE:
	/* code for tmpl_is_multivalued() */
	RETVAL = THIS->is_multivalued;
	OUTPUT:
	RETVAL

gboolean
tmpl_is_lookup(THIS)
	rpsl_template_t * THIS
	CODE:
	/* code for tmpl_is_lookup() */
	RETVAL = THIS->is_lookup;
	OUTPUT:
	RETVAL

gboolean
tmpl_is_inverse(THIS)
	rpsl_template_t * THIS
	CODE:
	/* code for tmpl_is_inverse() */
	RETVAL = THIS->is_inverse;
	OUTPUT:
	RETVAL
			
gboolean
tmpl_is_primary(THIS)
	rpsl_template_t * THIS
	CODE:
	/* code for tmpl_is_primary() */
	RETVAL = THIS->is_primary;
	OUTPUT:
	RETVAL
			
gboolean
tmpl_is_list(THIS)
	rpsl_template_t * THIS
	CODE:
	/* code for tmpl_is_list() */
	RETVAL = THIS->is_list;
	OUTPUT:
	RETVAL
			
gboolean
tmpl_is_ripe_list(THIS)
	rpsl_template_t * THIS
	CODE:
	/* code for tmpl_is_ripe_list() */
	RETVAL = THIS->is_ripe_list;
	OUTPUT:
		RETVAL
		

			



void
rpsl_get_raw_template(class)
	const gchar *class;
	INIT:
		const rpsl_template_t* const *attr_list;
		const rpsl_template_t* attr_tmp;
		int i;
		SV *attr;
	PPCODE:
		attr_list = rpsl_get_template(class);

		if(attr_list == NULL){
			XSRETURN_UNDEF;
		}

		for(i = 0; (attr_tmp = attr_list[i]) != NULL; i++){
			attr = sv_newmortal();
			sv_setref_pv(attr, "rpsl_template_tPtr", (void*)attr_tmp);
			XPUSHs(attr);
		}


gboolean
rpsl_object_has_error(object, error_level)
	rpsl_object_t *	object
	int	error_level

rpsl_object_t *
rpsl_object_init(s)
	gchar *	s

gboolean
rpsl_object_is_deleted(object)
	rpsl_object_t *	object

rpsl_attr_t *
rpsl_object_remove_attr(object, ofs, error)
	rpsl_object_t *	object
	gint	ofs
	Net::Whois::RIPE::RPSL::Error *	error

rpsl_attr_t *
rpsl_object_remove_attr_name(object, name, error)
	rpsl_object_t *	object
	gchar *	name
	Net::Whois::RIPE::RPSL::Error *	error

int
rpsl_read_dictionary()

BOOT:
	rpsl_load_dictionary(RPSL_DICT_FRONT_END);


MODULE = Net::Whois::RIPE::RPSL             PACKAGE = rpsl_object_tPtr

void
DESTROY(THIS)
        rpsl_object_t * THIS
    CODE:
  		rpsl_object_delete(THIS);

MODULE = Net::Whois::RIPE::RPSL             PACKAGE = rpsl_template_tPtr


MODULE = Net::Whois::RIPE::RPSL             PACKAGE = rpsl_error_tPtr

gint
level(THIS)
	rpsl_error_t * THIS
	CODE:
	/* level of the error (enum above) */
	RETVAL = THIS->level;
    OUTPUT:
	RETVAL

gint
code(THIS)
	rpsl_error_t * THIS
	CODE:
    /* code for the error */
	RETVAL = THIS->code;
    OUTPUT:
	RETVAL

gchar *
descr(THIS)
	rpsl_error_t * THIS
    PROTOTYPE: $
    CODE:
	/* description of the error */
	RETVAL = THIS->descr;
    OUTPUT:
	RETVAL

gint
attr_num(THIS)
	rpsl_error_t * THIS
	CODE:
    /* offset of attribute with this error, or  -1 if none */
	RETVAL = THIS->attr_num;
	OUTPUT:
	RETVAL

MODULE = Net::Whois::RIPE::RPSL             PACKAGE = Net::Whois::RIPE::RPSL::Error

#  Net::Whois::RIPE::RPSL::Error is for errors that RPSL manages the memory for
#  The main difference is that this package takes care of
#  freeing the memory as opposed to rpsl_error_tPtr above.

Net::Whois::RIPE::RPSL::Error *
new(class, __err = NO_INIT)
	char * class
	rpsl_error_t * __err
	PREINIT:
		Net__Whois__RIPE__RPSL__Error * err;
	CODE:
		if (( err = (Net__Whois__RIPE__RPSL__Error *) calloc ( 1, sizeof( Net__Whois__RIPE__RPSL__Error )))
			== (Net__Whois__RIPE__RPSL__Error *) NULL ) {
			croak("error in allocating Net::Whois::RIPE::RPSL::Error\n");
		}
		if (items > 1) {
			err->level = __err->level;
			err->code = __err->code;
			err->attr_num = __err->attr_num;
			if ( ( err->descr = (char*)strdup(__err->descr) ) == (char*)NULL )
				croak("error in allocating descr\n");
		}
		RETVAL = err;
    OUTPUT:
	RETVAL

gint
level(THIS, __level = NO_INIT)
	Net::Whois::RIPE::RPSL::Error * THIS
	gint __level
    PROTOTYPE: $;$
	CODE:
	/* level of the error (enum above) */
	if (items > 1)
	    THIS->level = __level;
	RETVAL = THIS->level;
    OUTPUT:
	RETVAL

gint
code(THIS, __code = NO_INIT)
	Net::Whois::RIPE::RPSL::Error * THIS
	gint __code
    PROTOTYPE: $;$
	CODE:
    /* code for the error */
	if (items > 1)
	    THIS->code = __code;
	RETVAL = THIS->code;
    OUTPUT:
	RETVAL

gchar *
descr(THIS, __descr = NO_INIT)
	Net::Whois::RIPE::RPSL::Error * THIS
	char * __descr
    PROTOTYPE: $;$
    CODE:
	/* description of the error */
	if (items > 1)
		if ( ( THIS->descr = (char*)strdup(__descr) ) == (char*)NULL )
			croak("error in allocating descr\n");
	RETVAL = THIS->descr;
    OUTPUT:
	RETVAL

gint
attr_num(THIS, __attr_num = NO_INIT)
	Net::Whois::RIPE::RPSL::Error * THIS
	gint __attr_num
    PROTOTYPE: $;$
	CODE:
    /* offset of attribute with this error, or  -1 if none */
	if (items > 1)
	    THIS->attr_num = __attr_num;
	RETVAL = THIS->attr_num;
    OUTPUT:
	RETVAL

void
DESTROY(THIS)
        Net::Whois::RIPE::RPSL::Error * THIS
    CODE:
		if (THIS->descr)
			free(THIS->descr);
		free(THIS);

