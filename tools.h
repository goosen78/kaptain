#ifndef TOOLS_HH
#define TOOLS_HH

#include <list>
#include <string>

using namespace std;

/**********/
class Error
{
  list<string> messages;
  bool stop;

public:
  Error();

  void warning(string);
  void error(string);

  bool go_on() { return !stop; }

  void print();
};


/***********/
class Regexp
{
private:

public:
  Regexp();

  static bool matches(string, string, list<string> & );

  static bool substitute_local(string, string, string, 
                               string &, int &);
  static void substitute(string, string, string, 
                         string &, bool);

  static void transliterate(string, string,
                            string, string &);

  static void split(char, string, list<string> &);

  static void join(char, list<string> &, string &);
};


#endif // TOOLS_HH
