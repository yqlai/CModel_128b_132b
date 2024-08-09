# include "headers/utils.h"
# include "headers/SDP.h"

struct SD_TU_Header parse_SD_TU_Header(uint32_t header)
{
    struct SD_TU_Header sdp_header;
    sdp_header.EFC_ND = (header >> 31) & 0x1;
    sdp_header.NSS = (header >> 30) & 0x1;
    sdp_header.NSE = (header >> 29) & 0x1;
    sdp_header.L = (header >> 28) & 0x1;
    sdp_header.Fill_Count = (header >> 14) & 0x3FFF;
    sdp_header.Secondary_Count = (header >> 8) & 0x3F;
    sdp_header.ECC = header & 0xFF;
    return sdp_header;
}

void transform_SDP(uint8_t *payload, int length, int lastPDF)
{
    extern int LANE_NUMBER;
    for(int i=0 ; i<length ;)
    {
        if(length - i <= 4) fprintf(stderr, "Wrong in parsing SDP.\n");

        uint8_t tuheader_t[4] = {payload[i++], payload[i++], payload[i++], payload[i++]};
        struct SD_TU_Header tuheader = parse_SD_TU_Header(DoubleWord2uint32(tuheader_t));
        
        // send Fill Count
        // Need to be completed
        int fillcount = get_Fill_Count();
        enum Control_Symbol k[1] = {SF};
        for(int i=0 ; i<fillcount ; i++)
            transform_k(k, 1);

        // Vefify if it is the very beginning of the Secondary Data
        if (lastPDF == PDF_SECONDARY_DATA_PACKET && tuheader.NSS == 0)
        {
            enum Control_Symbol k[1] = {SS};
            transform_k(k, 1);
        }

        // send Secondary Data
        write_data_to_file(payload+4, tuheader.Secondary_Count * LANE_NUMBER);
        i += ((tuheader.Secondary_Count) * LANE_NUMBER + 3) & ~3;

        if (tuheader.L == 1 && tuheader.NSE == 0)
        {
            enum Control_Symbol k[1] = {SE};
            transform_k(k, 1);
        }
    }
}