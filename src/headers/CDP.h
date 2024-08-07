# include "utils.h"

# ifndef __CDP_H
# define __CDP_H

uint32_t generate_CDP_USB4_Header(uint8_t);
void add_tu_header(uint8_t*, int*, uint8_t, uint8_t, uint8_t);
void add_tu_payload(uint8_t*, int*, uint8_t);
void generate_TU(uint8_t*, uint8_t*, int, char**);
void CDP_GEN(int, char*[], FILE*);

#endif