#include "ctype.hh"

std::u32string FromUTF8(std::string_view s) {
  std::u32string result;
  unsigned cache = 0;
  constexpr unsigned bytes_left_shift = 2,
                     bytes_left_mask = (1u << bytes_left_shift) - 1;
  constexpr unsigned con = 0b00000000000000000101010101011011u;

  for (unsigned char c : s) {
    unsigned c0 = (cache & bytes_left_mask);
    unsigned c1 = (((cache >> bytes_left_shift) * 0x40u + (c & 0x3F))
                   << bytes_left_shift) +
                  (c0 - 1);
    unsigned c2 = ((con << ((c >> 4) * 2)) & 0xFFFFFFFFu) >> 30;
    c2 += ((c & (0x70F1F7Fu >> (c2 * 8))) << bytes_left_shift);
    cache = c0 ? c1 : c2;

    if (!(cache & bytes_left_mask)) {
      char32_t c = cache >> bytes_left_shift;
      if (__builtin_expect(result.empty(), false) ||
          __builtin_expect(c < 0xDC00 || c > 0xDFFF, true) ||
          __builtin_expect(result.back() < 0xD800 || result.back() > 0xDBFF,
                           false))
        result += c;
      else
        result.back() =
            (result.back() - 0xD800u) * 0x400u + (c - 0xDC00u) + 0x10000u;
    }
  }

  return result;
}

std::string ToUTF8(std::u32string_view s) {
  std::string result;
  alignas(16) static constexpr unsigned S[4] = {0x7F, 0x3F, 0x3F, 0x3F},
                                        q[4] = {0xFF, 0, 0, 0},
                                        o[4] = {0, 0x80, 0x80, 0x80};

  for (char32_t c : s) {
    unsigned n = (c >= 0x80u) + (c >= 0x800u) + (c >= 0x10000u);
    unsigned val = 0xF0E0C000u >> (n * 8u);
    alignas(16) unsigned w[4] = {val, val, val, val};
#pragma omp simd
    for (unsigned m = 0; m < 4; ++m)
      w[m] = ((w[m] & q[m]) + ((c >> ((n - m) * 6u) & S[m]) + o[m]))
             << (m * 8u);
    unsigned sum = 0;
#pragma omp simd
    for (unsigned m = 0; m < 4; ++m)
      sum += w[m];
    alignas(4) char temp[4];
    for (unsigned m = 0; m < 4; ++m)
      temp[m] = sum >> (m * 8);
    result.append(temp, n + 1);
  }

  return result;
}
