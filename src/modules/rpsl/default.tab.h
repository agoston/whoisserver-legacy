#ifndef BISON_DEFAULT_TAB_H
# define BISON_DEFAULT_TAB_H

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
# define	KEYW_TO	277
# define	KEYW_ACTION	278
# define	KEYW_NETWORKS	279
# define	KEYW_EXCEPT	280
# define	TKN_PREF	281
# define	TKN_MED	282
# define	TKN_DPA	283
# define	TKN_ASPATH	284
# define	TKN_COMMUNITY	285
# define	TKN_NEXT_HOP	286
# define	TKN_COST	287
# define	TKN_COMM_NO	288
# define	KEYW_IGP_COST	289
# define	KEYW_SELF	290
# define	KEYW_PREPEND	291
# define	KEYW_APPEND	292
# define	KEYW_DELETE	293
# define	KEYW_CONTAINS	294
# define	KEYW_AT	295
# define	KEYW_INTERNET	296
# define	KEYW_NO_EXPORT	297
# define	KEYW_NO_ADVERTISE	298
# define	TKN_INT	299


extern YYSTYPE defaultlval;

#endif /* not BISON_DEFAULT_TAB_H */
