#pragma once

#include <stdint.h>

#define ILI9341_INIT_DELAY uint8_t(0x80) 

#define ILI9341_NOP uint8_t(0x00)
#define ILI9341_SWRST uint8_t(0x01)

#define ILI9341_CASET uint8_t(0x2A)
#define ILI9341_PASET uint8_t(0x2B)
#define ILI9341_RAMWR uint8_t(0x2C)

#define ILI9341_RAMRD uint8_t(0x2E)
#define ILI9341_IDXRD uint8_t(0xDD) 

#define ILI9341_MADCTL uint8_t(0x36)
#define ILI9341_MAD_MY uint8_t(0x80)
#define ILI9341_MAD_MX uint8_t(0x40)
#define ILI9341_MAD_MV uint8_t(0x20)
#define ILI9341_MAD_ML uint8_t(0x10)
#define ILI9341_MAD_BGR uint8_t(0x08)
#define ILI9341_MAD_MH uint8_t(0x04)
#define ILI9341_MAD_RGB uint8_t(0x00)

#define ILI9341_INVOFF uint8_t(0x20)
#define ILI9341_INVON uint8_t(0x21)

#define ILI9341_NOP uint8_t(0x00)
#define ILI9341_SWRESET uint8_t(0x01)
#define ILI9341_RDDID uint8_t(0x04)
#define ILI9341_RDDST uint8_t(0x09)

#define ILI9341_SLPIN uint8_t(0x10)
#define ILI9341_SLPOUT uint8_t(0x11)
#define ILI9341_PTLON uint8_t(0x12)
#define ILI9341_NORON uint8_t(0x13)

#define ILI9341_RDMODE uint8_t(0x0A)
#define ILI9341_RDMADCTL uint8_t(0x0B)
#define ILI9341_RDPIXFMT uint8_t(0x0C)
#define ILI9341_RDIMGFMT uint8_t(0x0A)
#define ILI9341_RDSELFDIAG uint8_t(0x0F)

#define ILI9341_INVOFF uint8_t(0x20)
#define ILI9341_INVON uint8_t(0x21)
#define ILI9341_GAMMASET uint8_t(0x26)
#define ILI9341_DISPOFF uint8_t(0x28)
#define ILI9341_DISPON uint8_t(0x29)

#define ILI9341_CASET uint8_t(0x2A)
#define ILI9341_PASET uint8_t(0x2B)
#define ILI9341_RAMWR uint8_t(0x2C)
#define ILI9341_RAMRD uint8_t(0x2E)

#define ILI9341_PTLAR uint8_t(0x30)
#define ILI9341_VSCRDEF uint8_t(0x33)
#define ILI9341_MADCTL uint8_t(0x36)
#define ILI9341_VSCRSADD uint8_t(0x37)
#define ILI9341_PIXFMT uint8_t(0x3A)

#define ILI9341_WRDISBV uint8_t(0x51)
#define ILI9341_RDDISBV uint8_t(0x52)
#define ILI9341_WRCTRLD uint8_t(0x53)

#define ILI9341_FRMCTR1 uint8_t(0xB1)
#define ILI9341_FRMCTR2 uint8_t(0xB2)
#define ILI9341_FRMCTR3 uint8_t(0xB3)
#define ILI9341_INVCTR uint8_t(0xB4)
#define ILI9341_DFUNCTR uint8_t(0xB6)

#define ILI9341_PWCTR1 uint8_t(0xC0)
#define ILI9341_PWCTR2 uint8_t(0xC1)
#define ILI9341_PWCTR3 uint8_t(0xC2)
#define ILI9341_PWCTR4 uint8_t(0xC3)
#define ILI9341_PWCTR5 uint8_t(0xC4)
#define ILI9341_VMCTR1 uint8_t(0xC5)
#define ILI9341_VMCTR2 uint8_t(0xC7)

#define ILI9341_RDID4 uint8_t(0xD3)
#define ILI9341_RDINDEX uint8_t(0xD9)
#define ILI9341_RDID1 uint8_t(0xDA)
#define ILI9341_RDID2 uint8_t(0xDB)
#define ILI9341_RDID3 uint8_t(0xDC)
#define ILI9341_RDIDX uint8_t(0xDD) 

#define ILI9341_GMCTRP1 uint8_t(0xE0)
#define ILI9341_GMCTRN1 uint8_t(0xE1)

#define ILI9341_MADCTL_MY uint8_t(0x80)
#define ILI9341_MADCTL_MX uint8_t(0x40)
#define ILI9341_MADCTL_MV uint8_t(0x20)
#define ILI9341_MADCTL_ML uint8_t(0x10)
#define ILI9341_MADCTL_RGB uint8_t(0x00)
#define ILI9341_MADCTL_BGR uint8_t(0x08)
#define ILI9341_MADCTL_MH uint8_t(0x04)
