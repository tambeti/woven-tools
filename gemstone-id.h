#ifndef _GEMSTONE_ID_H_
#define _GEMSTONE_ID_H_

#include <array>
#include <string>
#include "constants.h"

class GemstoneId {
 public:
  GemstoneId(std::string str);

  GemstoneId(const char* id_bytes);

  std::array<char, GEMSTONE_ID_LEN> data() const { return bytes; };

  std::string ToString();

 private:
  std::array<char, GEMSTONE_ID_LEN> bytes;
};

inline bool operator==(const GemstoneId& x, const GemstoneId& y) {
  return x.data() == y.data();
}

inline bool operator!=(const GemstoneId& x, const GemstoneId& y) {
  return !(x == y);
}

#endif // _GEMSTONE_ID_H_
