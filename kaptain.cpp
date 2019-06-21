#include "tools.h"
#include "process.h"
#include "kaptain.h"
#include "intermediate.h"
#include "grammar.h"
#include <qlabel.h>
#include <q3header.h>
#include <q3groupbox.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <q3filedialog.h>
#include <q3textview.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <QPixmap>
#include <Q3Frame>
#include <Q3VBoxLayout>
#include <Q3BoxLayout>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <cassert>

#define BORDER 10
#define SPACING 5

extern bool global_test;
extern Grammar * yygrammar;


/* ---------------------------------------------------------------------- */


QRegValidator::QRegValidator(QString s, QWidget *parent, const char *name )
    : QValidator(parent,name)
{
  compiled_pattern=new regex_t;
  valid=!regcomp(compiled_pattern, s, REG_EXTENDED);
  if (!valid) cerr << "Ignoring invalid regular expression:" << (const char*)s << endl;
}


QRegValidator::~QRegValidator()
{
  delete compiled_pattern;
}


QValidator::State QRegValidator::validate(QString & s, int &) const
{
  regmatch_t regs[10];
  QValidator::State state=QValidator::Invalid;
  const char * str=s;
  if (valid && str && regexec(compiled_pattern, str, 10, regs, 0)==0)
    state=QValidator::Acceptable;
  return state;
}


/* ---------------------------------------------------------------------- */


QPixmap & get_pixmap(QString file_name)
{
  QPixmap * pm=new QPixmap(file_name);
  return *pm;
}


void image_text(string s, string & image, string & text)
{
  list<string> result;
  image=string(); // clear
  text=string();
  if (Regexp::matches("^\\{(.*)\\}(.*)$",s, result) && result.size()==2) 
    {
      image=result.front();
      text=result.back();
    }
  else
    text=s;
}


/* make button label or image */
void button_label(QPushButton * push_button, string s)
{
  string image, text;
  image_text(s, image, text);
  if (image.empty())
    push_button->setText(text.c_str());
  else
    push_button->setPixmap(get_pixmap(image.c_str()));
}


/* ---------------------------------------------------------------------- */

Kaptain::Kaptain(Intermediate * i, Kaptain * p,
                 QWidget * widget, Q3BoxLayout * layout, QDialog * dialog,
                 bool _no_title, const char *)
{
  im=i;
  assert(im!=0);
  im->my_kaptain=this;
  parent=p;

  /* parent_widget is always needed */
  parent_widget=widget;
  parent_dialog=dialog;
  parent_layout=layout;
  if (!parent_widget)
    {
      if (im->is(is_wizard) && parent==0) 
        {
          /* only top level wizard */
          wizard=new Q3Wizard(dialog, im->get_name().c_str(), true);
          parent_dialog=wizard;
          parent_widget=parent_dialog;
        }
      else
        {
          parent_dialog=new QDialog(dialog, im->get_name().c_str(), true);
          parent_widget=parent_dialog;
          parent_layout=new Q3VBoxLayout(parent_widget, BORDER, SPACING);
        }
      kill_dialog=true;
    }
  else
    kill_dialog=false;

  myself=0;

  radio_button=0;
  checkbox=0;
  spin_box=0;
  line_edit=0;
  tab_widget=0;
  list_box=0;
  combo_box=0;
  push_button=0;
  list_view=0;
  list_view_item=0;
  widget_stack=0;
  my_panel=0;
  mapping=0;
  stack_size=0;
  container_source=0;
  edit=0;
  child_dialog=0;
  alignment=0;
  grid=0;
  grid_rows=1;
  grid_columns=1;

  no_title=_no_title;
  done_title=false;
  title=im->textinfo[0].c_str();
  tooltip=im->textinfo[1].c_str();
  whatsthis=im->textinfo[2].c_str();
  no_tree=false; // forbids tree creation if true
  ignore_radio=false;

  children.setAutoDelete(true);

  create();
  set_info(myself);
}


Kaptain::~Kaptain()
{
  /*
  if (kill_dialog && parent_dialog)
    delete parent_dialog;
  */

  if (mapping)
    delete mapping;
  
}

void Kaptain::create()
{
  if (im->type==INVALID) return;
  if (im->is(is_wizard) && parent==0)
    {
      // only toplevel node can be wizard
      create_wizard();
    }
  else if (parent && parent->im->is(is_tree) && !no_tree)
    {
      /*-- inside a tree --*/
      create_tree_item();
    }
  else 
    {
      /*-- out of a tree --*/
      no_title=no_title || im->is(is_subdialog);

      /** create frame **/
      if (im->is(is_framed))
        create_frame();
        
      /** create title **/
      create_title(); // also creates sublayout

      /** create object(s) **/
      if (im->is_special())
        {
          /* a special object */
          create_minimum_layout();
          create_special();
          /* using proper alignment */
          if (im->is(is_left_aligned))
            alignment=Qt::AlignLeft;
          if (im->is(is_right_aligned))
            alignment=Qt::AlignRight;
          /* inserted into a layout */
          if (myself)
            parent_layout->addWidget(myself, 0, alignment);
          /* give keyboard focus to the first SPEC widget */
          if (myself && im->is_default_widget())
            myself->setFocus();
        }
      else
        {
          /* children sequence */

          if (im->is(is_tabbed))
            {
              /* tab widget */
              create_minimum_layout();
              create_tabbed();
            }
          else if (im->is(is_tree))
            {
              /* tree view */
              create_minimum_layout();
              create_tree_view();
            }
          else if (im->is(is_subdialog))
            {
              /* subdialog */
              create_minimum_layout();
              create_subdialog();
            }
          else if (im->is(is_stack))
				{
				  /* stack */
              create_minimum_layout();
				  create_stack();
				}
			 else
            {
              /* box layout */
              if (im->is(is_double))
                create_grid_layout();
              create_sequence();
            }
        }
    }
}


void Kaptain::create_frame()
{
  if (!parent_layout)
    {
      parent_layout=new Q3VBoxLayout(parent_widget, SPACING);
    }
  
  Q3GroupBox * frame=new Q3GroupBox(title, parent_widget);
  done_title=true;
  parent_widget=frame;
  parent_layout->addWidget(parent_widget);

  int frame_size = 2*frame->lineWidth() 
    + frame->midLineWidth() 
    + frame->margin();

  parent_layout=new Q3VBoxLayout(frame, frame_size+5, SPACING);
  parent_layout->addSpacing(frame->fontMetrics().height()-frame_size);

  set_info(frame);
}


void Kaptain::create_title()
{
  /* dialog title */
  if (kill_dialog) // I'm dialog owner
    {
      parent_dialog->setCaption(title); // what about icons ???
      done_title=true;
    }

  if (!((no_title || done_title)))
    //&& !im->is(is_radio) && !im->is(is_checkbox))
    {
      /* need a layout for the title */
      Q3BoxLayout::Direction need_dir=Q3BoxLayout::TopToBottom;
      if (im->is(is_beside))
        need_dir=Q3BoxLayout::LeftToRight;
      if (!(parent_layout && parent_layout->direction()==need_dir))
        {
          if (parent_layout)
            parent_layout=new Q3BoxLayout(parent_layout, need_dir, SPACING);
          else
            parent_layout=new Q3BoxLayout(parent_widget, need_dir, 
                                         BORDER, SPACING);
        }
      
      if (im->is(is_radio) && !ignore_radio)
        {
          /* place a radio button */
          radio_button=new QRadioButton(title, parent_widget);
          parent_layout->addWidget(radio_button);
          connect(radio_button, SIGNAL(clicked()), SLOT(radio_clicked()));
          done_title=true;
          set_info(radio_button);
        }
      if (im->is(is_checkbox))
        {
          if (im->is(is_radio) && !ignore_radio) // both radio and checkbox
            {
              /* radio was done above. insert indentation */
              Q3HBoxLayout * indent=new Q3HBoxLayout(parent_layout);
              indent->addSpacing(parent_widget->fontMetrics().height()
                                 +SPACING);
              Q3VBoxLayout * indent2=new Q3VBoxLayout(indent);
              parent_layout=indent2;
              /* FIXME shouldn't the title differ? */
            }
          /* create checkbox */
          checkbox=new QCheckBox(title, parent_widget);
          if (im->is(is_tristate))
            checkbox->setTristate();
          parent_layout->addWidget(checkbox);
			 connect(checkbox, SIGNAL(stateChanged(int)), 
						SLOT(message_checkbox(int)));
          done_title=true;
          set_info(checkbox);
        }
      if (!no_title && !done_title && !title.isEmpty())
        {
          /* if all else fails, a simple label */
          QLabel * label=new QLabel(title, parent_widget);
          parent_layout->addWidget(label);
          set_info(label);
        }
  
      done_title=true;

      /* put content a bit indented to the radio or checkbox */
      if ( ( im->is(is_radio) || im->is(is_checkbox) ) 
           && need_dir==Q3BoxLayout::TopToBottom
           && !(im->empty()))
        {   
          Q3HBoxLayout * indent=new Q3HBoxLayout(parent_layout);
          indent->addSpacing(parent_widget->fontMetrics().height()+SPACING);
          parent_layout=indent;
        }
    }

  // create sublayout suitable for children
  create_sublayout();
}


void Kaptain::create_minimum_layout()
{
  if (parent_layout==0)
    parent_layout=new Q3VBoxLayout(parent_widget, BORDER, SPACING);
}

void Kaptain::create_sublayout()
{
  Q3BoxLayout::Direction dir;

  /* calculate layout direction needed */
  if (im->is(is_double))
    dir=Q3BoxLayout::TopToBottom; // no matter what direction
  else
    if (im->is(is_horizontal) || im->is(is_beside))
      if (im->is(is_reverse))
        dir=Q3BoxLayout::RightToLeft; 
      else
        dir=Q3BoxLayout::LeftToRight;      
    else
      if (im->is(is_reverse))
        dir=Q3BoxLayout::BottomToTop; 
      else
        dir=Q3BoxLayout::TopToBottom;

  /* create if parent_layout is different */
  if (!(parent_layout && (parent_layout->direction()==dir || 
                          im->is(is_double))))
    {
      if (parent_layout)
        parent_layout=new Q3BoxLayout(parent_layout, dir, SPACING);
      else
        parent_layout=new Q3BoxLayout(parent_widget, dir, BORDER, SPACING);
    }
      
  /* grid layout is created just before create_sequence() is called */
}


void Kaptain::create_grid_layout()
{
  /* double columns or rows */
  grid_rows=(im->children.size()+1)/2;
  grid_columns=2;

  if (im->is(is_horizontal))
    { int t=grid_rows; grid_rows=grid_columns; grid_columns=t; }

  grid=new Q3GridLayout(parent_layout, grid_rows, grid_columns, SPACING);
  //parent_layout->setStretchFactor(grid, 1);
  /*for (int i=0; i<grid_rows; i++)
    grid->setRowStretch(i,1);
  for (int i=0; i<grid_columns; i++)
  grid->setColStretch(i,1);*/
}


void Kaptain::create_sequence()
{
  create_sublayout();

  int child_index=0, child_num=im->children.size();
  list<Intermediate *>::iterator ii;
  for (ii=im->children.begin();
       ii!=im->children.end();
       ii++)
    if (*ii)
      {
        /* render new layout if needed */
        Q3BoxLayout * new_layout=parent_layout;
        if (im->is(is_double))
          {
            int x,y, pos=child_index;
            if (im->is(is_reverse)) 
              pos=child_num-1-pos;
            x=pos / 2;
            y=pos % 2;
            if (im->is(is_horizontal))
              {int t=x;x=y;y=t;}
            new_layout=new Q3VBoxLayout(SPACING);
            grid->addLayout(new_layout, x, y);
          }
          
        Kaptain * child=new Kaptain(*ii, this, parent_widget, new_layout, 
                                    parent_dialog);
        children.append(child);
        child_index++;
      }
  select_defaults();
}


void Kaptain::select_defaults()
{
  /* default selection */
  if (im->type==DISJ)
    {
      int sel=im->get_default_selection();
		select_it(sel);
    }
}


void Kaptain::select_it(int sel)
{
		int item=0;
      Kaptain * child;
      if (im->is(is_radioparent))
        {
          for (child=children.first(); child; child=children.next())
            {
              if (item==sel)
                break;
              item++;
            }
          if (!child) child=children.first();
          child->radio_select(true);
        }
      else if (im->is(is_checkbox))
		  if (checkbox)
			 if (im->is(is_tristate))
				switch (sel)
				  {
				  case 0:
					 checkbox->setChecked(true);
					 break;
				  case 1:
					 checkbox->setNoChange();
					 break;
				  case 2:
					 checkbox->setChecked(false);
					 break;
				  default:
					 break;
				  }
			 else
				checkbox->setChecked(!sel);
		  else {
			 if (list_view_item)
				((Q3CheckListItem *)list_view_item)->setOn(!sel);
                  }
		else if (im->is(is_stack))
		  {
			 select_page(sel);
		  }

}

void Kaptain::create_tabbed()
{
  tab_widget=new QTabWidget(parent_widget);
  parent_layout->addWidget(tab_widget);

  list<Intermediate *>::iterator ii;
  for (ii=im->children.begin();
       ii!=im->children.end();
       ii++)
    if (*ii)
      {
        QWidget * new_widget=new QWidget(tab_widget);
        tab_widget->addTab(new_widget, QString((*ii)->textinfo[0].c_str()));

        Kaptain * child=new Kaptain(*ii, this, new_widget, 0, 
                                    parent_dialog, true);
        children.append(child);
      }

}


void Kaptain::create_stack()
{
  widget_stack=new Q3WidgetStack(parent_widget);
  parent_layout->addWidget(widget_stack);

  int i=0;
  list<Intermediate *>::iterator ii;
  for (ii=im->children.begin();
       ii!=im->children.end();
       ii++)
    if (*ii)
      {
        QWidget * new_widget=new QWidget(widget_stack);
        widget_stack->addWidget(new_widget, i);

        Kaptain * child=new Kaptain(*ii, this, new_widget, 0, 
                                    parent_dialog, true);
        children.append(child);
		  i++;
      }

  select_defaults();
}


void Kaptain::select_page(int p)
{
  if (widget_stack && mapping==0)
	 widget_stack->raiseWidget(p);
}


int Kaptain::get_selected_page()
{
  int selected=0;
  if (widget_stack)
	 {
		selected=widget_stack->id(widget_stack->visibleWidget());
		if (selected==-1) selected=0;
	 }
  return selected;
}


Kaptain * Kaptain::get_tree_top()
{
  /* find topmost tree element */
  Kaptain * ancestor=this;
  while (ancestor && ancestor->widget_stack==0)
    ancestor=ancestor->parent;
  return ancestor;
}


void Kaptain::create_tree_view()
{
  list_view=new Q3ListView(parent_widget);
  mapping=new Q3PtrDict<Kaptain>;
  parent_layout->addWidget(list_view);

  /* list view setup */
  list_view->setSorting(-1);
  list_view->setRootIsDecorated(true);
  list_view->addColumn(QString());
  if (im->is(is_detailed))
    list_view->addColumn(QString()); // two columns
  else
    list_view->header()->hide(); // no header

  /* create widget stack */
  widget_stack=new Q3WidgetStack(parent_widget);

  list<Intermediate *>::reverse_iterator ii;
  /* simply insert child nodes */
  for (ii=im->children.rbegin();
       ii!=im->children.rend();
       ii++)
    if (*ii)
      {
        Kaptain * child=new Kaptain(*ii, this, parent_widget, 
                                    parent_layout, parent_dialog);
        children.append(child);
      }
  select_defaults();

  if (stack_size) /* panel should be created */
    {
      /* insert empty panel with 0 id */
      widget_stack->addWidget(new QWidget(widget_stack),0);
      parent_layout->addWidget(widget_stack);
      connect(list_view, SIGNAL(currentChanged(Q3ListViewItem *)), 
              SLOT(panel_selected(Q3ListViewItem *)));
    }
  else
    delete widget_stack;

  list_view->adjustSize();
}


void Kaptain::create_tree_item()
{
  // take image and label //
  string img, txt, tit;
  tit=title.latin1();
  image_text(tit,img,txt);
  title=QString(txt.c_str());

  if (parent && parent->list_view_item)
    {
      if (im->is(is_radioparent))
        list_view_item=new Q3CheckListItem(parent->list_view_item, title, 
                                          Q3CheckListItem::Controller);
      else if (im->is(is_radio)) {
        list_view_item=new Q3CheckListItem((Q3CheckListItem *)
                                          parent->list_view_item, title, 
                                          Q3CheckListItem::RadioButton);
		}

      else if (im->is(is_checkbox)) {
        list_view_item=new Q3CheckListItem(parent->list_view_item, title, 
                                          Q3CheckListItem::CheckBox);
		}

      else
        list_view_item=new Q3ListViewItem(parent->list_view_item, title);
    }
  else if (parent && parent->list_view)
    {
      if (im->is(is_radioparent))
        list_view_item=new Q3CheckListItem(parent->list_view, title, 
                                          Q3CheckListItem::Controller);
      else if (im->is(is_radio))
        {
          parent->list_view_item=
            new Q3CheckListItem(parent->list_view, 
                               parent->title, Q3CheckListItem::Controller);
          list_view_item=new Q3CheckListItem((Q3CheckListItem *)
                                            parent->list_view_item, title, 
                                            Q3CheckListItem::RadioButton);
        }
      else if (im->is(is_checkbox)) {
        list_view_item=new Q3CheckListItem(parent->list_view, title, 
                                          Q3CheckListItem::CheckBox);
		}
      else
        list_view_item=new Q3ListViewItem(parent->list_view, title);
    }

  if (im->is(is_detailed))
    list_view_item->setText(1,tooltip);

  if (!img.empty())
    list_view_item->setPixmap(0, get_pixmap(img.c_str()));

  no_tree=!(im->is(is_tree)) || im->is_special() 
    || im->is(is_radio) || im->is(is_checkbox);
  /* only insert children into widget stack, not this node */
  ignore_radio= im->is(is_radio) && im->is(is_checkbox);
  no_title= im->is(is_radio) != im->is(is_checkbox);

  if (no_tree)
    {
      /* create this node again on the widget_stack */ 
      Kaptain * top=get_tree_top();
      parent_widget=new QWidget(top->widget_stack);
      my_panel=parent_widget;
      parent_layout=0;
      top->widget_stack->addWidget(parent_widget,top->stack_size);
      /* to raise the widget when the tree item is selected
         the mapping list_view_item->Kaptain is needed */
      top->mapping->insert((void *)list_view_item, this);
      /* call create again with no_tree set */
      if (im->children.size()>0 || ignore_radio)
        {
          create();
          top->stack_size++;
        }
    }
  else
    {
      list<Intermediate *>::reverse_iterator ii;
      /* insert child nodes */
      for (ii=im->children.rbegin();
           ii!=im->children.rend();
           ii++)
        if (*ii)
          {
            Kaptain * child=new Kaptain(*ii, this, parent_widget, 
                                        parent_layout, parent_dialog);
            children.append(child);
          }
    }
  select_defaults();    
}


void Kaptain::panel_selected(Q3ListViewItem * item)
{
  Kaptain * owner=mapping->find((void *)item);
  if (owner)
    widget_stack->raiseWidget(owner->my_panel);
  else 
    widget_stack->raiseWidget(0);
}


void Kaptain::create_subdialog()
{
  QString label;
  push_button=new QPushButton(parent_widget);
  parent_layout->addWidget(push_button);
  button_label(push_button, title.latin1());
  push_button->setFixedSize(push_button->sizeHint());
  connect(push_button, SIGNAL(clicked()), SLOT(subdialog_clicked()));

  if (im->children.size()>1)
    {
      child_dialog=new QDialog(parent_dialog, im->get_name().c_str(), true);
      Q3BoxLayout * child_layout=new Q3VBoxLayout(child_dialog, BORDER, SPACING);
      child_dialog->setCaption(title);
      
      list<Intermediate *>::iterator ii;
      for (ii=im->children.begin();
           ii!=im->children.end();
           ii++)
        if (*ii)
          {
            Kaptain * child=new Kaptain(*ii, this, child_dialog, 
                                        child_layout, child_dialog);
            children.append(child);
          }
    }
  else if (im->children.size()==1)
    {
      Kaptain * child=new Kaptain(im->children.front(), this, 0, 0, 0);
      children.append(child);
      child_dialog=child->main_dialog();
    }
 
}


void Kaptain::create_wizard()
{
  list<Intermediate *>::iterator ii;
  for (ii=im->children.begin();
       ii!=im->children.end();
       ii++)
    if (*ii)
      {
        QWidget * new_widget=new QWidget(wizard);
        wizard->addPage(new_widget, QString((*ii)->textinfo[0].c_str()));
        wizard->setCaption(title);
        wizard->setHelpEnabled(new_widget, false);
        
        Kaptain * child=new Kaptain(*ii, this, new_widget, 0, wizard, true);
        children.append(child);
      }
}


void Kaptain::create_special()
{
  int spec=im->spec_type;
  bool again=false;
  /* creates a widget and sets myself to point to it */
  switch (spec)
    {    
    case 1: // @integer
      again=spin_box;
      if (!again)
        {
          spin_box=new QSpinBox(parent_widget);
          myself=spin_box;
          spin_box->setFixedSize(spin_box->sizeHint());
        }
      {
        /* initial setup */
        int min=0, max=99, init=0;
        if (im->parameter_type(0,T_NUMERAL)) init=im->get_int_parameter(0);
        if (im->parameter_type(1,T_NUMERAL)) min=im->get_int_parameter(1);
        if (im->parameter_type(2,T_NUMERAL)) max=im->get_int_parameter(2);
        if (max<min) max=min;
        if (init<min) init=min;
        if (init>max) init=max;
        spin_box->setRange(min,max);
        spin_box->setValue(init);        
      }
      break;

    case 2: // @string --> @float, @regex, @password
      again=line_edit;
      if (!again)
        {
          line_edit=new QLineEdit(parent_widget);
          myself=line_edit;
        }
      /* initial value */
      if (im->parameter_type(0,T_STRING)) 
        line_edit->setText(QString(im->get_string_parameter(0).c_str()));
      /* maximum length */
      if (im->parameter_type(1,T_NUMERAL)) 
        line_edit->setMaxLength(im->get_int_parameter(1));
      break;

    case 3: // @list --> @combo, @comborw
      {
        again=list_box;
        if (!again)
          {
            list_box=new Q3ListBox(parent_widget);
				connect(list_box, SIGNAL(highlighted(const QString &)),
						  SLOT(message_listbox(const QString &)));
            myself=list_box;
          }
        else
          {
            if (im->get_parameter_num()>0) 
              list_box->clear();
          }

        bool has_string_regexp_init=false, regexp=false;
        bool s_global_flag=false, has_substitution=false;
        string s_pattern, s_replacement, pattern;
        list<string> result;
        
        if (im->parameter_type(0,T_STRING) || im->parameter_type(0,T_REGEXP))
          {
            has_string_regexp_init=true;
            regexp=im->parameter_type(0,T_REGEXP);
            pattern=im->get_string_parameter(0);
          }
      
        int items_inserted=0;
        /* process parameters and initvalue */
        int pn=im->get_parameter_num();
        for (int i=1; i<=pn; i++) {
          if (im->parameter_type(i,T_SUBST))
            {
              /* update substitution */
              has_substitution=true;
              im->get_subst_parameter(i, s_pattern, s_replacement, 
                                      s_global_flag);
            }
          if (im->parameter_type(i,T_STRING))
            {
              list<string> lines;
              Regexp::split('\n', im->get_string_parameter(i), lines);
              list<string>::iterator it;
              for (it=lines.begin();
                   it!=lines.end();
                   it++)
                {
                  string original_line=(*it), 
                    modified_line=original_line;
                  /* execute substitution */
                  if (has_substitution)
                    Regexp::substitute(s_pattern, original_line,
                                       s_replacement, modified_line, 
                                       s_global_flag);
                  list_box->insertItem(modified_line.c_str());
                  if (has_string_regexp_init)
                    {
                      /* select the line 
                         - equal to a string 
                         - matching a regexp
                      */
                      if ((!regexp && pattern==(*it)) ||
                          (regexp && Regexp::matches
                           (pattern, original_line, result)))
                        {
                          has_string_regexp_init=false; // select first choice
                          list_box->setCurrentItem(items_inserted);
                        }
                    }
                  items_inserted++;
                }
            }
        }
        /* integer initvalue */
        if (im->parameter_type(0,T_NUMERAL))
          list_box->setCurrentItem(im->get_int_parameter(0)-1);
      }
      break;

    case 4: // @float --> @string, @regex, @password
      again=line_edit;
      if (!again)
        {
          line_edit=new QLineEdit(parent_widget);
          myself=line_edit;
          line_edit->setValidator(new QDoubleValidator(parent_widget));
        }
      /* initial value */
      if (im->parameter_type(0,T_STRING)) 
        line_edit->setText(QString(im->get_string_parameter(0).c_str()));
      if (im->parameter_type(0,T_NUMERAL)) 
        line_edit->setText(QString::number(im->get_int_parameter(0)));
      break;

    case 5: // @button
    case 12: // @close
    case 13: // @action
    case 16: // @exec
    case 17: // @echo
    case 20: // @execclose
    case 21: // @execbuffer
    case 22: // @dump
    case 23: // @preview
    case 26: // @fork
      again=push_button;
      if (!again)
        {
          push_button=new QPushButton(parent_widget);
          myself=push_button;
        }

      /* initial value is the label */
      if (im->parameter_type(0,T_STRING))
        button_label(push_button, im->get_string_parameter(0));
      push_button->setFixedSize(push_button->sizeHint());
      if (im->parameter_type(0,T_STRING) 
			 && im->get_string_parameter(0)[0]==' ')
        //if (im->is_default_button())
        push_button->setDefault(true);

      /* button pressed ... */
      if (!again)
        connect(push_button, SIGNAL(clicked()), SLOT(button_pressed()));
      break;

    case 6: // @regex --> @string, @float, @password
      again=line_edit;
      if (!again)
        {
          line_edit=new QLineEdit(parent_widget);
          myself=line_edit;
        }

      /* first parameter is regular expression */
      if (im->parameter_type(1,T_STRING) ||
          im->parameter_type(1,T_REGEXP))
        {
          line_edit->setValidator
            (new QRegValidator(im->get_string_parameter(1).c_str(), 
                               parent_widget));
          list<string> result;
          /* initial value */
          if (im->parameter_type(0,T_STRING) && 
              Regexp::matches(im->get_string_parameter(1).c_str(),
                              im->get_string_parameter(0).c_str(),result))
            line_edit->setText(im->get_string_parameter(0).c_str());
        }
      break;

    case 7: // @infile
    case 8: // @outfile
    case 9: // @directory
      again=line_edit;
      if (!again)
        {
          myself=new Q3HBox(parent_widget);
          ((Q3HBox *)myself)->setSpacing(SPACING);
          line_edit=new QLineEdit(myself);
          push_button=new QPushButton(myself);
          push_button->setText("...");
          push_button->adjustSize();
        }
      if (im->parameter_type(0,T_STRING))
        line_edit->setText(im->get_string_parameter(0).c_str());
      /* button pressed ... */
      if (!again)
        connect(push_button, SIGNAL(clicked()), SLOT(button_pressed()));
      break;

    case 10: // @combo
    case 15: // @combow --> @list
      {
        again=combo_box;
        if (!again)
          {
            combo_box=new QComboBox(spec==15, parent_widget);
				connect(combo_box, SIGNAL(activated(const QString &)),
						  SLOT(message_listbox(const QString &)));
            myself=combo_box;
          }
        else
          {
            if (im->get_parameter_num()>0) 
              combo_box->clear();
          }

        bool has_string_regexp_init=false, regexp=false;
        bool s_global_flag=false, has_substitution=false;
        string s_pattern, s_replacement, pattern;
        list<string> result;
        
        if (im->parameter_type(0,T_STRING) || im->parameter_type(0,T_REGEXP))
          {
            has_string_regexp_init=true;
            regexp=im->parameter_type(0,T_REGEXP);
            pattern=im->get_string_parameter(0);
          }
      
        int items_inserted=0;
        /* process parameters and initvalue */
        int pn=im->get_parameter_num();
        for (int i=1; i<=pn; i++) {
          if (im->parameter_type(i,T_SUBST))
            {
              /* update substitution */
              has_substitution=true;
              im->get_subst_parameter(i, s_pattern, s_replacement, 
                                      s_global_flag);
            }
          if (im->parameter_type(i,T_STRING))
            {
              list<string> lines;
              Regexp::split('\n', im->get_string_parameter(i), lines);
              list<string>::iterator it;
              for (it=lines.begin();
                   it!=lines.end();
                   it++)
                {
                  string original_line=(*it), 
                    modified_line=original_line;
                  /* execute substitution */
                  if (has_substitution)
                    Regexp::substitute(s_pattern, original_line,
                                       s_replacement, modified_line, 
                                       s_global_flag);
                  combo_box->insertItem(modified_line.c_str());
                  if (has_string_regexp_init)
                    {
                      /* select the line 
                         - equal to a string 
                         - matching a regexp
                      */
                      if ((!regexp && pattern==(*it)) ||
                          (regexp && Regexp::matches
                           (pattern, original_line, result)))
                        {
                          has_string_regexp_init=false; // select first choice
                          combo_box->setCurrentItem(items_inserted);
                        }
                    }
                  items_inserted++;
                }
            }
        }
        /* integer initvalue */
        if (im->parameter_type(0,T_NUMERAL))
          combo_box->setCurrentItem(im->get_int_parameter(0)-1);
      }
      break;
        
    case 11: // @container
      {
        again=list_box;
        if (!again)
          {
            Q3VBox * vbox=new Q3VBox(parent_widget);
            myself=vbox;
            Q3HBox * hbox=new Q3HBox(vbox);
            hbox->setSpacing(SPACING);
            vbox->setSpacing(SPACING);
            list_box=new Q3ListBox(vbox);
            // buttons
            push_button=new QPushButton(hbox);
            push_button2=new QPushButton(hbox);
          }

        string txt_add("Add");
        string txt_remove("Remove");
        /* 1: source for the container: NONTERMINAL */
        if (im->parameter_type(1,T_NONTERM))
          container_source=im->get_int_parameter(1);
        /* 2,3: Add, Remove button label: string */
        if (im->parameter_type(2,T_STRING))
          txt_add=im->get_string_parameter(2);
        if (im->parameter_type(3,T_STRING))
          txt_remove=im->get_string_parameter(3);
        // text or image label
        if (!again || im->parameter_type(2,T_STRING))
          button_label(push_button, txt_add.c_str());
        if (!again || im->parameter_type(3,T_STRING))
          button_label(push_button2, txt_remove.c_str());
        push_button->setFixedSize(push_button->sizeHint());
        push_button2->setFixedSize(push_button2->sizeHint());
        /* when button is pressed */
        if (!again)
          {
            connect(push_button, SIGNAL(clicked()), SLOT(container_add()));
            connect(push_button2, SIGNAL(clicked()), SLOT(container_remove()));
          }
      }
      break;

    case 14: // @edit --> @preview's pressed action
      {
        again=edit;
        if (!again)
          {
            edit=new Q3MultiLineEdit(parent_widget);
            myself=edit;
          }
        /* init value */
        if (im->parameter_type(0,T_STRING))
          edit->setText(im->get_string_parameter(0).c_str());
        int param=1;
        /* font, size parameters */
        if (im->parameter_type(1,T_STRING)) {
          string font_name=im->get_string_parameter(1);
          QFont font(font_name.c_str());
          if (font_name[0]=='-')
            font.setRawName(font_name.c_str());
          edit->setFont(font);
          param++;
        }
        /* window size */
        if (im->parameter_type(param,T_NUMERAL))
          edit->setMinimumWidth(im->get_int_parameter(param));
        if (im->parameter_type(param+1,T_NUMERAL))
          edit->setMinimumHeight(im->get_int_parameter(param+1));
      }
      break;

    case 18: // @icon
      {
        QLabel * label=(QLabel *)myself;
        again=label;
        if (!again)
          {
            label=new QLabel(parent_widget);
            myself=label;
          }

        if (im->parameter_type(1,T_STRING))
          {
            QPixmap & pixmap=get_pixmap(im->get_string_parameter(1).c_str());
            label->setPixmap(pixmap);
            label->setFixedSize(pixmap.size());
          }
      }
      break;

    case 19: // @text
      {
        QLabel * text=(QLabel *)myself;
        again=text;
        if (!again)
          {
            text=new QLabel(parent_widget);
            myself=text;
            text->setAlignment(Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap);
            text->setFrameStyle(Q3Frame::Panel | Q3Frame::Sunken);
          }

        /* concatenate parameters */
        list<string> lines;
        int pm=im->get_parameter_num();
        for (int i=0; i<=pm; i++)
          {
            if (im->parameter_type(i,T_STRING))
              lines.push_back(im->get_string_parameter(i));
          }
        string joined;
        Regexp::join('\n', lines, joined);
        text->setText(joined.c_str());
      }
      break;

    case 24: // @size
      if (im->parameter_type(1,T_NUMERAL))
        parent_widget->setMinimumWidth(im->get_int_parameter(1));
      if (im->parameter_type(2,T_NUMERAL))
        parent_widget->setMinimumHeight(im->get_int_parameter(2));
      if (im->parameter_type(3,T_NUMERAL))
        parent_widget->setMaximumWidth(im->get_int_parameter(3));
      if (im->parameter_type(4,T_NUMERAL))
        parent_widget->setMaximumHeight(im->get_int_parameter(4));
      break;

    case 25: // @password --> @string, @float, @regex
      again=line_edit;
      if (!again)
        {
          line_edit=new QLineEdit(parent_widget);
          myself=line_edit;
          line_edit->setEchoMode(QLineEdit::Password);
        }

      /* initial value */
      if (im->parameter_type(0,T_STRING)) 
        line_edit->setText(QString(im->get_string_parameter(0).c_str()));
      /* maximum length */
      if (im->parameter_type(1,T_NUMERAL)) 
        line_edit->setMaxLength(im->get_int_parameter(1));
      break;

    case 27: // @fill
      myself=new QWidget(parent_widget);
      parent_layout->addWidget(myself,1);
      myself=0; // do not add it later to the layout
      break;

    case 28: // @multicol -:-> @list
      {
        again=list_view;
        if (!again)
          {
            list_view=new Q3ListView(parent_widget);
            list_view->setAllColumnsShowFocus(true);
            list_view->setSorting(-1);
            myself=list_view;
          }
        else
          {
            list_view->clear();
          }

        /* coulomn separation, substitution */
        bool has_regexp=false, has_substitution=false;
        string s_pattern, s_replacement;
        string separator_reg;
        bool s_global_flag;
        list<string> result;

        /* initial value selection */
        bool init_regexp=false, init_string=false, init_number=false;
        int init_num=0;
        string init_str;
        string init_reg;

        /*  initial value */
        if (im->parameter_type(0,T_NUMERAL))
          { init_number=true; init_num=im->get_int_parameter(0); }
        if (im->parameter_type(0,T_REGEXP))
          { init_regexp=true; init_reg=im->get_string_parameter(0); }
        if (im->parameter_type(0,T_STRING))
          { init_string=true; init_str=im->get_string_parameter(0); }

        int i=1, item_inserted=0, pn=im->get_parameter_num();
        bool init_selected=false, has_header=false;
        int max_col_num=0;
        Q3ListViewItem * current_item=0;

        /* optional first regexp */
        if (im->parameter_type(i, T_REGEXP))
          {
            has_regexp=true;
            separator_reg=im->get_string_parameter(i);
            i++;
          }

        for (; i<=pn; i++) {
          /* process multiline strings like for @list */
          if (im->parameter_type(i,T_SUBST))
            {
              /* update substitution */
              has_substitution=true;
              im->get_subst_parameter(i, s_pattern, s_replacement, 
                                      s_global_flag);
            }
          if (im->parameter_type(i,T_STRING))
            {
              list<string> lines;
              Regexp::split('\n', im->get_string_parameter(i), lines);
              list<string>::iterator it;
              for (it=lines.begin();
                   it!=lines.end();
                   it++)
                {
                  string original_line=(*it), 
                    modified_line=original_line;

                  /** substitution **/
                  if (has_substitution)
                    Regexp::substitute(s_pattern, original_line,
                                       s_replacement, modified_line, 
                                       s_global_flag);
                  
                  /** separation **/
                  result.clear();
                  if (has_regexp) // separate using regexp
                    {
                      if (!Regexp::matches
                          (separator_reg, modified_line, result))
                        result.push_back(modified_line);
                    }
                  else // separate at tabs
                    {
                      Regexp::split('\t', modified_line, result);
                    }

                  if (has_header)
                    current_item=new Q3ListViewItem(list_view,current_item);


                  /** inserting the fields **/
                  list<string>::iterator si;
                  int col=0;
                  for (si=result.begin();
                       si!=result.end();
                       si++, col++)
                    if (has_header)
                      {
                        if (max_col_num==col)
                          {
                            max_col_num++;
                            list_view->addColumn("");
                          }
                        current_item->setText(col,(*si).c_str());
                      }
                    else
                      {
                        /* create header field */
                        list_view->addColumn((*si).c_str());
                        max_col_num++;
                      }

                  /** selection **/
                  result.clear();
                  if (!init_selected && has_header)
                    {
                      /* select the line 
                         - equal to a string 
                         - matching a regexp
                         - equal to an integer
                      */
                      if ((init_string && init_str.c_str()==(*it)) ||
                          (init_number && init_num==item_inserted+1) ||
                          (init_regexp && Regexp::matches
                           (string(init_reg), 
                            string(original_line), 
                            result)))
                        {
                          init_selected=true; // select first choice
                          list_view->setCurrentItem(current_item);
                        }
                    }
                  if (has_header)
                    item_inserted++;

                  has_header=true; //only the first time
                }
            }
          }
        
      }
      break;

    case 29: // @line
      {
        again=myself;
        if (!again)
          {
            Q3Frame * line=new Q3Frame(parent_widget);
            line->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );
            myself=line;
          }
      }
      break;

    default: // fallback to @integer
      again=spin_box;
      if (!again)
        {
          spin_box=new QSpinBox(parent_widget);
          myself=spin_box;
        }
    }

}


void Kaptain::set_info(QWidget * w)
{
  if (w)
    {
      if (!tooltip.isEmpty())
        QToolTip::add(w, tooltip);
      if (!whatsthis.isEmpty())
        Q3WhatsThis::add(w, whatsthis);
    }
}


/* ---------- RADIO BUTTONS ---------- */
bool Kaptain::radio_is_selected()
{
  bool is=false;
  if (radio_button)
    is=radio_button->isChecked();
  else 
    if (im->is(is_radio) && list_view_item)
      is=((Q3CheckListItem *)list_view_item)->isOn();
  return is; 
}


void Kaptain::radio_select(bool on)
{
  if (radio_button)
      radio_button->setChecked(on);
  else 
    if (im->is(is_radio) && list_view_item)
      {
        ((Q3CheckListItem *)list_view_item)->setOn(on);
      }

  if (on)
	 im->send("selected");
}


void Kaptain::radio_clicked()
{
  /* deselect others */
  Kaptain * brother;
  for (brother=parent->children.first();
       brother!=0;
       brother=parent->children.next())
    if (brother)
      {
        brother->radio_select(false);
      }
  /* select this */
  radio_select(true);
}


int Kaptain::get_check_selected()
{
  int is=0;
  if (checkbox)
    if (checkbox->isTristate())
      switch (checkbox->state())
        {
        case QCheckBox::Off:
          is=2;
          break;
        case QCheckBox::NoChange:
          is=1;
          break;
        case QCheckBox::On:
          is=0;
          break;
        default:
          break;
        }
    else  
      is=checkbox->isChecked() ? 0 : 1;
  else if (im->is(is_checkbox) && list_view_item)
    is=((Q3CheckListItem *)list_view_item)->isOn() ? 0 : 1;
  return is;
}


string Kaptain::evaluate_parameter(int i, bool consider_noeval)
{
  string eval;
  if (im->parameter_type(i,T_NONTERM))
    {
      int nt=im->get_int_parameter(i);
      eval=im->evaluate_nonterminal(nt, consider_noeval);
    }
  else if (im->parameter_type(i,T_STRING))
    {
      eval=im->get_string_parameter(i);
    }

  return eval;
}


/* Ask topmost Kaptain to quit */
void Kaptain::quit()
{
  Kaptain * p=this;
  /* fing topmost node */
  while (p->parent) p=p->parent;
  /* tell him to stop */
  connect(this, SIGNAL(close()), p->parent_dialog, SLOT(accept()));
  emit close();
}


/* Close current dialog */
void Kaptain::close_dialog()
{
  /* tell him to stop */
  connect(this, SIGNAL(close()), parent_dialog, SLOT(accept()));
  emit close();
}


/* evaluate each nonterminal parameter, collect into string list */
void Kaptain::generate_arglist(list<string> & arglist, bool need_shell,
                               bool consider_noeval)
{
  int pn=im->get_parameter_num();
  // create command line
  if (pn==1 && need_shell) // calling bash
    { arglist.push_back("/bin/sh"); arglist.push_back("-c"); }
  for (int i=1; i<=pn; i++) 
    if (im->parameter_type(i,T_STRING)) // append string
      arglist.push_back(im->get_string_parameter(i));
    else if (im->parameter_type(i, T_NONTERM))
      arglist.push_back // append generated text
        (im->evaluate_nonterminal(im->get_int_parameter(i), consider_noeval));
}


/* ---------- BUTTONS ---------- */
void Kaptain::button_pressed()
{
  int spec=im->spec_type;
  switch (spec)
    {
    case 5: // @button
      im->send("pressed");
      break;

    case 12: // @close
      close_dialog();
      break;

    case 13: // @action
    case 16: // @exec
    case 20: // @execclose
    case 26: // @fork
      {
        list<string> arglist;
        if (global_test)
          {
            /* just print */
            generate_arglist(arglist, false, true);
            copy(arglist.begin(), arglist.end(), 
                 ostream_iterator<string>(cout));
            cout << endl;
          } 
        else
          { 
            generate_arglist(arglist, true, true); 
            exec_async(arglist);
          }
        if (spec==16) // @exec
          quit();
        else if (spec==20) // @execclose
          close_dialog();
      }
      // FIXME @action is synchronous!
      
      break;

    case 17: // @echo
    case 22: // @dump
      {
        list<string> arglist;
        generate_arglist(arglist, false, true);
        copy(arglist.begin(), arglist.end(), ostream_iterator<string>(cout));
        cout << endl;
        if (spec==22) // @dump
          quit();
      }
      break;

    case 21: // @execbuffer
      {
        // FIXME blocking gui!
        list<string> arglist;
        generate_arglist(arglist, true, true);
        buffer_output=exec_sync_stdout(arglist);
      }
      break;

    case 23: // @preview
		{
		  QDialog * view=new QDialog(parent_dialog,0,true);
		  Q3BoxLayout * lay=new Q3VBoxLayout(view, BORDER, SPACING);
		  Q3TextView * text_view=new Q3TextView(view);
		  QPushButton * button=new QPushButton(view);
		  lay->addWidget(text_view);
		  lay->addWidget(button);
		  connect(button, SIGNAL(clicked()),view, SLOT(accept()));
		  //connect(text_view, SIGNAL(), view, SLOT(accept()));
		  //button->setDefault(TRUE);

        /* init value */
        if (im->parameter_type(0,T_STRING))
          view->setCaption(im->get_string_parameter(0).c_str());
        /* process parameters */
        int param=2;
        /* font, size parameters */
        if (im->parameter_type(param,T_STRING)) {
          string font_name=im->get_string_parameter(param);
          QFont font(font_name.c_str());
          if (font_name[0]=='-')
            font.setRawName(font_name.c_str());
          text_view->setFont(font);
          param++;
        }
        /* window size */
        if (im->parameter_type(param,T_NUMERAL)) { 
			 text_view->setMinimumWidth(im->get_int_parameter(param)); 
			 param++; 
		  }
        if (im->parameter_type(param,T_NUMERAL)) { 
			 text_view->setMinimumHeight(im->get_int_parameter(param)); 
			 param++; 
		  }
        if (im->parameter_type(param,T_STRING))
			 button->setText(im->get_string_parameter(param).c_str());
		  else
			 button->setText("OK");
		  button->setFixedSize(button->sizeHint());

		  if (im->parameter_type(1,T_NONTERM))
			 text_view->setText(im->evaluate_nonterminal
									  (im->get_int_parameter(1), true).c_str());
		  view->exec();
		  delete lay;
		  delete view;		  
		}
      break;

    case 7: // @infile
    case 8: // @outfile
    case 9: // @directory
      {
        QString filter, initial, result;
        if (im->parameter_type(1,T_STRING))
          filter=im->get_string_parameter(1).c_str();
        initial=line_edit->text();
        if (spec==7)
          result=Q3FileDialog::getOpenFileName(initial, filter, parent_widget);
        if (spec==8)
          result=Q3FileDialog::getSaveFileName(initial, filter, parent_widget);
        if (spec==9)
          result=Q3FileDialog::getExistingDirectory(initial, parent_widget);
        if (!result.isEmpty())
          line_edit->setText(result);       
      }
      break;
    default:
      ;
    }
}


/* ---------- CONTAINER ---------- */
void Kaptain::container_add()
{
  /* add one line to the continer */
  if (container_source) 
    {
      string s=im->evaluate_nonterminal(container_source, 
                                        false);
      if (!s.empty())
        list_box->insertItem(s.c_str());
    }
  else
    list_box->insertItem("NO_SOURCE");
}


void Kaptain::container_remove()
{
  /* remove selected line */
  if (list_box->currentItem()!=-1)
    list_box->removeItem(list_box->currentItem());
}


void Kaptain::subdialog_clicked()
{
  if (child_dialog)
    {
      child_dialog->exec();
    }
}

string Kaptain::evaluate_special()
{
  string s;
  int spec=im->spec_type;
  /* creates a widget and sets myself to point to it */
  switch (spec)
    {    
    case 1: // @integer
      {
        ostringstream ss;
        ss << spin_box->value();
        s=ss.str();
      }     
      break;

    case 2: // @string
    case 4: // @float
    case 6: // @regex
    case 7: // @infile
    case 8: // @outfile
    case 9: // @directory
    case 25: // @password
      s=line_edit->text().latin1();
      break;

    case 3: // @list
    case 10: // @combo
      {
        int n=0;
        if (spec==3) // @list
          n=list_box->currentItem();
        else if (spec==10) // @combo
          n=combo_box->currentItem();

        int items_inserted=0;
        /* find nth line */
        int pn=im->get_parameter_num();
        for (int i=1; i<=pn; i++) {
          if (im->parameter_type(i,T_STRING))
            {
              list<string> lines;
              Regexp::split('\n', im->get_string_parameter(i), lines);
              list<string>::iterator it;
              for (it=lines.begin();
                   it!=lines.end();
                   it++)
                {
                  if (items_inserted==n)
                    s=(*it);
                  items_inserted++;
                }
            }
        }
      }
      break;

    case 5: // @button
    case 12: // @close
    case 13: // @action
    case 16: // @exec
    case 17: // @echo
    case 20: // @execclose
    case 22: // @dump
    case 23: // @preview
    case 26: // @fork
      break;

    case 21: // @execbuffer
      s=buffer_output;
      break;

    case 15: // @combow --> @list
      s=combo_box->currentText().latin1();
      break;
        
    case 11: // @container
      for (uint i=0; i<list_box->count(); i++)
        s+=list_box->text(i).latin1();
      break;

    case 14: // @edit
      s=edit->text().latin1();
      break;

    case 18: // @icon
    case 19: // @text
    case 24: // @size
    case 27: // @fill
      break;

    case 28: // @multicol
      {
		  Q3ListViewItem * p = list_view->firstChild();
        Q3ListViewItem * current=list_view->currentItem();
		  int n = 0;
		  while (p) 
			 {
				if (p == current) break;
				p = p->nextSibling();
				n++;
			 }
		  if (p)
			 {
				n++; // because the first string is the header

				int items_inserted=0;
				/* find nth line */
				int pn=im->get_parameter_num();
				for (int i=1; i<=pn; i++) {
				  if (im->parameter_type(i,T_STRING))
					 {
						list<string> lines;
						Regexp::split('\n', im->get_string_parameter(i), lines);
						list<string>::iterator it;
						for (it=lines.begin();
							  it!=lines.end();
							  it++)
						  {
							 if (items_inserted==n)
								s=(*it);
							 items_inserted++;
						  }
					 }
				}
			 }
      }
      break;

    case 29: // @line
      break;

    default:
      break;
    }
  return s;
}


/* -------------------- message handlers -------------------- */


void Kaptain::message_checkbox(int state)
{
  if (state==2)
	 im->send("on");
  else if (state==1)
	 im->send("no-change");
  else if (state==0)
	 im->send("off");
}


void Kaptain::message_listbox(const QString & s)
{
  if (im)
	 im->send(s.latin1());
}
