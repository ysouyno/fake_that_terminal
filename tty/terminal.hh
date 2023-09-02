#ifndef TERMINAL_H
#define TERMINAL_H

#include "screen.hh"
#include <deque>
#include <string>

class termwindow {
public:
  termwindow(Window &w) : wnd(w) {
    Reset();
    save_cur();
  }

  void EchoBack(std::u32string_view buffer);
  void Write(std::u32string_view s);

private:
  void Reset();

  void save_cur();
  void restore_cur();

  void ScrollFix();
  void FixCoord();
  void yscroll_down(unsigned y1, unsigned y2, int amount) const;
  void yscroll_up(unsigned y1, unsigned y2, int amount) const;

  void Lf();

  void ResetFG();
  void ResetBG();
  void ResetAttr();

public:
  std::deque<char32_t> OutBuffer;
  int cx, cy;

private:
  Window &wnd;

  int top, bottom;
  char g0set, g1set, activeset, utfmode, translate;
  char32_t lastch = U' ';
  unsigned utflength;
  unsigned long utfvalue;

  std::vector<unsigned> p;
  unsigned state = 0;

  struct backup {
    int cx, cy, top, bottom;
    Cell attr;
  } backup;

  std::u32string buf{};
  std::size_t fill_req = 0;
};

#endif /* TERMINAL_H */
