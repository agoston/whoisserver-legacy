#ifndef BISON_V6_FILTER_TAB_H
# define BISON_V6_FILTER_TAB_H

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
# define	OP_MS_V6	261
# define	OP_EQUAL	262
# define	OP_APPEND	263
# define	OP_COMPARE	264
# define	KEYW_ANY	265
# define	KEYW_PEERAS	266
# define	KEYW_IGP_COST	267
# define	KEYW_PREPEND	268
# define	KEYW_APPEND	269
# define	KEYW_DELETE	270
# define	KEYW_CONTAINS	271
# define	KEYW_INTERNET	272
# define	KEYW_NO_EXPORT	273
# define	KEYW_NO_ADVERTISE	274
# define	KEYW_SELF	275
# define	ASPATH_POSTFIX	276
# define	TKN_FLTRNAME	277
# define	TKN_ASNO	278
# define	TKN_RSNAME	279
# define	TKN_ASNAME	280
# define	TKN_PRFXV4	281
# define	TKN_PRFXV4RNG	282
# define	TKN_PRFXV6	283
# define	TKN_PRFXV6DC	284
# define	TKN_PRFXV6RNG	285
# define	TKN_PRFXV6DCRNG	286
# define	TKN_PREF	287
# define	TKN_MED	288
# define	TKN_DPA	289
# define	TKN_ASPATH	290
# define	TKN_COMMUNITY	291
# define	TKN_COMM_NO	292
# define	TKN_NEXT_HOP	293
# define	TKN_IPV4	294
# define	TKN_IPV6	295
# define	TKN_IPV6DC	296
# define	TKN_COST	297
# define	TKN_INT	298


extern YYSTYPE v6_filterlval;

#endif /* not BISON_V6_FILTER_TAB_H */
