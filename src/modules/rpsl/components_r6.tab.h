#ifndef BISON_COMPONENTS_R6_TAB_H
# define BISON_COMPONENTS_R6_TAB_H

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
# define	ASPATH_POSTFIX	267
# define	TKN_FLTRNAME	268
# define	TKN_ASNO	269
# define	TKN_RSNAME	270
# define	TKN_ASNAME	271
# define	TKN_PRFXV4	272
# define	TKN_PRFXV4RNG	273
# define	TKN_PRFXV6	274
# define	TKN_PRFXV6DC	275
# define	TKN_PRFXV6RNG	276
# define	TKN_PRFXV6DCRNG	277
# define	KEYW_ATOMIC	278
# define	KEYW_PROTOCOL	279
# define	TKN_PROTOCOL	280
# define	KEYW_IGP_COST	281
# define	KEYW_SELF	282
# define	KEYW_PREPEND	283
# define	KEYW_APPEND	284
# define	KEYW_DELETE	285
# define	KEYW_CONTAINS	286
# define	KEYW_INTERNET	287
# define	KEYW_NO_EXPORT	288
# define	KEYW_NO_ADVERTISE	289
# define	TKN_PREF	290
# define	TKN_MED	291
# define	TKN_DPA	292
# define	TKN_ASPATH	293
# define	TKN_COMMUNITY	294
# define	TKN_NEXT_HOP	295
# define	TKN_COST	296
# define	TKN_COMM_NO	297
# define	TKN_IPV4	298
# define	TKN_IPV6	299
# define	TKN_IPV6DC	300
# define	TKN_INT	301


extern YYSTYPE components_r6lval;

#endif /* not BISON_COMPONENTS_R6_TAB_H */
