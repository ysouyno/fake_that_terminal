#include "forkpty.hh"
#include <cstdlib>
#include <fcntl.h>
#include <pty.h>
#include <unistd.h>

void ForkPTY::Open(std::size_t w, std::size_t h) {
  struct winsize ws = {};
  ws.ws_row = w;
  ws.ws_col = h;
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
