#ifndef BISON_INJECT_R6_TAB_H
# define BISON_INJECT_R6_TAB_H

#ifndef YYSTYPE
typedef union {
    char *sval;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	OP_OR	257
# define	OP_EQUAL	258
# define	OP_APPEND	259
# define	OP_COMPARE	260
# define	OP_AND	261
# define	TKN_ASNO	262
# define	TKN_IPV4	263
# define	TKN_IPV6	264
# define	TKN_IPV6DC	265
# define	KEYW_ACTION	266
# define	KEYW_AFI	267
# define	KEYW_AFI_VALUE	268
# define	TKN_PREF	269
# define	TKN_MED	270
# define	TKN_DPA	271
# define	TKN_ASPATH	272
# define	TKN_COMMUNITY	273
# define	TKN_NEXT_HOP	274
# define	TKN_COST	275
# define	TKN_COMM_NO	276
# define	TKN_RTRSNAME	277
# define	TKN_PRFXV4	278
# define	TKN_PRFXV4RNG	279
# define	TKN_PRFXV6	280
# define	TKN_PRFXV6DC	281
# define	TKN_PRFXV6RNG	282
# define	TKN_PRFXV6DCRNG	283
# define	KEYW_IGP_COST	284
# define	KEYW_SELF	285
# define	KEYW_PREPEND	286
# define	KEYW_APPEND	287
# define	KEYW_DELETE	288
# define	KEYW_CONTAINS	289
# define	KEYW_AT	290
# define	KEYW_EXCEPT	291
# define	KEYW_UPON	292
# define	KEYW_STATIC	293
# define	KEYW_HAVE_COMPONENTS	294
# define	KEYW_EXCLUDE	295
# define	KEYW_INTERNET	296
# define	KEYW_NO_EXPORT	297
# define	KEYW_NO_ADVERTISE	298
# define	KEYW_MASKLEN	299
# define	TKN_INT	300
# define	TKN_DNS	301


extern YYSTYPE inject_r6lval;

#endif /* not BISON_INJECT_R6_TAB_H */
