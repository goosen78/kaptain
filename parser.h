#ifndef BISON_PARSER_TAB_HPP
# define BISON_PARSER_TAB_HPP

#ifndef YYSTYPE
typedef union
{
  int integer;
  string * pstring;
  Rule * prule;
  Disjunction * pdisjunction;
  Conjunction * pconjunction;
  Parameter * pparameter;
  list<Parameter *> * pparamlist;
  string * string_array[3];
  list<string *> * pstringlist;
  list<Parameter *> * ptrafolist;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	NUMBER	257
# define	NONTERMINAL	258
# define	TERMINAL	259
# define	SPECIAL	260
# define	MODIFIER	261
# define	RARROW	262
# define	LARROW	263
# define	SUBSTITUTION	264
# define	TRANSLITERATION	265
# define	REGEXP	266
# define	ERROR	267


#define yylval  parserlval
extern YYSTYPE yylval;

#endif /* not BISON_PARSER_TAB_HPP */
