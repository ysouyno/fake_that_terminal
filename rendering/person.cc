#include "256color.hh"
#include "color.hh"
#include <chrono>
#include <cstring>

static constexpr char persondata[] = "                      #####     "
                                     "      ######         #'''''###  "
                                     "     #''''''##      #'''''''''# "
                                     "    #'''''''''#     ###'.#.###  "
                                     "    ###..#.###     #..##.#....# "
                                     "   #..##.#....#    #..##..#...# "
                                     "   #..##..#...#     ##...#####  "
                                     "    ##...#####      ###.....#   "
                                     "     ##.....#     ##'''##''###  "
                                     "    #''##''#     #..''''##''#'# "
                                     "   #''''##''#    #..'''######'.#"
                                     "   #''''#####     #..####.##.#.#"
                                     "    #...##.##     .#########''# "
                                     "    #..'''###     #''######'''# "
                                     "     #'''''#      #'''#  #'''#  "
                                     "      #####        ###    ###   ";
static constexpr unsigned xcoordinates[2] = {0, 16};
static constexpr unsigned person_width = 16;
static constexpr unsigned data_width = 32, data_lines = 16;

static auto start_time = std::chrono::system_clock::now();
static double walk_speed = 64.0; // pixels per second

static int PersonBaseX(unsigned window_width) {
  auto now_time = std::chrono::system_clock::now();

  // X coordinate where The Person is
  std::chrono::duration<double> time_elapsed = (now_time - start_time);
  unsigned walkway_width =
      window_width + std::max(person_width, window_width / 5) + person_width;
  return unsigned(time_elapsed.count() * walk_speed) % walkway_width -
         person_width;
}

static int PersonFrame() {
  auto now_time = std::chrono::system_clock::now();
  std::chrono::duration<double> time_elapsed = (now_time - start_time);
  return unsigned(time_elapsed.count() * 6) % 2;
}

struct ColorSlideCache {
  enum { MaxWidth = 3840 };
  const unsigned char *const colors;
  const unsigned short *const color_positions;
  const unsigned color_length;
  unsigned cached_width;
  unsigned short cache_color[MaxWidth];
  unsigned char cache_char[MaxWidth];

  ColorSlideCache(const unsigned char *c, const unsigned short *p, unsigned l)
      : colors(c), color_positions(p), color_length(l), cached_width(0) {}

  void SetWidth(unsigned w) {
    if (w == cached_width)
      return;

    cached_width = w;

    unsigned char first = 0;
    std::memset(cache_char, 0, sizeof(cache_char));
    std::memset(cache_color, 0, sizeof(cache_color));

    for (unsigned x = 0; x < w && x < MaxWidth; ++x) {
      unsigned short cur_position = (((unsigned long)x) << 16u) / w;
      while (first < color_length && color_positions[first] <= cur_position)
        ++first;
      unsigned long next_position = 0;
      unsigned char next_value = 0;
      if (first < color_length) {
        next_position = color_positions[first];
        next_value = colors[first];
      } else {
        next_position = 10000ul;
        next_value = colors[color_length - 1];
      }

      unsigned short prev_position = 0;
      unsigned char prev_value = next_value;
      if (first > 0) {
        prev_position = color_positions[first - 1];
        prev_value = colors[first - 1];
      }

      float position =
          (cur_position - prev_position) / float(next_position - prev_position);
      static const unsigned char chars[2] = {0, 1};
      unsigned char ch = chars[unsigned(position * 2)];
      if (prev_value == next_value || ch == 0) {
        ch = 0;
        next_value = 0;
      }

      cache_char[x] = ch;
      cache_color[x] = prev_value | (next_value << 8u);
    }
  }

  inline void Get(unsigned x, unsigned char &ch, unsigned char &c1,
                  unsigned char &c2) const {
    unsigned short tmp = cache_color[x];
    ch = cache_char[x];
    c1 = tmp;
    c2 = tmp >> 8u;
  }
};

static const unsigned char slide1_colors[21] = {6, 73, 109, 248, 7,  7,  7,
                                                7, 7,  248, 109, 73, 6,  6,
                                                6, 36, 35,  2,   2,  28, 22};

static const unsigned short slide1_positions[21] = {
    0u,     1401u,  3711u,  6302u,  7072u,  8192u,  16384u,
    24576u, 32768u, 33889u, 34659u, 37250u, 39560u, 40960u,
    49152u, 50903u, 53634u, 55944u, 57344u, 59937u, 63981u};

static const unsigned char slide2_colors[35] = {
    248, 7,   249, 250, 251, 252, 188, 253, 254, 255, 15,  230,
    229, 228, 227, 11,  227, 185, 186, 185, 179, 143, 142, 136,
    100, 94,  58,  239, 238, 8,   236, 235, 234, 233, 0};

static const unsigned short slide2_positions[35] = {
    0u,     440u,   1247u,  2126u,  3006u,  3886u,  4839u,  5938u,  6965u,
    8064u,  9750u,  12590u, 15573u, 18029u, 19784u, 21100u, 24890u, 27163u,
    30262u, 35051u, 35694u, 38054u, 40431u, 41156u, 46212u, 46523u, 50413u,
    52303u, 53249u, 54194u, 56294u, 58815u, 61335u, 63856u, 64696u};

static ColorSlideCache slide1(slide1_colors, slide1_positions,
                              sizeof(slide1_colors));

static ColorSlideCache slide2(slide2_colors, slide2_positions,
                              sizeof(slide2_colors));

void PersonTransform(unsigned &bgcolor, unsigned &fgcolor, unsigned width,
                     unsigned x, unsigned y, unsigned action_type) {
  if (bgcolor != 0xACAAAC) {
    // Only transform lines with white (ansi 7) background
    return;
  }

  auto GetSlide = [&](ColorSlideCache &slide) {
    slide.SetWidth(width);
    unsigned char ch, c1, c2;
    slide.Get(x, ch, c1, c2);
    unsigned result = c1;

    if (ch) {
      unsigned pos = (x ^ y) & 1;
      result = pos ? c2 : c1;
    }

    return xterm256table[result];
  };

  if (action_type <= 1)
    bgcolor = GetSlide(slide1);
  if (action_type == 2)
    bgcolor = GetSlide(slide2);

  if (y >= data_lines || action_type != 1) {
    // Don't change mario unless requested
    return;
  }

  unsigned basex = PersonBaseX(width);
  x -= basex;

  // Person outside view?
  if (x >= person_width) {
    return;
  }

  unsigned frame_number = PersonFrame();
  unsigned frame_start = xcoordinates[frame_number];
  char c = persondata[y * data_width + frame_start + x];

  switch (c) {
  case ' ':
    bgcolor = fgcolor = Mix(bgcolor, fgcolor, 15, 1, 16);
    break;
  case '.':
    bgcolor = fgcolor = Mix(bgcolor, 0x000000, 7, 1, 8);
    break;
  case '\'':
    bgcolor = fgcolor = Mix(bgcolor, 0x000000, 6, 2, 8);
    break;
  case '#':
    bgcolor = fgcolor = 0x000000;
    break;
  }
}
