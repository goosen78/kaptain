#include <algorithm>
#include <sstream>
#include <set>
#include "global.h"
#include "grammar.h"
#include "communication.h"
#include "tools.h"
#include "intermediate.h"
#include "kaptain.h"

extern Communicator * glob_communicator;


const char * special_names[]={
  "@integer",       // 1
  "@string",        // 2
  "@list",          // 3
  "@float",         // 4
  "@button",        // 5
  "@regexp",        // 6
  "@infile",        // 7
  "@outfile",       // 8
  "@directory",     // 9
  "@combo",         // 10
  "@container",     // 11
  "@close",         // 12
  "@action",        // 13
  "@edit",          // 14
  "@combow",        // 15
  "@exec",          // 16
  "@echo",          // 17
  "@icon",          // 18
  "@text",          // 19
  "@execclose",     // 20
  "@execbuffer",    // 21
  "@dump",          // 22
  "@preview",       // 23
  "@size",          // 24
  "@password",      // 25
  "@fork",          // 26
  "@fill",          // 27
  "@multicol",      // 28
  "@line"           // 29
};
int special_num=29;


const char * modifier_names[]={
  ":tabbed",        // 1
  ":framed",        // 2
  ":horizontal",    // 4
  ":wizard",        // 8
  ":dialog",        // 16
  ":reverse",       // 32
  ":noeval",        // 64
  ":double",        // 128
  ":beside",        // 256
  ":tree",          // 512
  ":detailed",      // 1024
  ":stack",         // 2048
  ":checkbox",      // 4096
  ":tristate",      // 8192
  ":radioparent",   // 16384
  ":leftaligned",   // 32768
  ":rightaligned",  // 65536
  ":radio",         // 131072
  ""
};
int modifier_num=12;
int extra_modifier_num=18;

int two_power[]={1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
                 8192, 16384, 32768, 65536, 131072, 262144, 524288,
                 1048576, 2097152, 4194304, 8388608, 16777216,
                 33554432, 67108864};


/*----------------------------------------------------------------------*/


Grammar::Grammar()
{
  start_rule=0;
  was_default_widget_done=false;
}


Grammar::~Grammar()
{
  list<Rule *>::iterator pr;

  /* delete the rules of the grammar */
  for (pr=rules.begin(); pr!=rules.end(); pr++)
    if (*pr)
      delete *pr;

  list<Description *>::iterator pd;

  /* delete the descriptions of the grammar */
  for (pd=descriptions.begin(); pd!=descriptions.end(); pd++)
    if (*pd)
      delete *pd;
  
}


void Grammar::add_rule(Rule * r)
{
  rules.push_back(r);
}


void Grammar::add_description(string *s, list<string *> *l, string **t)
{
  descriptions.push_back(new Description(this, s, l, t));
}


void Grammar::add_constraint(string *s1, string *s2)
{
  int n1=broker.get_nonterminal(s1), n2=broker.get_nonterminal(s2);

  /* add to a multimap */
  constraints.insert(pair<int, int>(n1, n2));
}


Rule * Grammar::get_rule(int nt)
{
  Rule * result=0;

  list<Rule *>::iterator ri;
  for (ri=rules.begin(); ri!=rules.end(); ri++)
    if ((*ri)->get_left()==nt)
      result=(*ri);

  /* return the pointer to the last rule of the list 
     having left side `nt' */
  return result;
}


void Grammar::verify()
{
  /* verify every separate rule */
  list<Rule *>::iterator rp;
  for (rp=rules.begin(); rp!=rules.end(); rp++)
    if (*rp) 
      // special symbol names
      // modifier symbol names
      (*rp)->verify_rule();

  /* start rule */
  verify_start_rule();

  /* recursion */
  STATUS("Looking for recursion...");
  verify_recursion();
  DONE();

  /* unreachable nonterminals */
  STATUS("Checking reachability...");
  verify_reachability();
  DONE();

  /* unterminateable nonterminals */
  STATUS("Verifying termination...");
  verify_terminateability();
  DONE();

  /* descriptions of nonexistent rules */
  verify_descriptions();

}


void Grammar::verify_start_rule()
{
  if (!GO_ON()) return; 
  string start_const("start");
  int start=broker.get_nonterminal(&start_const);
  list<Rule *>::iterator rp;
  for (rp=rules.begin(); rp!=rules.end(); rp++)
    if (*rp) 
      {
        if ((*rp)->get_left()==start) { start_rule=*rp; break; }
      }
  
  if (rp==rules.end())
    {
      ostringstream message;
      /* No `start' rule. */
      message << "No `start' rule." << endl;
      MAKE_ERROR(message.str());
    }
}


void Grammar::verify_recursion()
{
  if (!GO_ON()) return;

  int i=0;
  int N=broker.get_nt_num();
  vector<list<int> > is_on_right;
  is_on_right.resize(N);
  int validnum=0;
  vector<bool> valid;
  valid.resize(N);
  for (i=0; i<N; i++) valid[i]=false;

  list<Rule *>::iterator rp;
  for (rp=rules.begin(); rp!=rules.end(); rp++) if (*rp)
    {
      if ((*rp)->disjunction==0) continue;
      list<Conjunction *> & conj=(*rp)->disjunction->conjunctions;
      list<Conjunction *>::iterator cp;
      for (cp=conj.begin();
           cp!=conj.end();
           cp++)
        {
          list<int>::iterator ip;
          list<int> & li=(*cp)->symbols;
          for (ip=li.begin();
               ip!=li.end();
               ip++)
            if (*ip<0)
              {
                is_on_right[-*ip-1].push_back(-(*rp)->get_left()-1);
              }
        }
      if (!valid[-(*rp)->get_left()-1]) validnum++;
      valid[-(*rp)->get_left()-1]=true;
      
    }

  i=0;
  while (i<N)
    {
      for (i=0; i<N; i++) if (valid[i] && is_on_right[i].empty()) break;
      if (i<N)
        {
          /* remove rule `i' and its appeareances */
          valid[i]=false;
          validnum--;
          for (int j=0; j<N; j++) if (valid[j])
            is_on_right[j].remove(i);
          /* removes all */
        }
      else if (validnum>0)
        {
          /* recursion found */
          ostringstream message;
          message << "Recursion found in rules:";

          for (int j=0; j<N; j++)
            if (valid[j])
              message << " `" << broker.get_string(-j-1) << "'";
          message << "." << endl;

          MAKE_ERROR(message.str());
        }
    }
  
}


void Grammar::verify_reachability()
{
  if (!GO_ON()) return; 
  set<int> nt_done;
  set<int> nt_pending;
  int current;
  nt_pending.insert(start_rule->get_left());

  do
    {
      /* get first pending element */
      current=*nt_pending.begin();
      nt_pending.erase(nt_pending.begin());

      list<Rule *>::iterator rp;
      for (rp=rules.begin(); rp!=rules.end(); rp++) if (*rp)
        if ((*rp)->get_left()==current)
          {
            /* normal rule, having a right side */
            if ((*rp)->disjunction)
              {
                list<Conjunction *> & conj=(*rp)->disjunction->conjunctions;
                list<Conjunction *>::iterator cp;
                for (cp=conj.begin(); 
                     cp!=conj.end();
                     cp++) 
                  {
                    list<int>::iterator ip;
                    list<int> & li=(*cp)->symbols;
                    for (ip=li.begin(); 
                         ip!=li.end();
                         ip++)
                      if (*ip<0 && *ip!=current 
                          && find(nt_done.begin(), nt_done.end(), *ip)
                          == nt_done.end()
                          && find(nt_pending.begin(), nt_pending.end(), *ip)
                          == nt_pending.end() )
                        {
                          nt_pending.insert(*ip);
                        }
                  }
              }
            if ((*rp)->type>0 && (*rp)->parameters)
              {
                /* special symbol:
                   parameters are also reachable */
                list<Parameter *>::iterator pi;
                for (pi=(*rp)->parameters->begin();
                     pi!=(*rp)->parameters->end();
                     pi++) 
                  if (*pi && (*pi)->type==T_NONTERM) 
                    {
                      int nt=(*pi)->value;
                      if (find(nt_done.begin(), nt_done.end(), nt)
                          == nt_done.end()
                          &&
                          find(nt_pending.begin(), nt_pending.end(), nt)
                          == nt_pending.end())
                        nt_pending.insert(nt);
                    }
              }
            if ((*rp)->type>0 && (*rp)->initvalue)
              {
                /* special symbol:
                   initvalue is also reachable */
                if ((*rp)->initvalue && (*rp)->initvalue->type==T_NONTERM) 
                  {
                    int nt=(*rp)->initvalue->value;
                    if (find(nt_done.begin(), nt_done.end(), nt)
                        == nt_done.end()
                        &&
                        find(nt_pending.begin(), nt_pending.end(), nt)
                        == nt_pending.end())
                      nt_pending.insert(nt);
                  }
              }
          }
      nt_done.insert(current);
    }
  while (!nt_pending.empty());
  
  
  list<int> unreachable;


  int nt_num=broker.get_nt_num();
  for (int i=0; i<nt_num; i++)
    if (find(nt_done.begin(), nt_done.end(), -i-1)==nt_done.end())
      unreachable.push_back(-i-1);

  if (!unreachable.empty())
    {
      ostringstream message;
      message << "Unreachable nonterminals:";
      list<int>::iterator it;
      for (it=unreachable.begin(); it!=unreachable.end(); it++) 
        {
          string name=broker.get_string(*it);
          if (name[0]!='-') /* not internal nonterminal */
            message << " `" << name << "'";
        }
      message << "." << endl;
      MAKE_WARNING(message.str());
    }

}


void Grammar::verify_terminateability()
{
  if (!GO_ON()) return; 

  list<int> unterminated;
  list<Rule *>::iterator rp;
  /* check every rule */
  for (rp=rules.begin(); rp!=rules.end(); rp++) if (*rp)
    {
      /* normal rule, having a right side */
      if ((*rp)->disjunction)
        {
          list<Conjunction *> & conj=(*rp)->disjunction->conjunctions;
          list<Conjunction *>::iterator cp;
          for (cp=conj.begin(); 
               cp!=conj.end();
               cp++) 
            {
              list<int>::iterator ip;
              list<int> & li=(*cp)->symbols;
              for (ip=li.begin(); 
                   ip!=li.end();
                   ip++)
                if (*ip<0 && get_rule(*ip)==0)
                  unterminated.push_back(*ip);
            }
        }
      if ((*rp)->type>0) /* SPECIAL */
        {
          if ((*rp)->parameters)
            {
              /* parameters are also reachable */
              
              list<Parameter *>::iterator pi;
              for (pi=(*rp)->parameters->begin();
                   pi!=(*rp)->parameters->end();
                   pi++) 
                if (*pi && (*pi)->type==T_NONTERM) 
                  {
                    int nt=(*pi)->value;
                    if (nt<0 && get_rule(nt)==0)
                      unterminated.push_back(nt);
                  }
            }
          if ((*rp)->initvalue)
          {
            /* initvalue is also reachable */
            if ((*rp)->initvalue && (*rp)->initvalue->type==T_NONTERM) 
              {
                int nt=(*rp)->initvalue->value;
                if (nt<0 && get_rule(nt)==0)
                  unterminated.push_back(nt);
              }
          }
        }
    }

  /* make message */
  if (!unterminated.empty())
    {
      ostringstream message;
      message << "Unterminated nonterminals:";
      list<int>::iterator it;
      for (it=unterminated.begin(); it!=unterminated.end(); it++)
        {
          string name=broker.get_string(*it);
          if (name[0]!='-') /* not internal nonterminal */
            message << " `" << name << "'";
        }
      message << "." << endl;
      MAKE_ERROR(message.str());
    }

}


void Grammar::verify_descriptions()
{
  if (!GO_ON()) return;

  list<Description *>::iterator di;
  for (di=descriptions.begin();
       di!=descriptions.end();
       di++) if (*di)
    {
      Rule * r=get_rule((*di)->nonterminal);
      if (r==0)
        {
          ostringstream message;
          message << "No rule found for `" 
                  << broker.get_string((*di)->nonterminal)
                  << "', description ignored." << endl;
          MAKE_WARNING(message.str());

          delete *di;
          descriptions.erase(di);
        }
    }
  
}


void Grammar::unify_rules()
{
  if (!GO_ON()) return;
  /* combines several rules of the same left hand side */
  list<Rule *>::iterator r, rx;

 restart:
  for (r=rules.begin();
       r!=rules.end();
       r++) if (*r)
    {
      rx=r;
      rx++;
      while (rx!=rules.end()) if (*rx)
        {
          if ((*r)->get_left()==(*rx)->get_left())
            {
              /* unifying `r' and `rx' */
              list<Conjunction *>::reverse_iterator ci;
              if ((*r)->disjunction)
                for (ci=(*r)->disjunction->conjunctions.rbegin();
                     ci!=(*r)->disjunction->conjunctions.rend();
                     ci++)
                  if (*ci)
                    {
                      (*rx)->disjunction->add_conjunction(*ci);
                      *ci=0;
                    }
              Rule * zombie=*r;
              rules.erase(r);
              delete zombie;
              goto restart;
            }
          rx++;
        }
    }

}


void Grammar::unify_descriptions()
{
  /* insert description data into rules */
  list<Description *>::iterator di;
  for (di=descriptions.begin();
       di!=descriptions.end();
       di++)
    if (*di)
      {
        int nt=(*di)->nonterminal;
        Rule * r=get_rule(nt);
        if (r->textinfo[0].empty()) r->textinfo[0]=(*di)->textinfo[0];
        if (r->textinfo[1].empty()) r->textinfo[1]=(*di)->textinfo[1];
        if (r->textinfo[2].empty()) r->textinfo[2]=(*di)->textinfo[2];
        if ((*di)->modifiers)
          {
            r->add_modifiers((*di)->modifiers);
            (*di)->modifiers=0;
          }
      }
}


void Grammar::unify()
{
  unify_rules();
  unify_descriptions();
}


string Grammar::evaluate_nonterminal(int nt, 
                                     bool consider_noeval)
{
  //cout << "Grammar::evaluate_nonterminal(" << nt << ")" << endl;
  string s;
  Rule * r=get_rule(nt);
  if (r)
    s=r->evaluate(consider_noeval);
  return s;
}


int Grammar::get_constraint_for(int nt)
{
  int result=0;
  multimap<int,int>::iterator mi;
  mi=constraints.find(nt);
  if (mi!=constraints.end())
    result=mi->second;
  return result;
}


void Grammar::send(string message)
{
  glob_communicator->send(message);
}


void Grammar::recieve(string message)
{
  size_t b=message.find_first_of("(=?!");
  if (b!=message.npos)
    {
      string tail=message.substr(b+1,message.length()-b-1);
      string head=message.substr(0,b);
      int nt=broker.get_nonterminal(&head);
      Rule * r=get_rule(nt);
      if (r)
        r->recieve(message[b],tail);
    }
}


/*----------------------------------------------------------------------*/


Rule::Rule(Grammar * gr)
{
  grammar=gr;
  left=0;
  disjunction=0;
  parameters=0;
  type=0; /* normal by default */
  initvalue=0;
  transformations=0;
  modifiers=0;
  my_intermediate=0;

}


Rule::~Rule()
{
  if (disjunction)
    delete disjunction;

  if (parameters)
    {
      list<Parameter *>::iterator pp;
      /* delete each parameter in the list */
      for (pp=parameters->begin(); pp!=parameters->end(); pp++)
        if (*pp)
          delete *pp;
      delete parameters;
    }

  if (initvalue)
      delete initvalue;

  if (transformations)
    {
      list<Parameter *>::iterator tp;
      /* delete each transformation in the list */
      for (tp=transformations->begin(); tp!=transformations->end(); tp++)
        if (*tp)
          delete *tp;
      delete transformations;
    }

  if (modifiers)
    {
      list<string *>::iterator sp;
      /* delete each modifier in the list */
      for (sp=modifiers->begin(); sp!=modifiers->end(); sp++)
        if (*sp)
          delete *sp;
      delete modifiers;
    }

}


void Rule::set_left(string * s)
{
  left=grammar->broker.get_nonterminal(s);
}


void Rule::set_disjunction(Disjunction * d)
{
  disjunction=d;
}


void Rule::set_special(string * spec_name, list<Parameter *> * params, 
                       Parameter * init)
{
  type=1;
  if (spec_name) 
    special_type=*spec_name;

  /* remove old values */
  if (initvalue)
    delete initvalue;
  if (parameters)
    {
      list<Parameter *>::iterator pi;
      for (pi=parameters->begin();
           pi!=parameters->end();
           pi++)
        if (*pi)
          delete (*pi);
      delete parameters;
    }

  /* set new values */
  initvalue=init;
  parameters=params;
}


void Rule::add_modifiers(list<string *> *  l)
{
  if (modifiers==0)
    modifiers=l;
  else
    {
      list<string *>::iterator sp;
      for (sp=l->begin(); sp!=l->end(); sp++)
        modifiers->push_back(*sp);
      delete l;
    }
}


void Rule::set_textinfo(string ** ti)
{
  /* `ti' is an array of 3 string pointers */
  if (ti[0]) textinfo[0]=*ti[0];
  if (ti[1]) textinfo[1]=*ti[1];
  if (ti[2]) textinfo[2]=*ti[2];
}


void Rule::set_transformations(list<Parameter *> * t)
{
  transformations=t;
}


void Rule::verify_rule()
{
  if (!GO_ON()) return;
  /* verify special name 
     also sets `type' */
  if (type>0)
    {
      for (type=0; type<special_num; type++)
        if (special_names[type]==special_type) break;
      
      if (type==special_num) 
        {
          ostringstream message;
          message << "Invalid special symbol name `" 
                  << special_type
                  << "'." << endl;
          MAKE_ERROR(message.str());
          type=0;
        }
      type++;
    }

  mods=0;
  /* verify modifier names 
     also sets `mods' */
  list<string *>::iterator mi;
  if (modifiers)
  for (mi=modifiers->begin(); mi!=modifiers->end(); mi++)
    if (*mi)
      {
        int n=0;
        for (n=0; n<modifier_num; n++)
          if (modifier_names[n] == **mi)
            mods|=two_power[n];
      }
}


string Rule::evaluate(bool consider_noeval)
{
  //cout << "Rule::evaluate()" << grammar->broker.get_string(left) << endl;
  string result;
  if (type>0)
    {
      //cout << "SPEC " << type << endl;
      if (my_intermediate)
        result=my_intermediate->evaluate_special();
    }
  else if (disjunction)
    {
      if (disjunction->conjunctions.size()==0);
      else if (disjunction->conjunctions.size()==1)
        result=disjunction->conjunctions.front()->evaluate(consider_noeval);
      else
        {
          int sel=0;
          if (my_intermediate)
            sel=my_intermediate->get_current_selection();
          else
            {
              // directly default selection:
              sel=disjunction->default_num;
              /* try using constraints */
              int nt=grammar->get_constraint_for(left);
              if (nt!=0)
                {
                  Rule * source=grammar->get_rule(nt);
                  if (source && source->my_intermediate)
                    sel=source->my_intermediate->
                      get_current_selection();
                }
            }

          /* find `sel'th child */
          list<Conjunction *>::iterator ci;
          for (ci=disjunction->conjunctions.begin();
               ci!=disjunction->conjunctions.end();
               ci++)
            {
              if (sel==0) break;
              sel--;
            }
          if (ci!=disjunction->conjunctions.end())
            result=(*ci)->evaluate(consider_noeval);
        }
    }

  /* perform some transformations ... */
  if (transformations)
    {
      list<Parameter *>::iterator ti;
      for (ti=transformations->begin();
           ti!=transformations->end();
           ti++)
        if (*ti)
          {
            string result2=result;
            /* apply (*ti) to result */
            if ((*ti)->type==T_SUBST)
              {
                //cout << "s '" << (*ti)->data << "' '"
                //      << (*ti)->data2 << "' " 
                //   << ((*ti)->value ? 't' : 'f')
                //   << endl;
                /* substitution */
                Regexp::substitute((*ti)->data, // pattern
                                   result2, // src
                                   (*ti)->data2, // replacement
                                   result, // dest
                                   (*ti)->value);
              }
            if ((*ti)->type==T_TRANS)
              {
                //cout << "t '" << (*ti)->data << "' '"
                //   << (*ti)->data2 << "' "
                //   << endl;
                Regexp::transliterate((*ti)->data, // from
                                      (*ti)->data2, // to
                                      result2, // src
                                      result); // dest
              }

          }
           
    }
  if (consider_noeval && my_intermediate && my_intermediate->is(is_noeval))
    result.erase();
  return result;
}


int Rule::get_default_selection()
{
  int sel=0;
  if (disjunction)
    sel=disjunction->get_default_num();
  /* reverse checkboxes */
  if (my_intermediate->is(is_checkbox)
      && disjunction->conjunctions.size()==2
      && disjunction->conjunctions.front()->symbols.size()==0)
    /* setting checkbox means to choose nonempty */
    sel=1-sel;
  return sel;
}


int Rule::get_current_selection()
{
  int sel=0;
  if (my_intermediate)
    sel=my_intermediate->get_current_selection();

  /* treat reversed disjunctions in a different way: 
     `a -> @ | b' */
  if (my_intermediate->is(is_checkbox) 
      && disjunction->conjunctions.size()==2
      && disjunction->conjunctions.front()->symbols.size()==0)
    /* setting checkbox means to choose nonempty */
    sel=1-sel;

  return sel;
}


void Rule::send(string message, bool prefix_sender)
{
  string total;
  if (prefix_sender)
	 total=grammar->broker.get_string(left)+message+'\n';
  else
	 total=message+'\n';
  grammar->send(total);
}


void Rule::recieve(char command, string message)
{
  if (my_intermediate)
    if (command=='?')
      {
        // process here 
        send(evaluate(false), false); 
		  // don't consider :noeval and do not write sender before the message
      }
    else
      if (command=='!')
        {
          /* FIXME */
        }
      else
        {
          /* process at `intermediate' */
          my_intermediate->recieve(command+message);
        }
}


/*----------------------------------------------------------------------*/


Disjunction::Disjunction(Grammar * gr)
{
  grammar=gr;
  default_num=-1;
}


Disjunction::~Disjunction()
{
  list<Conjunction *>::iterator cp;

  /* delete each conjunction in the list */
  for(cp=conjunctions.begin(); cp!=conjunctions.end(); cp++)
    if (*cp)
      delete *cp;
}


int Disjunction::get_default_num()
{
  if (default_num==-1)
    {
      default_num=0;
      int def=0;
      list<Conjunction *>::iterator conj;

      /* set `num_default' to the last conjunction marked to be default */
      for (conj=conjunctions.begin(); 
           conj!=conjunctions.end(); 
           conj++, def++)
        if ((*conj)->get_default()) 
          default_num=def;
    }
  return default_num;
}


void Disjunction::add_conjunction(Conjunction * c)
{
  conjunctions.push_front(c);
}


/*----------------------------------------------------------------------*/


Conjunction::Conjunction(Grammar * gr)
{
  grammar=gr;
  is_default=false;
}


Conjunction::~Conjunction()
{
  
}


void Conjunction::set_default()
{
  is_default=true;
}


void Conjunction::add_nonterminal(string * s)
{
  symbols.push_front(grammar->broker.get_nonterminal(s));
}


void Conjunction::add_terminal(string * s)
{
  symbols.push_front(grammar->broker.get_terminal(s));
}


void Conjunction::add_special(string * s, list<Parameter *> * p,
                              Parameter * iv)
{
  Rule * r=new Rule(grammar);
  ostringstream name;
  int new_left_side=-grammar->broker.get_nt_num()-1;
  name << new_left_side;
  string str=name.str();
  r->set_left(&str);
  r->set_special(s, p, iv);
  grammar->add_rule(r);

  symbols.push_front(new_left_side);
}


string Conjunction::evaluate(bool consider_noeval)
{
  //cout << "Conjunction::evaluate()" << endl;
  string s;
  list<int>::iterator li;
  for (li=symbols.begin(); li!=symbols.end(); li++)
    if ((*li)>0)
      s+=grammar->broker.get_string(*li);
    else
      s+=grammar->evaluate_nonterminal(*li, consider_noeval);
  return s;
}


/*----------------------------------------------------------------------*/


Broker::Broker()
{
  /* initial table sizes */
  nonterminal_table.reserve(20);
  terminal_table.reserve(20);
}


Broker::~Broker()
{
}


int Broker::get_nonterminal(string * s)
{
  int result;
  vector<string>::iterator it;
  it=find(nonterminal_table.begin(), nonterminal_table.end(), *s);
  if (it==nonterminal_table.end())
    {
      /* insert element as new */
      nonterminal_table.push_back(*s);
      result=nonterminal_table.size()-1;
    }
  else
    result=it-nonterminal_table.begin();

  return -result-1;
}


int Broker::get_terminal(string * s)
{
  int result;
  vector<string>::iterator it;
  it=find(terminal_table.begin(), terminal_table.end(), *s);
  if (it==terminal_table.end())
    {
      /* insert element as new */
      terminal_table.push_back(*s);
      result=terminal_table.size()-1;
    }
  else
    result=it-terminal_table.begin();
  
  return result+1;
}


string Broker::get_string(int i)
{
  if (i<0)
    {
      /* nonterminal */
      return nonterminal_table[-i-1];
    }
  else
    {
      /* terminal */
      return terminal_table[i-1];
    }
}


/*----------------------------------------------------------------------*/


Parameter::Parameter(Grammar * gr)
{
  grammar=gr;
}


void Parameter::set_numeral(int n)
{
  type=T_NUMERAL;
  value=n;
}


void Parameter::set_string(string * s)
{
  type=T_STRING;
  data=*s;
}


void Parameter::set_nonterminal(string * s)
{
  type=T_NONTERM;
  value=grammar->broker.get_nonterminal(s);
}


void Parameter::set_regexp(string * s)
{
  type=T_REGEXP;
  data=*s;
}


void Parameter::set_substitution(string ** s)
{
  type=T_SUBST;
  data=*s[0];
  data2=*s[1];
  if ((*s[2])[0]=='g') value=1; // indicate `g' flag
  else value=0;
}


void Parameter::set_transliteration(string ** s)
{
  type=T_TRANS;
  data=*s[0];
  data2=*s[1];
}


/*----------------------------------------------------------------------*/


Description::Description(Grammar * gr, string *s, list<string *> *l, 
                         string **t)
{
  grammar=gr;
  nonterminal=grammar->broker.get_nonterminal(s);
  modifiers=l;
  if (t[0]) textinfo[0]=*t[0];
  if (t[1]) textinfo[1]=*t[1];  
  if (t[2]) textinfo[2]=*t[2];
}


Description::~Description()
{
  if (modifiers)
    {
      list<string *>::iterator sp;
      for (sp=modifiers->begin(); sp!=modifiers->end(); sp++)
        if (*sp)
          delete *sp;
      delete modifiers;
    }
}
