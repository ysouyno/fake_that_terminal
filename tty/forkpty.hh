#ifndef FORKPTY_H
#define FORKPTY_H

#include <signal.h>
#include <string>

class ForkPTY {
public:
  ForkPTY(std::size_t w, std::size_t h) { Open(w, h); }

  void Open(std::size_t w, std::size_t h);

  int getfd() const { return fd; }

  int Send(std::string_view buffer);
  std::pair<std::string, int> Recv();
  void Kill(int signal);
  void Resize(unsigned xsize, unsigned ysize);
  void Close();

private:
  int fd, pid;
};

#endif /* FORKPTY_H */
