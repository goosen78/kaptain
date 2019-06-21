#ifndef COMMUNICATION_HH
#define COMMUNICATION_HH

#include <qsocketnotifier.h>
#include <string>

using namespace std;

class Communicator : public QObject
{
  Q_OBJECT
private:
  int fd_read, fd_write;
  QSocketNotifier * reader;
  bool ok;
  bool input, output;
  string message; // buffer

public:
  Communicator();
  ~Communicator();

  bool is_stdin();

  bool setup_stdio();
  bool setup_progio(string);
  bool setup_socket_server(string);
  bool setup_socket_client(string);

  bool setup_stdin();
  bool setup_stdout();
  bool setup_filein(string);
  bool setup_fileout(string);

  void no_input() {input=false;}
  void no_output() {output=false;}

  void send(string);

public slots:
  void data_arrived(int);

};


#endif // COMMUNICATION_HH
