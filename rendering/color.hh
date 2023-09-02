#ifndef COLOR_H
#define COLOR_H

#include <array>

static std::array<unsigned, 3> Unpack(unsigned rgb) {
  return {rgb >> 16, (rgb >> 8) & 0xFF, rgb & 0xFF};
}

static unsigned Repack(std::array<unsigned, 3> &rgb) {
  if (rgb[0] > 255 || rgb[1] > 255 || rgb[2] > 255) {
    // Clamp with desaturation
    float l = (rgb[0] * 299u + rgb[1] * 587u + rgb[2] * 114u) * 1e-3f;
    float s = 1.f;
    for (unsigned n = 0; n < 3; ++n)
      if (rgb[n] > 255)
        s = std::min(s, (l - 255.f) / (l - rgb[n]));

    if (s != 1.f) { // Compare directly with float type?
      for (unsigned n = 0; n < 3; ++n)
        rgb[n] = (rgb[n] - l) * s + l + 0.5f;
    }
  }

  return (std::min(rgb[0], 255u) << 16) + (std::min(rgb[1], 255u) << 8) +
         (std::min(rgb[2], 255u) << 0);
}

static unsigned Mix(unsigned color1, unsigned color2, unsigned fac1,
                    unsigned fac2, unsigned sum) {
  auto a = Unpack(color1), b = Unpack(color2);
  for (unsigned n = 0; n < 3; ++n)
    a[n] = (b[n] * fac1 + a[n] * fac2) / (sum);
  return Repack(a);
}

#endif /* COLOR_H */
