#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include "global.h"
#include "tools.h"
#include "communication.h"
#include "grammar.h"

#define NO_MORE_IO "At most one i/o channel is allowed.\n"

extern Grammar * yygrammar;


Communicator::Communicator()
{
  reader=0;
  fd_read=-1;
  fd_write=-1;
  input=true;
  output=true;
  ok=false;
}


Communicator::~Communicator()
{
  if (reader)
    delete reader;
  if (fd_read!=-1) close(fd_read);
  if (fd_write!=-1) close(fd_write);
}


bool Communicator::is_stdin()
{
  return fd_read==STDIN_FILENO;
}


bool Communicator::setup_stdio()
{
  if (fd_read!=-1 || fd_write!=-1) {
	 MAKE_WARNING(NO_MORE_IO);
	 return false;
  }

  setup_stdin();
  setup_stdout();
  return true;
}


bool Communicator::setup_socket_server(string port_str)
{
  if (fd_read!=-1 || fd_write!=-1) {
	 MAKE_WARNING(NO_MORE_IO);
	 return false;
  }

  int port;
  /* determining port number */
  if (sscanf(port_str.c_str(), "%d", &port)!=1)
	 {
		ostringstream message;
		message << "Invalid port number `" 
				  << port_str
				  << "'." << endl;
		MAKE_ERROR(message.str());
		return false;
	 }

  int socket_fd;
  struct sockaddr_in my_addr;
  struct sockaddr_in their_addr;
  int sin_size, yes=1;

  /* Create the socket.  */
  socket_fd = socket (AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1)
	 {
		MAKE_ERROR(string("socket: ")+string(strerror(errno))+string(".\n"));
		return false;
	 }
  
  if (setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) 
	 {
		MAKE_ERROR(string("setsockopt: ")+string(strerror(errno))+string(".\n"));
		return false;
	 }
  
  my_addr.sin_family = AF_INET;         // host byte order
  my_addr.sin_port = htons(port);       // short, network byte order
  my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
  memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

  /* bind */
  if (bind(socket_fd, (struct sockaddr *)&my_addr, 
			  sizeof (struct sockaddr)) == -1)
	 {
		MAKE_ERROR(string("bind: ")+string(strerror(errno))+string(".\n"));
		return false;
	 }

  /* listen */
  if (listen(socket_fd,1) == -1)
	 {
		MAKE_ERROR(string("listen: ")+string(strerror(errno))+string(".\n"));
		return false;
	 }

  /* accept */
  sin_size = sizeof(struct sockaddr_in);
  fd_read=accept(socket_fd, (struct sockaddr *)& their_addr, 
					  (socklen_t *)&sin_size);
  if (fd_read==-1)
	 {
		MAKE_ERROR(string("accept: ")+string(strerror(errno))+string(".\n"));
		return false;
	 }

  fcntl(fd_read, F_SETFL, O_NONBLOCK);

  //cout << inet_ntoa(their_addr.sin_addr) << endl;
  fd_write=fd_read;

  reader=new QSocketNotifier(fd_read, QSocketNotifier::Read);
  connect(reader, SIGNAL(activated(int)), SLOT(data_arrived(int)));
  ok=true;
  return true;
}


bool Communicator::setup_socket_client(string url)
{
  if (fd_read!=-1 || fd_write!=-1) {
	 MAKE_WARNING(NO_MORE_IO);
	 return false;
  }

  unsigned int port;
  size_t separ;
  string host, port_str;
  /* separating host and port */
  separ=url.find(':');
  if (separ==url.npos) 
	 {
		ostringstream message;
		message << "Could not determine server address and port from `" 
				  << url
				  << "' (host:port)." << endl;
		MAKE_ERROR(message.str());
		return false;
	 }
  
  /* determining port number */
  host=url.substr(0,separ);
  port_str=url.substr(separ+1);
  if (sscanf(port_str.c_str(), "%u", &port)!=1)
	 {
		ostringstream message;
		message << "Invalid port number `" 
				  << port_str
				  << "'." << endl;
		MAKE_ERROR(message.str());
		return false;
	 }

  /* connect to the server */
  struct hostent* hostinfo;
  struct sockaddr_in their_addr;

  hostinfo = gethostbyname (host.c_str());
  if (hostinfo == NULL)
	 {
		MAKE_ERROR(string("Couldn't resolve host name `")
					  +host+string("'.\n"));
		return false;
	 }

  fd_read = socket (AF_INET, SOCK_STREAM, 0);
  if (fd_read == -1)
	 {
		MAKE_ERROR(string("socket: ")+string(strerror(errno))+string(".\n"));
		return false;
	 }

  their_addr.sin_family = AF_INET;    // host byte order 
  their_addr.sin_port = htons(port);  // short, network byte order 
  their_addr.sin_addr = *((struct in_addr *)hostinfo->h_addr);
  memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct 

  if (::connect (fd_read, (struct sockaddr *)&their_addr, 
					  sizeof (struct sockaddr)) == -1) 
	 {
		MAKE_ERROR(string("Couldn't connect to `")
					  +url+string("'. ")+string(strerror(errno))+string("\n"));
		return false;
	 }

  fd_write=fd_read;
  fcntl(fd_read, F_SETFL, O_NONBLOCK);
  
  reader=new QSocketNotifier(fd_read, QSocketNotifier::Read);
  connect(reader, SIGNAL(activated(int)), SLOT(data_arrived(int)));
  ok=true;
  return true;
}


bool Communicator::setup_progio(string command)
{
  if (fd_read!=-1 || fd_write!=-1) {
	 MAKE_WARNING(NO_MORE_IO);
	 return false;
  }

  int left[2];
  int right[2];
  pid_t pid;

  pipe (left);
  pipe (right);

  pid = fork ();
  if (pid == (pid_t) 0) 
    {
      close (left[1]);
      close (right[0]);
      dup2 (left[0], STDIN_FILENO);
      dup2 (right[1], STDOUT_FILENO);
      execlp("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
    }
  else 
    {
      close (left[0]);
		close (right[1]);
		fd_read=right[0];
		fd_write=left[1];
    }

  if (fd_read!=-1 && fd_write!=-1)
	 {
		fcntl(fd_read, F_SETFL, O_NONBLOCK);
		reader=new QSocketNotifier(fd_read, QSocketNotifier::Read);
		connect(reader, SIGNAL(activated(int)), SLOT(data_arrived(int)));
		ok=true;
		return true;
	 }
  MAKE_ERROR("Unable to create pipe to `"+command+"': "+strerror(errno)+".\n");
  return false;
}


bool Communicator::setup_stdin()
{
  fd_read=STDIN_FILENO; // stdin
  fcntl(fd_read, F_SETFL, O_NONBLOCK | O_RDONLY );
  reader=new QSocketNotifier(fd_read, QSocketNotifier::Read);
  connect(reader, SIGNAL(activated(int)), SLOT(data_arrived(int)));
  return true;
}


bool Communicator::setup_stdout()
{
  fd_write=STDOUT_FILENO;
  ok=true;
  return true;
}


bool Communicator::setup_filein(string)
{
  return false;
}


bool Communicator::setup_fileout(string)
{
  return false;
}


void Communicator::send(string message)
{
  if (ok && output)
	 {
		int res=write(fd_write, message.c_str(), message.size());
		if (res==-1)
		  {
			 cerr << (string("Error: Unable to send data: ")
						 +string(strerror(errno))+string(".\n"));
			 exit(-1);
			 close(fd_write);
			 ok=false;
		  }
	 }
}


void Communicator::data_arrived(int)
{
#define BUFF_SIZE 255
  char buff[BUFF_SIZE+1];
  ssize_t n;
  do
    {
      n=read(fd_read, buff, BUFF_SIZE);
		if (n==-1)
		  {
			 break;
		  }
		buff[n]=0;
      message+=string(buff);
    }
  while (n==BUFF_SIZE);

  size_t line;
  while ( (line=message.find('\n'))!=message.npos)
	 {
		string first_line=message.substr(0,line+1);
		if (input)
		  yygrammar->recieve(first_line);
		message=message.substr(line+1);

	 }
}
