# include "utils.h"

# ifndef __MSA_H
# define __MSA_H

struct MSA_Header
{
    uint32_t Fill_Count;
    uint8_t ECC;
};

struct MSA_Header parse_MSA_Header(uint32_t);
void transform_MSA(uint8_t*, uint8_t*);

# endif