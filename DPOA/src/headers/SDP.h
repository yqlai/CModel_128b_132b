# include "utils.h"

# ifndef __SDP_H
# define __SDP_H

struct SD_TU_Header
{
    uint8_t EFC_ND;
    uint8_t NSS;
    uint8_t NSE;
    uint8_t L;
    uint32_t Fill_Count;
    uint8_t Secondary_Count;
    uint8_t ECC;
};

struct SD_TU_Header parse_SD_TU_Header(uint32_t);
void transform_SDP(uint8_t*, int, int);

# endif