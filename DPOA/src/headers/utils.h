# ifndef __UTILS_H
# define __UTILS_H

# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <string.h>

# define HEC_POLY 0x07
# define HEC_INIT 0x00
# define HEC_XOROUT 0x55
 
# define ECC_POLY 0x07
# define ECC_INIT 0x00
# define ECC_XOROUT 0x00
 
# define PDF_MAIN_STREAM_ATTRIBUTE_PACKET 3
# define PDF_BLANK_START_PACKET 2
# define PDF_SECONDARY_DATA_PACKET 4
# define PDF_VIDEO_DATA_PACKET 1
 
# define HOPID_DEFAULT 9
# define RESERVED_DEFAULT 0
# define SUPP_ID_DEFAULT 0

extern int LANE_NUMBER;
extern int write_file_ind;

extern FILE* input_file;
extern FILE* output_file;

struct USB4_Header
{
    uint8_t PDF;
    uint8_t SuppID;
    uint8_t Reserved;
    uint8_t HopID;
    uint8_t Length;
    uint8_t HEC;
};

enum Control_Symbol
{
    SS, SE, SF,
    BS, BE, BF,
    SR, CP,
    FS, FE
};

uint8_t calculateHEC(uint32_t);
uint8_t calculateECC(uint32_t);

uint32_t DoubleWord2uint32(uint8_t *data);
struct USB4_Header parse_USB4_Header(uint32_t header);
int verify_HEC(uint32_t);
int verify_ECC(uint32_t);

void transform_k(enum Control_Symbol *, int);

void write_k_to_file(enum Control_Symbol *, int);
void write_data_to_file(uint8_t*, int);

int get_Fill_Count();

# endif