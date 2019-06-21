#ifndef PROCESS_HH
#define PROCESS_HH

#include <list>
#include <string>

using namespace std;

void clean_up_child_process(int);
void POSIX_init();
int exec_async(list<string> &);
string exec_sync_stdout(string &);
string exec_sync_stdout(list<string> &);

#endif // PROCESS_HH
