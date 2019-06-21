#include <stdio.h>
#include <sstream>
#include <iostream>
#include "global.h"
#include "tools.h"
#include "communication.h"
#include "process.h"
#include "grammar.h"
#include "intermediate.h"
#include "kaptain.h"
#include <qapplication.h>
#include <qobject.h>
#include <qdialog.h>

//#include "../config.h"
#define PACKAGE "kaptain"
#define VERSION "0.8"

extern FILE * lexerin;
extern int last_correct_lineno, yylineno;
Grammar * yygrammar;
int parserparse();
int yylex();
bool global_debug=false, global_test=false;
int glob_parse_result=0;
list<Parameter *> * glob_param_list=0;
Parameter * glob_init_value=0;
Communicator * glob_communicator=0;
Error * glob_error=0;

class Instance
{
  Grammar * grammar;
  Intermediate * intermediate;
  Kaptain * kaptain;

public:
  Instance();
  ~Instance();

  void init(int, char **);
  void load();
  void transform();
  int run();

  Communicator communicator;
  Error error;

  string filename;
  bool dbg_print_before;
  bool dbg_print_after;
  bool quit;
  bool test;

};

extern int lexerlex (void);

int parserlex() {
  return lexerlex();
}


int main(int argc, char * argv[])
{
  POSIX_init();
  Instance instance;

  QApplication a(argc, argv);

  glob_communicator=&instance.communicator;
  glob_error=&instance.error;

  instance.init(argc,argv);
  instance.load();
  instance.error.print();
  instance.transform();
  int res=instance.run();

  return res;
}


/*---*/

Instance::Instance()
{
  grammar=0;
  intermediate=0;
  kaptain=0;
  dbg_print_before=false;
  dbg_print_after=false;
  quit=false;
}


void STATUS(const char * s)
{
  if (global_debug)
    cout << s << endl;
}

void DONE(void)
{
}

Instance::~Instance()
{
  if (grammar) delete grammar;
  if (intermediate) delete intermediate;
  if (kaptain) delete kaptain;
}


void Instance::init(int argc, char * argv[])
{
  int at=1; // start at first argument

  while (at<argc)
    {
      if 
        /* verbose */
        (strcmp(argv[at],"-V")==0 ||
         strcmp(argv[at],"--verbose")==0 ||
         strcmp(argv[at],"--debug")==0)
        {
          global_debug=true;
          dbg_print_before=true;
          dbg_print_after=true;
        }
      else if 
        /* test */
        (strcmp(argv[at],"--test")==0 ||
         strcmp(argv[at],"-t")==0)
        {
          global_test=true;
        }
      else if 
        /* about */
        (strcmp(argv[at],"--author")==0 ||
         strcmp(argv[at],"--about")==0 ||
         strcmp(argv[at],"--version")==0)
        {
          cout << PACKAGE << " " << VERSION << endl <<
            "Copyright (C) 2000-2004 Zsolt Terék"
            " <terek@users.sourceforge.net>" << endl;
          quit=true;
        }
      else if 
        /* help */
        (strcmp(argv[at],"--help")==0 ||
         strcmp(argv[at],"-h")==0)
        {
          cout << "kaptain is a universal graphical front-end."
               << endl
               << "Usage: kaptain [options] file" 
               << endl << endl;
          quit=true;
        }
      else if 
        /* socket client */
        (strcmp(argv[at],"--client")==0 ||
         strcmp(argv[at],"-c")==0)
        {
			 /* getting next argument */;
			 at++;
			 if (at==argc) 
				MAKE_ERROR("Missing `hostname:port' for client socket.\n");
			 else 
				communicator.setup_socket_client(argv[at]);
        }
      else if 
        /* socket server */
        (strcmp(argv[at],"--server")==0 ||
         strcmp(argv[at],"--port")==0 ||
         strcmp(argv[at],"-s")==0)
        {
			 /* getting next argument */;
			 at++;
			 if (at==argc) 
				MAKE_ERROR("Missing port number for server socket.\n");
			 else 
				communicator.setup_socket_server(argv[at]);
        }
      else if 
        /* pipe to a program */
        (strcmp(argv[at],"--pipe")==0 ||
			strcmp(argv[at],"--prog")==0 ||
         strcmp(argv[at],"-p")==0)
        {
			 /* getting next argument */;
			 at++;
			 if (at==argc) 
				MAKE_ERROR("Missing command name for creatinf pipe.\n");
			 else 
				communicator.setup_progio(argv[at]);
        }
		else if
        (strcmp(argv[at],"--no-input")==0)
        {
			 communicator.no_input();
			 ;
        }
		else if
        (strcmp(argv[at],"--no-output")==0)
        {
			 communicator.no_output();
			 ;
        }
      else if 
        /* file input
        (strcmp(argv[at],"--input")==0 ||
         strcmp(argv[at],"-i")==0)
        {
			 communicator.setup_stdin();
			 ;
		  }
		else if */
        /* standard input/output */
        (strcmp(argv[at],"--stdio")==0)
        {
			 communicator.setup_stdio();
			 ;
        }
      else if (filename.empty())
        /* filename */
        filename=argv[at];
      at++;
    }

}


void Instance::load()
{
  if (quit || !GO_ON()) return;
  grammar=new Grammar();
  yygrammar=grammar; // global variable for the parser
  STATUS("Parsing script...");
  if (strcmp(filename.c_str(),"-")==0 || filename.empty())
	 {
		if (communicator.is_stdin())
		  {
			 MAKE_ERROR("No input file.\n");
			 lexerin=0;
			 return;
		  }
		lexerin=stdin;
	 }
  else 
    lexerin=fopen(filename.c_str(),"rt");
  if (!lexerin)
    {
      /* I/O error */
      ostringstream message;
      if (filename.empty()) filename=string("stdin");
      message << "Can't read input file: " << filename << endl;
      MAKE_ERROR(message.str());
    }
  else
    {
      /* OK */
      if (parserparse()!=0 || glob_parse_result!=0)
        {
          /* parse error */
          ostringstream message;
          extern int lexerlineno;
          message << "Parse error on line " << lexerlineno << endl
                  << "Stuck __after__ the rule on line " 
                  << last_correct_lineno << endl;
          MAKE_ERROR(message.str());
        }
      fclose(lexerin);
    }
  DONE(); 
  grammar->unify();
  grammar->verify();
  return;
}


void Instance::transform()
{
  if (quit || !GO_ON()) return;
  STATUS("Transforming rules...");
  intermediate=new Intermediate(0);
  intermediate->create_from(grammar, grammar->get_start_rule());
  if (dbg_print_before) intermediate->print(0);
  intermediate->transduce();
  if (dbg_print_before && dbg_print_after) cout << "---" << endl;
  if (dbg_print_after) intermediate->print(0);
  DONE();
}


int Instance::run()
{
  if (quit) return 0;
  if (!GO_ON()) return -1;
  STATUS("Done.");
  kaptain=new Kaptain(intermediate, 0, 0, 0, 0);
  qApp->setMainWidget(kaptain->main_dialog());
  return kaptain->main_dialog()->exec();
}
