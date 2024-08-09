# include "headers/utils.h"
# include "headers/BSP.h"

struct BS_Header parse_BS_Header(uint32_t header)
{
    struct BS_Header bs_header;
    bs_header.SR = (header >> 31) & 0x1;
    bs_header.CP = (header >> 30) & 0x1;
    bs_header.Fill_Count = (header >> 8) & 0x1FFFF;
    bs_header.ECC = header & 0xFF;
    return bs_header;
}

void transform_BSP(uint8_t *header, uint8_t *data)
{
    struct BS_Header bs_header = parse_BS_Header(DoubleWord2uint32(header));

    enum Control_Symbol k[4];
    switch(bs_header.SR<<1 + bs_header.CP)
    {
        case 0:
            k[0] = BS; k[1] = BF; k[2] = BF; k[3] = BS;
            break;
        case 1:
            k[0] = BS; k[1] = CP; k[2] = CP; k[3] = BS;
            break;
        case 2:
            k[0] = SR; k[1] = BF; k[2] = BF; k[3] = SR;
            break;
        case 3:
            k[0] = SR; k[1] = CP; k[2] = CP; k[3] = SR;
            break;
        default:
            fprintf(stderr, "Invalid SR and CP Number.\n");
            break; 
    }

    transform_k(k, 4);
        
    write_data_to_file(data, 12);
}