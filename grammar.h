#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include <list>
#include <vector>
#include <string>
#include <map>

using namespace std;

class Grammar;
class Rule;
class Disjunction;
class Conjunction;
class Broker;
class Parameter;
class Intermediate;


/* different parameter types */
#define T_NUMERAL  1
#define T_NONTERM  2
#define T_STRING   3
#define T_REGEXP   4
#define T_SUBST    5
#define T_TRANS    6


/****************/
class Description
{
  friend class Grammar;
  friend class Intermediate;

  Grammar * grammar;
  list<string *> * modifiers;
  string textinfo[3];
  int nonterminal;

public:
  Description(Grammar *, string *, list<string *> *, string **);
  ~Description();
};



/***************/
class Parameter
{
  friend class Grammar;
  friend class Intermediate;
  friend class Rule;

  Grammar * grammar;
  int type;
  int value;
  string data;
  string data2;

public: 
  Parameter(Grammar *);

  void set_numeral(int);
  void set_string(string *);
  void set_nonterminal(string *);
  void set_regexp(string *);
  void set_substitution(string **);  
  void set_transliteration(string **);

};


/***********/
class Broker
{
  /* Broker of string<->int transformations */

  vector<string> nonterminal_table;
  vector<string> terminal_table;

public:

  Broker();
  ~Broker();

  int get_nonterminal(string *); /* returns the number of a nonterminal */
  int get_terminal(string *); /* returns the number of a terminal */

  string get_string(int); /* returns the name of a nonterminal
                             or the value of a terminal */

  int get_nt_num() { return nonterminal_table.size(); }

};


/****************/
class Conjunction
{
  friend class Grammar;
  friend class Intermediate;
  friend class Rule;

  Grammar * grammar;

  list<int> symbols;
  bool is_default;

public:
  Conjunction(Grammar *);
  ~Conjunction();

  void add_nonterminal(string *);
  void add_terminal(string *);

  void set_default();
  bool get_default() { return is_default; }

  void add_special(string *, list<Parameter *> *, Parameter *);

  string evaluate(bool);

};


/****************/
class Disjunction
{
  friend class Grammar;
  friend class Rule;
  friend class Intermediate;

  Grammar * grammar;

  list<Conjunction *> conjunctions;
  int default_num;

public:

  Disjunction(Grammar * gr=NULL);
  ~Disjunction();

  int get_default_num();
  void add_conjunction(Conjunction *);

};


/*********/
class Rule
{
  friend class Grammar;
  friend class Intermediate;

  Grammar * grammar;

  int left; /* left hand side nonterminal */
  Disjunction * disjunction; /* the right hand side */

  int type; /* special(>0) or normal(0) */
  string special_type;
  list<Parameter *> * parameters; /* parameter list */
  Parameter  * initvalue;
  list<string *> * modifiers;
  int mods; /* modifier bits */

  /* textual information for the rule */
  string textinfo[3];

  /* transformations for the rule */
  list<Parameter *> * transformations;

  Intermediate * my_intermediate;

public:

  Rule(Grammar *);
  ~Rule();

  void set_left(string *);
  void set_disjunction(Disjunction *);
  void set_special(string *, list<Parameter *> *, Parameter *);
  void add_modifiers(list<string *> *);
  void set_textinfo(string ** ti);
  void set_transformations(list<Parameter *> *);
  
  int get_left() { return left; }
  Intermediate * get_intermediate() { return my_intermediate; }
  int get_default_selection();
  int get_current_selection();

  void verify_rule();

  string evaluate(bool);

  void send(string message, bool prefix_sender=true);
  void recieve(char, string);

};


/************/
class Grammar
{
  friend class Intermediate;

  list<Rule *> rules;
  multimap<int, int> constraints;
  list<Description *> descriptions;
  Rule * start_rule;

public:
  Grammar();
  ~Grammar();

  Broker broker;
  bool was_default_widget_done; // global variable for a Kaptain tree

  void add_rule(Rule *);
  void add_description(string *, list<string *> *, string **);
  void add_constraint(string *, string *);
  
  Rule * get_rule(int);
  Rule * get_start_rule() { return start_rule; }
  
  void verify();
  void verify_start_rule();
  void verify_recursion();
  void verify_reachability();
  void verify_terminateability();
  void verify_descriptions();

  void unify();
  void unify_rules();
  void unify_descriptions();

  string evaluate_nonterminal(int, bool);
  int get_constraint_for(int);

  void send(string);
  void recieve(string);
};


#endif // GRAMMAR_HH
