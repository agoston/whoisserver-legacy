#ifndef BISON_INJECT_TAB_H
# define BISON_INJECT_TAB_H

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
# define	KEYW_ACTION	264
# define	TKN_PREF	265
# define	TKN_MED	266
# define	TKN_DPA	267
# define	TKN_ASPATH	268
# define	TKN_COMMUNITY	269
# define	TKN_NEXT_HOP	270
# define	TKN_COST	271
# define	TKN_COMM_NO	272
# define	TKN_RTRSNAME	273
# define	TKN_PRFXV4	274
# define	TKN_PRFXV4RNG	275
# define	KEYW_IGP_COST	276
# define	KEYW_SELF	277
# define	KEYW_PREPEND	278
# define	KEYW_APPEND	279
# define	KEYW_DELETE	280
# define	KEYW_CONTAINS	281
# define	KEYW_AT	282
# define	KEYW_EXCEPT	283
# define	KEYW_UPON	284
# define	KEYW_STATIC	285
# define	KEYW_HAVE_COMPONENTS	286
# define	KEYW_EXCLUDE	287
# define	KEYW_INTERNET	288
# define	KEYW_NO_EXPORT	289
# define	KEYW_NO_ADVERTISE	290
# define	KEYW_MASKLEN	291
# define	TKN_INT	292
# define	TKN_DNS	293


extern YYSTYPE injectlval;

#endif /* not BISON_INJECT_TAB_H */
