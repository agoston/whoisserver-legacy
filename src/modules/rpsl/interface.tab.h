#ifndef BISON_INTERFACE_TAB_H
# define BISON_INTERFACE_TAB_H

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
# define	TKN_ASNO	261
# define	TKN_IPV4	262
# define	TKN_IPV6	263
# define	TKN_IPV6DC	264
# define	KEYW_ACTION	265
# define	TKN_PREF	266
# define	TKN_MED	267
# define	TKN_DPA	268
# define	TKN_ASPATH	269
# define	TKN_COMMUNITY	270
# define	TKN_NEXT_HOP	271
# define	TKN_COST	272
# define	TKN_COMM_NO	273
# define	KEYW_IGP_COST	274
# define	KEYW_SELF	275
# define	KEYW_PREPEND	276
# define	KEYW_APPEND	277
# define	KEYW_DELETE	278
# define	KEYW_CONTAINS	279
# define	KEYW_INTERNET	280
# define	KEYW_NO_EXPORT	281
# define	KEYW_NO_ADVERTISE	282
# define	KEYW_MASKLEN	283
# define	KEYW_AFI	284
# define	KEYW_ENCAPSULATION	285
# define	KEYW_TUNNEL	286
# define	TKN_INT	287


extern YYSTYPE interfacelval;

#endif /* not BISON_INTERFACE_TAB_H */
