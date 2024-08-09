# include "headers/utils.h"
# include "headers/VDP.h"

struct VD_TU_Header parse_VD_TU_Header(uint32_t header)
{
    struct VD_TU_Header vd_tu_header;
    vd_tu_header.EOC = (header >> 31) & 0x1;
    vd_tu_header.TU_Type = (header >> 29) & 0x3;
    vd_tu_header.L = (header >> 28) & 0x1;
    vd_tu_header.Fill_Count = (header >> 14) & 0x3FFF;
    vd_tu_header.Video_Count = (header >> 8) & 0x3F;
    vd_tu_header.ECC = header & 0xFF;
    return vd_tu_header;
}

void transform_VDP(uint8_t *payload, int length)
{
    extern int LANE_NUMBER;
    for(int i=0 ; i<length ;)
    {
        if(length - i <= 4) fprintf(stderr, "Wrong in parsing VDP.\n");

        uint8_t tuheader_t[4] = {payload[i++], payload[i++], payload[i++], payload[i++]};
        struct VD_TU_Header tuheader = parse_VD_TU_Header(DoubleWord2uint32(tuheader_t));
        
        // send Fill Count
        // Need to be completed
        int fillcount = get_Fill_Count();
        enum Control_Symbol k[1] = {SF};
        for(int i=0 ; i<fillcount ; i++)
            transform_k(k, 1);

        // send Video Data
        write_data_to_file(payload+4, tuheader.Video_Count * LANE_NUMBER);
        i += ((tuheader.Video_Count) * LANE_NUMBER + 3) & ~3;
    }
}