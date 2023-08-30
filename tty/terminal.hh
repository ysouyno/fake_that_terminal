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
  void Ri();

  inline void csi_J(unsigned c) const;
  void csi_K(unsigned c) const;

  void csi_L(int c) const {
    if (c == 0)
      c = 1;
    yscroll_down(cy, bottom, c);
  }

  inline void csi_M(int c) const {
    if (c == 0)
      c = 1;
    yscroll_up(cy, bottom, c);
  }

  inline void csi_P(unsigned c) const;
  inline void csi_X(unsigned c) const;
  void csi_at(unsigned c) const;

  void ResetAttr();
  void BuildAttr();

public:
  std::deque<char32_t> OutBuffer;
  int cx, cy;

private:
  Window &wnd;

  int top, bottom;
  signed char intensity, italic, underline, blink, reverse, bold;
  unsigned fgc, bgc;
  char g0set, g1set, activeset, utfmode, translate;
  unsigned utflength;
  unsigned long utfvalue;

  enum {
    ESnormal,
    ESesc,
    ESsquare,
    ESgetpars,
    ESgotpars,
    EShash,
    ESignore,
    ESescnext,
    ESnonstd,
    ESsetG0,
    ESsetG1,
    ESpercent
  } state;

  std::vector<int> par;
  int ques;

  struct backup {
    int cx, cy, i, I, u, b, r, B, f, g, top, bottom;
  } backup;
};

#endif /* TERMINAL_H */
