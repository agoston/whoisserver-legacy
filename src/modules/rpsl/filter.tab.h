#ifndef BISON_FILTER_TAB_H
# define BISON_FILTER_TAB_H

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
# define	KEYW_IGP_COST	266
# define	KEYW_PREPEND	267
# define	KEYW_APPEND	268
# define	KEYW_DELETE	269
# define	KEYW_CONTAINS	270
# define	KEYW_INTERNET	271
# define	KEYW_NO_EXPORT	272
# define	KEYW_NO_ADVERTISE	273
# define	KEYW_SELF	274
# define	ASPATH_POSTFIX	275
# define	TKN_FLTRNAME	276
# define	TKN_ASNO	277
# define	TKN_RSNAME	278
# define	TKN_ASNAME	279
# define	TKN_PRFXV4	280
# define	TKN_PRFXV4RNG	281
# define	TKN_PREF	282
# define	TKN_MED	283
# define	TKN_DPA	284
# define	TKN_ASPATH	285
# define	TKN_COMMUNITY	286
# define	TKN_COMM_NO	287
# define	TKN_NEXT_HOP	288
# define	TKN_IPV4	289
# define	TKN_COST	290
# define	TKN_INT	291


extern YYSTYPE filterlval;

#endif /* not BISON_FILTER_TAB_H */
