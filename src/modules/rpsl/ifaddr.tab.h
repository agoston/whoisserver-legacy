#ifndef BISON_IFADDR_TAB_H
# define BISON_IFADDR_TAB_H

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
# define	KEYW_ACTION	263
# define	TKN_PREF	264
# define	TKN_MED	265
# define	TKN_DPA	266
# define	TKN_ASPATH	267
# define	TKN_COMMUNITY	268
# define	TKN_NEXT_HOP	269
# define	TKN_COST	270
# define	TKN_COMM_NO	271
# define	KEYW_IGP_COST	272
# define	KEYW_SELF	273
# define	KEYW_PREPEND	274
# define	KEYW_APPEND	275
# define	KEYW_DELETE	276
# define	KEYW_CONTAINS	277
# define	KEYW_INTERNET	278
# define	KEYW_NO_EXPORT	279
# define	KEYW_NO_ADVERTISE	280
# define	KEYW_MASKLEN	281
# define	TKN_INT	282
# define	TKN_DNS	283


extern YYSTYPE ifaddrlval;

#endif /* not BISON_IFADDR_TAB_H */
