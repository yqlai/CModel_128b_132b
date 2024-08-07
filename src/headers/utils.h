# ifndef __UTILS_H
# define __UTILS_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdint.h>

# define HEC_POLY 0x07
# define HEC_INIT 0x00
# define HEC_XOROUT 0x55

# define ECC_POLY 0x07
# define ECC_INIT 0x00
# define ECC_XOROUT 0x00

# define PDF_LINK_LAYER_CONTROL_PACKET 0x8
# define PDF_CONTROL_AND_DATA_PACKET 0x9
# define PDF_DATA_PACKET 0xA

#define HOPID_DEFAULT 9
#define RESERVED_DEFAULT 0
#define SUPP_ID_DEFAULT 0

uint8_t calculateHEC(uint32_t header);
void bytesToHexString(const unsigned char*, int, FILE* , int);

#endif