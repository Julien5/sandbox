#include "utils.h"

#if defined(DEVHOST)
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

#if !defined(ARDUINO)
#include <stdio.h>
void utils::dump(const unsigned char *data_buffer, const unsigned int length) {
  unsigned char byte;
  unsigned int i, j;
  for(i=0; i < length; i++) {
    byte = data_buffer[i];
    printf("%02x ", data_buffer[i]);  // display byte in hex
    if(((i%16)==15) || (i==length-1)) {
      for(j=0; j < 15-(i%16); j++)
	printf("   ");
      printf("| ");
      for(j=(i-(i%16)); j <= i; j++) {  // display printable bytes from line
	byte = data_buffer[j];
	if((byte > 31) && (byte < 127)) // outside printable char range
	  printf("%c", byte);
	else
	  printf(".");
      }
      printf("\n"); // end of the dump line (each line 16 bytes)
    } // end if
  } // end for
}
#else
void utils::dump(const unsigned char *data_buffer, const unsigned int length) {
  DBG("not implemented\n");
}
#endif


