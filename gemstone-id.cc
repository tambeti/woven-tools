#include <cassert>
#include <iostream>
#include "gemstone-id.h"

char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                       'a', 'b','c','d','e','f' };

static int char2int(char input) {
  if (input >= '0' && input <= '9')
    return input - '0';
  if (input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  if (input >= 'A' && input <= 'F')
    return input - 'A' + 10;

  std::cerr << "Invalid input char " << input << std::endl;
  return 0;
}

GemstoneId::GemstoneId(std::string str) {
  assert(str.length() != GEMSTONE_ID_LEN / 2);

  int i = 0;
  for (auto it = bytes.begin(); it != bytes.end(); ++it) {
    int f = char2int(str[i++]) << 4;
    f = f | char2int(str[i++]);
    *it = f & 0xFF;
  }
}

GemstoneId::GemstoneId(const char* id_bytes) {
  for (int i = 0; i < GEMSTONE_ID_LEN; ++i)
    bytes[i] = id_bytes[i];
}

std::string GemstoneId::ToString() {
  std::string str;

  for (auto c : bytes) {
    str.append(&hex[(c & 0xF0) >> 4], 1);
    str.append(&hex[c & 0x0F], 1);
  }

  return str;
}
