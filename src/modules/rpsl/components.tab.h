#ifndef BISON_COMPONENTS_TAB_H
# define BISON_COMPONENTS_TAB_H

#ifndef YYSTYPE
typedef union {
    char *sval;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	OP_OR	257
# define	OP_AND	258
# define	OP_NOT	259
# define	OP_MS	260
# define	OP_EQUAL	261
# define	OP_APPEND	262
# define	OP_COMPARE	263
# define	KEYW_ANY	264
# define	KEYW_PEERAS	265
# define	ASPATH_POSTFIX	266
# define	TKN_FLTRNAME	267
# define	TKN_ASNO	268
# define	TKN_RSNAME	269
# define	TKN_ASNAME	270
# define	TKN_PRFXV4	271
# define	TKN_PRFXV4RNG	272
# define	KEYW_ATOMIC	273
# define	KEYW_PROTOCOL	274
# define	TKN_PROTOCOL	275
# define	KEYW_IGP_COST	276
# define	KEYW_SELF	277
# define	KEYW_PREPEND	278
# define	KEYW_APPEND	279
# define	KEYW_DELETE	280
# define	KEYW_CONTAINS	281
# define	KEYW_INTERNET	282
# define	KEYW_NO_EXPORT	283
# define	KEYW_NO_ADVERTISE	284
# define	TKN_PREF	285
# define	TKN_MED	286
# define	TKN_DPA	287
# define	TKN_ASPATH	288
# define	TKN_COMMUNITY	289
# define	TKN_NEXT_HOP	290
# define	TKN_COST	291
# define	TKN_COMM_NO	292
# define	TKN_IPV4	293
# define	TKN_INT	294


extern YYSTYPE componentslval;

#endif /* not BISON_COMPONENTS_TAB_H */
