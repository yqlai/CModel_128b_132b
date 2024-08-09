# include "headers/utils.h"
# include "headers/MSA.h"

struct MSA_Header parse_MSA_Header(uint32_t header)
{
    struct MSA_Header msa_header;
    msa_header.Fill_Count = (header >> 8) & 0x1FFFF;
    msa_header.ECC = header & 0xFF;
    return msa_header;
}

void transform_MSA(uint8_t *header, uint8_t *data)
{
    struct MSA_Header msa_header = parse_MSA_Header(DoubleWord2uint32(header));

    enum Control_Symbol k[2] = {SS, SS};
    
    transform_k(k, 2);
    
    write_data_to_file(data, 36);
}