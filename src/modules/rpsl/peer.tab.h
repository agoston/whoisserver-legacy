#ifndef BISON_PEER_TAB_H
# define BISON_PEER_TAB_H

#ifndef YYSTYPE
typedef union {
    char *sval;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	TKN_SIMPLE_PROTOCOL	257
# define	TKN_BGP4	258
# define	TKN_IPV4	259
# define	TKN_RTRSNAME	260
# define	TKN_PRNGNAME	261
# define	TKN_ASNO	262
# define	TKN_SMALLINT	263
# define	KEYW_ASNO	264
# define	KEYW_FLAP_DAMP	265
# define	TKN_DNS	266


extern YYSTYPE peerlval;

#endif /* not BISON_PEER_TAB_H */
