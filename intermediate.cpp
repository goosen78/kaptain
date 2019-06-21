#include "intermediate.h"
#include "grammar.h"
#include <iostream>
#include <sstream>
#include <assert.h>
#include <sys/types.h>
#include "kaptain.h"
#include <stdio.h>

extern char * special_names[];
extern char * modifier_names[];
extern int two_power[];
extern int extra_modifier_num;
extern int glob_parse_result;
extern list<Parameter *> * glob_param_list;
extern Parameter * glob_init_value;
extern int glob_parse_result;
extern void lexer_scan_string(const char *);
extern int parserparse();


Intermediate::Intermediate(Intermediate * f)
{
  type=0;
  father=f;
  my_rule=0;
  my_kaptain=0;
  child_num=0;
  grammar=0;
  modifiers=0;
}


Intermediate::~Intermediate()
{
  list<Intermediate *>::iterator ip;
  for (ip=children.begin();
       ip!=children.end();
       ip++)
    if (*ip)
      delete *ip;
}


bool Intermediate::is(int i)
{ 
  bool result=(modifiers & i);
  //cout << "IS " << modifiers << " & " << i << " = " << result << endl;
  return result; 
  //return modifiers & i;
}


bool Intermediate::create_from(Grammar * gr, Rule * r)
{
  grammar=gr;
  my_rule=r;
  if (my_rule==0) return false;

  if (my_rule->my_intermediate) return false;

  my_rule->my_intermediate=this;

  if (my_rule->type > 0)
    {
      /* special symbol */
      type=SPEC;
      spec_type=my_rule->type;
    }
  else
    {
      assert(my_rule->disjunction!=0);

      /* a rule of a nonterminal */
      if (my_rule->disjunction->conjunctions.size()>1)
        {
          type=DISJ;
          list<Conjunction *>::iterator ci;
          for (ci=my_rule->disjunction->conjunctions.begin();
               ci!=my_rule->disjunction->conjunctions.end();
               ci++)
            if (*ci)
              {
                Intermediate * child=new Intermediate(this);
                child->type=CONJ;
                child->grammar=grammar;
                child->create_from_conjunction(*ci);
                children.push_back(child);
              }
          child_num=children.size();
               
        }
      else if (my_rule->disjunction->conjunctions.size()==1)
        {

          type=CONJ;
          Conjunction * only_child;
          only_child=*(my_rule->disjunction->conjunctions.begin());
          create_from_conjunction(only_child);
        }
      else type=INVALID;
    }

  textinfo[0]=my_rule->textinfo[0];
  textinfo[1]=my_rule->textinfo[1];  
  textinfo[2]=my_rule->textinfo[2];

  modifiers=my_rule->mods;

  if (type==INVALID)
    cerr << "INVALID Intermediate Object." << endl;

  return true;
}


void Intermediate::create_from_conjunction(Conjunction * c)
{
  //c->is_default;

  list<int>::iterator ii;
  for (ii=c->symbols.begin();
       ii!=c->symbols.end();
       ii++)
    if (*ii<0) /* only nonteminals */
      {
        Rule * child_rule=grammar->get_rule(*ii);
        if (child_rule)
          {
            Intermediate * child=new Intermediate(this);
            /* throw INVALID children */
            if (child->create_from(grammar, child_rule))
              children.push_back(child);
            else
              delete child;
          }
      }
            
      child_num=children.size();
}


void Intermediate::print(int tab)
{
  for (int i=0; i<tab; i++)
    cout << "  ";
  if (type==SPEC)
    cout << "SPEC " << special_names[spec_type-1] << " ";
  if (type==DISJ)
    cout << "DISJ ";
  if (type==CONJ)
    cout << "CONJ ";
  if (type==INVALID)
    cout << "INVALID ";
  if (my_rule && my_rule->get_left()!=0)
    cout << grammar->broker.get_string(my_rule->get_left()) << " ";
  else if (my_rule) 
    cout << "0 ";
  else
    cout << "NULL ";
  cout << children.size() << " ";
  if (! textinfo[0].empty()) 
    cout << textinfo[0] << " " << modifiers << " ";

  for (int i=0; i<extra_modifier_num; i++)
    if (modifiers & two_power[i])
      cout << modifier_names[i] << " ";

  cout << endl;

  list<Intermediate *>::iterator ii;
  for (ii=children.begin();
       ii!=children.end();
       ii++)
    if (*ii)
      (*ii)->print(tab+1);


}


bool Intermediate::empty()
{
  if (type==SPEC || type==DISJ) return false;

  if (children.size()==0)
    return textinfo[0].empty();
  
  list<Intermediate *>::iterator ii;
  for (ii=children.begin();
       ii!=children.end();
       ii++)
    if (*ii && !((*ii)->empty()))
      return false; /* has a nonempty child */
  
  /* all children are empty */
  return true;
}


/* get intermediate for a given rule */
Intermediate * Intermediate::get_intermediate(int nt)
{
  Rule * r=grammar->get_rule(nt);
  return r ? r->get_intermediate() : 0;
}


string Intermediate::evaluate_nonterminal(int nt, 
                                          bool consider_noeval)
{
  return grammar->evaluate_nonterminal(nt, consider_noeval);
}

string Intermediate::get_name()
{
  string s;
  if (my_rule && my_rule->get_left()!=0)
    s=grammar->broker.get_string(my_rule->get_left());
  return s;
}


bool Intermediate::is_default_widget()
{
  if (grammar->was_default_widget_done)
    return false;
  grammar->was_default_widget_done=true;
  return true;
}


int Intermediate::get_default_selection()
{
  int sel=0;
  if (my_rule && type==DISJ)
    sel=my_rule->get_default_selection();
  return sel;
}


string Intermediate::evaluate_special()
{
  string s;
  if (my_kaptain)
    s=my_kaptain->evaluate_special();
  return s;
}


int Intermediate::get_selected_child()
{
  list<Intermediate *>::iterator child;
  int selected=0;
  if (is(is_radioparent))
    {
      for (child=children.begin(); child!=children.end(); child++)
        {
          if ((*child)->my_kaptain 
				  && (*child)->my_kaptain->radio_is_selected())
            break;
          selected++;
        }
      if (child==children.end()) selected=0;
    }
  else
	 if (is(is_stack) && my_kaptain)
		{
		  selected=my_kaptain->get_selected_page();
		}
 return selected;
}


int Intermediate::get_selected_checkbox()
{
  int i=0;
  if (is(is_checkbox) && my_kaptain)
    i=my_kaptain->get_check_selected();
  return i;
}


int Intermediate::get_current_selection()
{
  return is(is_radioparent) || is(is_stack) ? 
	 get_selected_child() : get_selected_checkbox();
}


/* handling parameters */
/* No 0 : initial value */
/* No 1,2,... : parameters specified in parentheses */
/* If `glob_parse_result' is 1, `glob_param_list' and `glob_init_value'
   are regarded */
Parameter * Intermediate::get_parameter(int n)
{
  Parameter * par=0, * _initvalue=0;
  list<Parameter *> * _parameters=0;

  if (glob_parse_result==0 && my_rule)
    {
      _initvalue=my_rule->initvalue;
      _parameters=my_rule->parameters;
    }
  else
    if (glob_parse_result==1)
      {
        _initvalue=glob_init_value;
        _parameters=glob_param_list;
      }

  if (is_special() && my_rule && n>=0)
    {
      if (n==0)
        par=_initvalue;
      else if (_parameters) {
        int i=0;
        list<Parameter *>::iterator pp=_parameters->begin();
        while (pp!=_parameters->end()) {
          i++;
          if (i==n) {
            par=*pp;
            break;    
          }
          pp++;
        }
      }
    }
  return par;
}


int Intermediate::get_parameter_num()
{
  int result=0;
  if (is_special())
    if (glob_parse_result==0 && my_rule && my_rule->parameters)
      result=my_rule->parameters->size();
    else if (glob_parse_result==1 && glob_param_list)
      result=glob_param_list->size();
  return result;
}


bool Intermediate::parameter_type(int n, int ptype)
{
  Parameter * par=get_parameter(n);
  return  (par && par->type==ptype);
}


int Intermediate::get_int_parameter(int n)
{
  Parameter * par=get_parameter(n);
  int result = (par && par->type==T_NUMERAL || par->type==T_NONTERM) 
    ? par->value : -1;
  return result;
}


string Intermediate::get_string_parameter(int n)
{
  Parameter * par=get_parameter(n);
  string s = (par && (par->type==T_STRING || par->type==T_REGEXP)) 
    ? par->data : string() ;
  int size = s.size();
  if (size>1 && s[size-1]=='\n')
	 s.erase(size-1);
  return s;
}


void Intermediate::get_subst_parameter(int n, 
                                       string & pattern, 
                                       string & replacement, 
                                       bool & global_flag)
{
  Parameter * par=get_parameter(n);
  if (par)
    {
      pattern=par->data;
      replacement=par->data2;
      global_flag=par->value==1;
    }
}


void Intermediate::transduce()
{
  transduce_1();
  transduce_2();
}


void Intermediate::transduce_1()
{
  /* BOTTOM-UP */

  bool xis_framed=false;

  list<Intermediate *>::iterator ii;
  for (ii=children.begin();
       ii!=children.end();
       ii++)
    if (*ii) 
      {
        (*ii)->transduce_1();

        /* upspread SPEC rule
        if (children.size()==1 && (*ii)->is_special())
          {
            type=(*ii)->type;
            spec_type=(*ii)->spec_type;            
            delete *ii;
            *ii=0;
            break;
            inkabb a nagyapatol kellene -
          }
        */

        /* FIXME !!!!!!!!!!!! */
        /* get selection information before killing! */

        /* 1) transfer framing data to children */
        if (is(is_framed)) 
          {
            xis_framed=true;
            (*ii)->modifiers|=2; // framed;
          }

        if ((*ii)->children.size()==1 && (*ii)->my_rule==0)
          {
            Intermediate * zombie=*ii;
            *ii=*(zombie->children.begin());
				(*ii)->father=zombie->father;
            zombie->children.clear();
            delete zombie;
          }
        else
          if ((*ii)->children.size()==0 && (*ii)->my_rule==0)
            {
              Intermediate * zombie=*ii;
              *ii=0;
              delete zombie;
            }
      }

  if (xis_framed)            
    modifiers^=2; // framed
  children.remove(NULL);
}


void Intermediate::transduce_2()
{
  /* TOP-DOWN */

  /* remove :stack if not in disjunction */
  if (is(is_stack) && (!is_disjunction()
							  || is(is_tree)))
	 modifiers^=is_stack;

  if (is_disjunction() && !is(is_stack) && children.size()<=1)
    if (child_num<3)
      modifiers|=is_checkbox; // checkbox
    else
      modifiers|=is_checkbox | is_tristate;

  list<Intermediate *>::iterator ii;
  bool has_spec_child=false;
  for (ii=children.begin();
       ii!=children.end();
       ii++)
    if (*ii) 
      {
		  if ((*ii)->is_special()) has_spec_child=true;
		}

  /* remove :tree from direct specials */
  if (is(is_tree) && has_spec_child)
	 modifiers^=is_tree;

  int empty_child=0;
  for (ii=children.begin();
       ii!=children.end();
       ii++)
    if (*ii) 
      {
        if ((*ii)->empty()) empty_child++;
		  if ((*ii)->is_special()) has_spec_child=true;

        /* `@integer' and `:beside' special case */
        if (children.size()==1 &&
            (*ii)->is_special() && (*ii)->spec_type==1) // @integer
          (*ii)->modifiers|=(is(is_beside) 
                             ? is_right_aligned :
                               is_left_aligned);
          
        /* get label for a frame */
        if (is(is_framed) && children.size()==1 && textinfo[0].empty())
          {
            textinfo[0]=(*ii)->textinfo[0];
            (*ii)->textinfo[0]=string();
          }

        /* spread :detailed in a tree */
        if (is(is_tree) && is(is_detailed))
          (*ii)->modifiers|=is_detailed; // spread 'is_detailed'

        /* radio & checkbox work */
        if (is_disjunction())
          {
				if (is(is_stack))
				  modifiers|=is_stack;
				else if (children.size()>1 /*|| child_num>3 */)
              {
                modifiers|=is_radioparent;
                (*ii)->modifiers|=is_radio; // radio
              }
            else if (children.size()==1)
              {
                modifiers|=is_checkbox; // checkbox

                if (textinfo[0].empty())
                  {
                    textinfo[0]=(*children.begin())->textinfo[0];
                    (*children.begin())->textinfo[0]=string();
                  }
              }                      
          }

        (*ii)->transduce_2();
      }


  /*
  if (children.size()==3 && is(is_radioparent) 
      && (empty_child==2 && !(children.front()->empty())
          || empty_child==3))
    {
      modifiers|=is_tristate;
      modifiers^=is_radioparent;
      list<Intermediate *>::iterator ii;
      for (ii=children.begin();

           ii!=children.end();
           ii++)
        if (*ii) 
          {
            (*ii)->modifiers^=is_radio;
          }
    }
  */
}




void Intermediate::send(string message, char separ)
{
  if (is_special() && father)
    /* send throuh SPEC's father */
    father->send(message);
  else if (my_rule)
	 if (separ!=0)
    /* send directly */
		my_rule->send(separ+message);
	 else
		my_rule->send(message, false);
}


void Intermediate::recieve(string message)
{
  if (children.size()==1 &&
      children.front()->is_special())
    /* process at the SPEC child */
    children.front()->recieve(message);
  else
    {
      /* process here */
      if (is_special())
        {
          lexer_scan_string(message.c_str());
          glob_parse_result=-1;
          glob_param_list=0;
          glob_init_value=0;
          if (parserparse()==0 && glob_parse_result==1 && my_kaptain)
            {
              /* save old parameters, initvalue */

              /* reset the widget */
              my_kaptain->create_special();

              /* restore old parameters, initvalue 
                 if the new ones are 0 */
              my_rule->set_special(0, glob_param_list, 
                                   glob_init_value);

              glob_parse_result=0; 
              glob_init_value=0;
              glob_param_list=0;
              // get_parameter now works again
              // on my_rule's params

            }
          /* garbage collection */
          if (glob_init_value)
            delete glob_init_value;
          if (glob_param_list)
            {
              list<Parameter *>::iterator pi;
              for (pi=glob_param_list->begin();
                   pi!=glob_param_list->end();
                   pi++)
                if (*pi)
                  delete (*pi);
              delete glob_param_list;
            }
        }
		else /* not special */
		  if (type==DISJ && message[0]=='=')
			 if (message[1]!='\r' && message[1]!='\n')
				{
				  int res=0;
				  if (sscanf(message.c_str()+1,"%d",&res)==1)
					 {
						my_kaptain->select_it(res);
					 }
				}
			 else
				{
				  int res=0;
				  ostringstream reply;
				  res=get_current_selection();
				  reply << res;
				  send(reply.str(),'=');				  
				}
    }

}
