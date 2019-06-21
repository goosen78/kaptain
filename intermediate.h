#ifndef INTERMEDIATE__HH
#define INTERMEDIATE__HH

#include <list>
#include <string>

using namespace std;

class Grammar;
class Rule;
class Conjunction;
class Parameter;
class Kaptain;

#define INVALID 0
#define DISJ 1
#define CONJ 2
#define SPEC 3

#define is_tabbed     1
#define is_framed     2
#define is_horizontal 4
#define is_wizard     8
#define is_subdialog  16
#define is_reverse    32
#define is_noeval     64
#define is_double     128
#define is_beside     256
#define is_tree       512
#define is_detailed   1024
#define is_stack      2048
#define is_checkbox   4096
#define is_tristate   8192
#define is_radioparent 16384
#define is_left_aligned 32768
#define is_right_aligned 65536
#define is_radio      131072

class Intermediate
{
  friend class Kaptain;
  friend class Rule;

  list<Intermediate *> children;
  Intermediate * father;
  int type;
  int child_num; // before deleting some...

  Rule * my_rule;
  Kaptain * my_kaptain;
  Grammar * grammar;

  string textinfo[3];
  int spec_type;
  int modifiers;

public:
  Intermediate(Intermediate *);
  ~Intermediate();

  bool is_disjunction() { return type==DISJ; }
  bool is_conjunction() { return type==CONJ; }
  bool is_special()     { return type==SPEC; }

  bool is(int i);

  bool empty();

  /* interface to grammar */
  Intermediate * get_intermediate(int);
  string evaluate_nonterminal(int, bool);
  string get_name();
  bool is_default_widget();
  int get_default_selection();

  /* interface to kaptain */
  int get_selected_child();
  int get_selected_checkbox();
  int get_current_selection();
  string evaluate_special();

  /* parameters and initvalue */
  Parameter * get_parameter(int);
  int get_parameter_num();
  bool parameter_type(int, int);
  int get_int_parameter(int);
  string get_string_parameter(int);
  void get_subst_parameter(int, string &, string &, bool &);

  bool create_from(Grammar *, Rule *);
  void create_from_conjunction(Conjunction *);

  void transduce();
  void transduce_1();  
  void transduce_2();

  void print(int);

  void recieve(string);
  void send(string message, char separ=':');
};

#endif // INTERMEDIATE__HH
