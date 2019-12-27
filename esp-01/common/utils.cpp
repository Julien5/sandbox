#ifndef ARDUINO

#include "utils.h"

namespace utils {
  std::vector<uint8_t> hex_to_bytes(const std::string &hex) {
    std::vector<uint8_t> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
      std::string byteString = hex.substr(i, 2);
      uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
      bytes.push_back(byte);
    }
    return bytes;
  }

  uint8_t * as_cbytes(std::vector<uint8_t> &bytes, int *L) {
    if (L)
      *L=bytes.size();
    return reinterpret_cast<uint8_t*>(&bytes[0]);
  }
};

#endif

long long fixed_atoll(char *s) {
  long long result = 0;
  for (int i = 0; s[i] != '\0'; ++i)
    result = result*10 + s[i] - '0';
  return result;
}
