#ifndef BISON_MP_DEFAULT_TAB_H
# define BISON_MP_DEFAULT_TAB_H

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
# define	TKN_IPV4	273
# define	TKN_RTRSNAME	274
# define	TKN_PRNGNAME	275
# define	TKN_IPV6	276
# define	TKN_IPV6DC	277
# define	TKN_PRFXV6	278
# define	TKN_PRFXV6DC	279
# define	TKN_PRFXV6RNG	280
# define	TKN_PRFXV6DCRNG	281
# define	KEYW_TO	282
# define	KEYW_ACTION	283
# define	KEYW_NETWORKS	284
# define	KEYW_EXCEPT	285
# define	KEYW_AFI	286
# define	KEYW_AFI_VALUE_V4	287
# define	KEYW_AFI_VALUE_V6	288
# define	KEYW_AFI_VALUE_ANY	289
# define	TKN_PREF	290
# define	TKN_MED	291
# define	TKN_DPA	292
# define	TKN_ASPATH	293
# define	TKN_COMMUNITY	294
# define	TKN_NEXT_HOP	295
# define	TKN_COST	296
# define	TKN_COMM_NO	297
# define	KEYW_IGP_COST	298
# define	KEYW_SELF	299
# define	KEYW_PREPEND	300
# define	KEYW_APPEND	301
# define	KEYW_DELETE	302
# define	KEYW_CONTAINS	303
# define	KEYW_AT	304
# define	KEYW_INTERNET	305
# define	KEYW_NO_EXPORT	306
# define	KEYW_NO_ADVERTISE	307
# define	TKN_INT	308
# define	TKN_DNS	309


extern YYSTYPE mp_defaultlval;

#endif /* not BISON_MP_DEFAULT_TAB_H */
