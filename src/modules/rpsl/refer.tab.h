#ifndef BISON_REFER_TAB_H
# define BISON_REFER_TAB_H

# ifndef YYSTYPE
#  define YYSTYPE int
#  define YYSTYPE_IS_TRIVIAL 1
# endif
# define	TKN_TYPE	257
# define	TKN_IPV4	258
# define	TKN_HOSTNAME	259
# define	TKN_PORT	260


extern YYSTYPE referlval;

#endif /* not BISON_REFER_TAB_H */
