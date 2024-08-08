# include "config.h"

# ifndef __GEN_H
# define __GEN_H

void write_to_file(int, char**, FILE*);

void GEN_MSA(struct PacketConfig, FILE*);
void GEN_BS(struct PacketConfig, FILE*);
void GEN_VD(struct PacketConfig, FILE*);
void GEN_SD(struct PacketConfig, FILE*);

# endif