#ifndef KAPTAIN_HH
#define KAPTAIN_HH

#include <qwidget.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qlist.h>
#include <qcombobox.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <q3listview.h>
#include <qdialog.h>
#include <q3widgetstack.h>
#include <qvalidator.h>
#include <q3multilineedit.h>
#include <q3wizard.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3BoxLayout>
#include <Q3PtrList>
#include <regex.h>
#include <string>
#include <list>

using namespace std;

class Intermediate;


class QRegValidator : public QValidator
{
  QString regexp;
  regex_t * compiled_pattern;
  bool valid;
public:
  QRegValidator(QString, QWidget *parent=0, const char *name=0);
  ~QRegValidator();
  virtual QValidator::State validate(QString &, int &) const;
};


class Kaptain : public QObject
{
  Q_OBJECT
private:
  Intermediate * im;
  Kaptain * parent;

  QWidget * parent_widget;
  QDialog * parent_dialog;
  bool kill_dialog;
  Q3BoxLayout * parent_layout;
  QWidget * myself;

  QRadioButton * radio_button;
  QCheckBox * checkbox;
  QSpinBox * spin_box;
  QLineEdit * line_edit;
  QTabWidget * tab_widget;
  Q3ListBox * list_box;
  QComboBox * combo_box;
  QPushButton * push_button, * push_button2;
  Q3ListView * list_view;
  Q3ListViewItem * list_view_item;
  Q3WidgetStack * widget_stack;
  Q3PtrDict<Kaptain> * mapping; // used in list_views
  QWidget * my_panel;
  int stack_size; // used in treeview stacks
  int container_source; // used in @container
  Q3MultiLineEdit * edit;
  QDialog * child_dialog;
  Q3Wizard * wizard;
  string buffer_output;
  Qt::Alignment alignment;
  int grid_rows, grid_columns;
  Q3GridLayout * grid;

  QString title;
  QString tooltip;
  QString whatsthis;
  bool no_title;
  bool done_title;
  bool no_tree;
  bool ignore_radio;

  Q3PtrList<Kaptain> children;

  void create_frame();
  void create_title();
  void create_minimum_layout();
  void create_sublayout();
  void create_grid_layout();
  void create_sequence();  
  void create_tabbed();
  void create_tree_view();
  void create_subdialog();
  void create_wizard();
  void create_tree_item();
  void create_stack();

  void set_info(QWidget *); // add tooltip & whatsthis information

  Kaptain * get_tree_top();
  void quit();
  void close_dialog();
  void generate_arglist(list<string> &, bool, bool);

  void radio_select(bool);
  void select_defaults();

  string evaluate_parameter(int, bool);

public:
  Kaptain(Intermediate *, Kaptain *, QWidget *, Q3BoxLayout *, QDialog *,
          bool no_title=false, const char *name=0 );

  ~Kaptain();

  void create();
  void create_special();

  bool radio_is_selected();
  int get_check_selected();
  void select_page(int);
  int get_selected_page();
  void select_it(int);


  string evaluate_special();

  QDialog * main_dialog() { return parent_dialog; }

public slots:
  void radio_clicked();
  void panel_selected(Q3ListViewItem *);
  void button_pressed();
  void container_add();
  void container_remove();
  void subdialog_clicked();

  void message_checkbox(int);
  void message_listbox(const QString &);

  signals:
  void close();
};

#endif // KAPTAIN_HH
