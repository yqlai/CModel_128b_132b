# include "utils.h"

# ifndef __LLCP_H
# define __LLCP_H

uint32_t generate_LLCP_USB4_Header();
uint32_t* generate_LLCP_Payload(int, char**);
void LLCP_GEN(int, char*[], FILE*);

# endif