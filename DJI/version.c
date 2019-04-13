#include "version.h"
FirmWare extendedVersionBase;
const FirmWare M100_31           = 0x03010A00;//Version::FW(3, 1, 10,  0  );
FirmWare
FW(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  return (((a << 24) & 0xff000000) | ((b << 16) & 0x00ff0000) |
          ((c << 8) & 0x0000ff00) | (d & 0x000000ff));
}

