#ifndef BISON_MP_PEER_TAB_H
# define BISON_MP_PEER_TAB_H

#ifndef YYSTYPE
typedef union {
    char *sval;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	TKN_SIMPLE_PROTOCOL	257
# define	TKN_NON_SIMPLE_PROTOCOL	258
# define	TKN_IPV4	259
# define	TKN_IPV6	260
# define	TKN_IPV6DC	261
# define	TKN_RTRSNAME	262
# define	TKN_PRNGNAME	263
# define	TKN_ASNO	264
# define	TKN_SMALLINT	265
# define	KEYW_ASNO	266
# define	KEYW_FLAP_DAMP	267
# define	TKN_DNS	268


extern YYSTYPE mp_peerlval;

#endif /* not BISON_MP_PEER_TAB_H */
