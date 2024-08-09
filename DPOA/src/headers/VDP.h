# include "utils.h"

# ifndef __VDP_H
# define __VDP_H

struct VD_TU_Header
{
    uint8_t EOC;
    uint8_t TU_Type;
    uint8_t L;
    uint32_t Fill_Count;
    uint32_t Video_Count;
    uint8_t ECC;
};

struct VD_TU_Header parse_VD_TU_Header(uint32_t);
void transform_VDP(uint8_t*, int);

# endif