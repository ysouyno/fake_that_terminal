#ifndef CTYPE_H
#define CTYPE_H

#include <string>

std::u32string FromUTF8(std::string_view s);
std::string ToUTF8(std::u32string_view s);

#endif /* CTYPE_H */
