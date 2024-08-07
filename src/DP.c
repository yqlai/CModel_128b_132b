# include "headers/DP.h"

uint32_t generate_DP_USB4_Header(uint8_t length)
{
    uint8_t PDF = PDF_DATA_PACKET;
    uint8_t SuppID = SUPP_ID_DEFAULT;
    uint8_t Rsvd1 = RESERVED_DEFAULT;
    uint8_t HopID = HOPID_DEFAULT;
    uint8_t HEC = calculateHEC((PDF << 28) | (SuppID << 27) | (Rsvd1 << 23) | (HopID << 16) | (length << 8));
    return (PDF << 28) | (SuppID << 27) | (Rsvd1 << 23) | (HopID << 16) | (length << 8) | HEC;
}

void DP_GEN(int argc, char *argv[], FILE *file) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Number of Data (in DoubleWord)>\n", argv[0]);
        return;
    }
    
    // Construct the USB4 Tunneled Packet header (excluding HEC for now)
    uint8_t Length = strtol(argv[1], NULL, 10) * 4;
    uint32_t header = generate_DP_USB4_Header(Length);
    uint8_t USB4_Header[4] = { (header >> 24) & 0xFF, (header >> 16) & 0xFF, 
                               (header >> 8) & 0xFF, header & 0xFF };

    bytesToHexString(USB4_Header, 4, file, 1);

    // Data Packet Payload
    for(int i=0 ; i < Length ; i++)
    {
        uint8_t dummy[1] = {i*0xFF};
        bytesToHexString(dummy, 1, file, 0);
    }
}
