#ifndef BISON_MP_IMPORT_TAB_H
# define BISON_MP_IMPORT_TAB_H

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
# define	TKN_DNS	274
# define	TKN_RTRSNAME	275
# define	TKN_PRNGNAME	276
# define	TKN_IPV6	277
# define	TKN_IPV6DC	278
# define	TKN_PRFXV6	279
# define	TKN_PRFXV6DC	280
# define	TKN_PRFXV6RNG	281
# define	TKN_PRFXV6DCRNG	282
# define	KEYW_ACTION	283
# define	KEYW_EXCEPT	284
# define	KEYW_AFI	285
# define	KEYW_AFI_VALUE_V4	286
# define	KEYW_AFI_VALUE_V6	287
# define	KEYW_AFI_VALUE_ANY	288
# define	TKN_PREF	289
# define	TKN_MED	290
# define	TKN_DPA	291
# define	TKN_ASPATH	292
# define	TKN_COMMUNITY	293
# define	TKN_NEXT_HOP	294
# define	TKN_COST	295
# define	TKN_COMM_NO	296
# define	KEYW_IGP_COST	297
# define	KEYW_SELF	298
# define	KEYW_PREPEND	299
# define	KEYW_APPEND	300
# define	KEYW_DELETE	301
# define	KEYW_CONTAINS	302
# define	KEYW_AT	303
# define	KEYW_INTERNET	304
# define	KEYW_NO_EXPORT	305
# define	KEYW_NO_ADVERTISE	306
# define	KEYW_PROTOCOL	307
# define	TKN_PROTOCOL	308
# define	KEYW_INTO	309
# define	KEYW_REFINE	310
# define	KEYW_ACCEPT	311
# define	KEYW_FROM	312
# define	TKN_INT	313


extern YYSTYPE mp_importlval;

#endif /* not BISON_MP_IMPORT_TAB_H */
