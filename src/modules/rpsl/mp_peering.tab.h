#ifndef BISON_MP_PEERING_TAB_H
# define BISON_MP_PEERING_TAB_H

#ifndef YYSTYPE
typedef union {
    char *sval;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	OP_OR	257
# define	OP_AND	258
# define	TKN_ASNO	259
# define	TKN_ASNAME	260
# define	TKN_IPV4	261
# define	TKN_IPV6	262
# define	TKN_IPV6DC	263
# define	TKN_DNS	264
# define	TKN_RTRSNAME	265
# define	TKN_PRNGNAME	266
# define	KEYW_EXCEPT	267
# define	KEYW_AT	268


extern YYSTYPE mp_peeringlval;

#endif /* not BISON_MP_PEERING_TAB_H */
