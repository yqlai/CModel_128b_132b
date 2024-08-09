# include "utils.h"

# ifndef __BSP_H
# define __BSP_H

struct BS_Header
{
    uint8_t SR;
    uint8_t CP;
    uint32_t Fill_Count;
    uint8_t ECC;
};

struct BS_Header parse_BS_Header(uint32_t header);
void transform_BSP(uint8_t*, uint8_t*);

# endif