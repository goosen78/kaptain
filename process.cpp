#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "process.h"


/* ---------------------------------------------------------------------- */


void clean_up_child_process(int s)
{
  cout << "Signal: " << s << endl;
  int status;
  wait(&status);
}


void POSIX_init()
{
  signal(SIGCHLD, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);

  /*
  // handle SIGCHLD by calling clean_up_child_process 
  struct sigaction sigchld_action;
  memset(&sigchld_action, 0, sizeof(sigchld_action));
  sigchld_action.sa_handler=&clean_up_child_process;
  sigaction(SIGCHLD, &sigchld_action, NULL);
  // handle SIGPIPE 
  memset(&sigchld_action, 0, sizeof(sigchld_action));
  sigchld_action.sa_handler=&clean_up_child_process;
  sigaction(SIGPIPE, &sigchld_action, NULL);
  */
}


/* asynchronous program execution */
int exec_async(list<string> & arguments)
{
  pid_t child_pid;

  child_pid=fork();
  if (child_pid == 0)
    {
      /* child process */     
      const char ** arglist=new const char *[arguments.size()+1];
      list<string>::iterator si;
      int ind=0;
      for (si=arguments.begin(); si!=arguments.end(); si++)
        arglist[ind++]=(*si).c_str();
      arglist[ind]=0; // NULL terminated (char * array)
      execvp(arglist[0], (char * const *)arglist);
    }

  /* parent process */
  return child_pid;
}

#define READ_BUFFER_SIZE 1000

string exec_sync_stdout(string & command)
{
  string eval;
  char * buffer=new char[READ_BUFFER_SIZE+2];
  FILE * fp;
  fp=popen(command.c_str(), "r");
  if (fp)
    while (!feof(fp))
    {
      if (fgets(buffer, READ_BUFFER_SIZE, fp))
        eval+=buffer;
    }
  pclose(fp);
  return eval;
}


string exec_sync_stdout(list<string> & arguments)
{
  string eval;
  int fds[2];
  pid_t pid;

  /* Create a pipe.  File descriptors for the two ends of the pipe are
     placed in fds.  */
  pipe (fds);
  /* Fork a child process.  */
  pid = fork ();
  if (pid == (pid_t) 0) 
    {
      /* This is the child process.  Close our copy of the read end of
         the file descriptor.  */
      close (fds[0]);
      /* Connect the write end of the pipe to standard input.  */
      dup2 (fds[1], STDOUT_FILENO);
      /* Replace the child process with the given program.  */
      const char ** arglist=new const char *[arguments.size()+1];
      list<string>::iterator si;
      int ind=0;
      for (si=arguments.begin(); si!=arguments.end(); si++)
        arglist[ind++]=(*si).c_str();
      arglist[ind]=0; // NULL terminated (char * array)
      execvp(arglist[0], (char * const *)arglist);
    }
  else 
    {
      /* This is the parent process.  */
      FILE* stream;
      char * buffer=new char[READ_BUFFER_SIZE+2];
      /* Close our copy of the write end of the file descriptor.  */
      close (fds[1]);
      /* Convert the write file descriptor to a FILE object, and write
         to it.  */
      stream = fdopen (fds[0], "r");
      while (!feof(stream))
        {
          if (fgets(buffer, READ_BUFFER_SIZE, stream))
            eval+=buffer;
        }
      close (fds[0]);
      /* Wait for the child process to finish.  */
      waitpid (pid, NULL, 0);
    }
  
  return eval;  
}


