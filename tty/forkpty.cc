#include "forkpty.hh"
#include <cstdlib>
#include <fcntl.h>
#include <pty.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

void ForkPTY::Open(std::size_t w, std::size_t h) {
  struct winsize ws = {};
  ws.ws_col = w;
  ws.ws_row = h;
  pid = forkpty(&fd, NULL, NULL, &ws);
  if (!pid) {
    static char termstr[] = "TERM=linux";
    putenv(termstr);
    execl(getenv("SHELL"), getenv("SHELL"), "-i", "-l", nullptr);
  }
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int ForkPTY::Send(std::string_view buffer) {
  return write(fd, buffer.data(), buffer.size());
}

std::pair<std::string, int> ForkPTY::Recv() {
  char buffer[4096];
  std::pair<std::string, int> result;

  result.second = read(fd, buffer, sizeof(buffer));
  if (result.second > 0)
    result.first.assign(buffer, buffer + result.second);

  return result;
}

void ForkPTY::Kill(int signal) { kill(pid, signal); }

void ForkPTY::Resize(unsigned xsize, unsigned ysize) {
  struct winsize ws = {};
  ws.ws_col = xsize;
  ws.ws_row = ysize;
  ioctl(fd, TIOCSWINSZ, &ws);
}

void ForkPTY::Close() {
  kill(pid, SIGTERM);
  close(fd);
  waitpid(pid, nullptr, 0);
}
