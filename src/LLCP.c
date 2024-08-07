# include "headers/LLCP.h"

uint32_t generate_LLCP_USB4_Header()
{
    uint8_t PDF = PDF_LINK_LAYER_CONTROL_PACKET;
    uint8_t SuppID = SUPP_ID_DEFAULT;
    uint8_t Rsvd1 = RESERVED_DEFAULT;
    uint8_t HopID = HOPID_DEFAULT; // This can be user-defined
    uint8_t Length = 0x1C;
    uint8_t HEC;
    
    // Construct the USB4 Tunneled Packet header (excluding HEC for now)
    uint32_t header = (PDF << 28) | (SuppID << 27) | (Rsvd1 << 23) | (HopID << 16) | (Length << 8);
    header = header | calculateHEC(header);

    return header;
}

uint32_t* generate_LLCP_Payload(int argc, char **argv)
{
    uint32_t GapCounter     = (uint32_t)strtoul(argv[1], NULL, 10);
    uint8_t AllocatedSlots  = (uint8_t)strtoul(argv[2], NULL, 10);
    uint64_t ECF            = (uint64_t)strtoull(argv[3], NULL, 10);
    uint8_t LLEI            = (uint8_t)strtoul(argv[4], NULL, 10);
    uint8_t HEI             = (uint8_t)strtoul(argv[5], NULL, 10);
    uint8_t ACT             = (uint8_t)strtoul(argv[6], NULL, 10);
    uint16_t LVP_Lane1      = (uint16_t)strtoul(argv[7], NULL, 10);
    uint16_t LVP_Lane2      = (uint16_t)strtoul(argv[8], NULL, 10);
    uint16_t LVP_Lane3      = (uint16_t)strtoul(argv[9], NULL, 10);
    uint16_t LVP_Lane4      = (uint16_t)strtoul(argv[10], NULL, 10);

    uint32_t *LLCP = (uint32_t*)malloc(7 * sizeof(uint32_t));
    uint8_t reserved = RESERVED_DEFAULT;

    LLCP[0] = (reserved << 25) | (GapCounter << 8) | AllocatedSlots;
    LLCP[1] = ((ECF & 0xFFFF) << 16) | ((reserved & 0x3FFF) << 3) | ((LLEI & 0x1) << 2) | ((HEI & 0x1) << 1) | (ACT & 0x1);
    LLCP[2] = (uint32_t)((ECF >> 16) & 0xFFFFFFFF);
    LLCP[3] = (LVP_Lane1 << 16) | (uint32_t)((ECF >> 48) & 0xFFFF);
    LLCP[4] = (LVP_Lane2 << 16) | (uint32_t)((ECF >> 48) & 0xFFFF);
    LLCP[5] = (LVP_Lane3 << 16) | (uint32_t)((ECF >> 48) & 0xFFFF);
    LLCP[6] = (LVP_Lane4 << 16) | (uint32_t)((ECF >> 48) & 0xFFFF);

    return LLCP;
}

void LLCP_GEN(int argc, char *argv[], FILE *file) {
    if (argc != 11) {
        fprintf(stderr, "Usage: %s <GapCounter> <AllocatedSlots> <ECF> <LLEI> <HEI> <ACT> <LVP-Lane1> <LVP-Lane2> <LVP-Lane3> <LVP-Lane4>\n", argv[0]);
        printf("argc = %d\n", argc);
        for(int i=0 ; i<argc ; i++)
            printf("argv[%d] = %s\n", i, argv[i]);
        return;
    }

    // Generate USB4 Header
    uint32_t header = generate_LLCP_USB4_Header();
    uint8_t USB4_Header[4] = { (header >> 24) & 0xFF, (header >> 16) & 0xFF, 
                               (header >> 8) & 0xFF, header & 0xFF };
    
    
    uint32_t *LLCP = generate_LLCP_Payload(argc, argv);
    uint8_t *LLCP_Bytes = (uint8_t*)malloc(28 * sizeof(uint8_t));
    for(int i=0 ; i<7 ; i++)
    {
        LLCP_Bytes[4*i] = (LLCP[i] >> 24) & 0xFF;
        LLCP_Bytes[4*i + 1] = (LLCP[i] >> 16) & 0xFF;
        LLCP_Bytes[4*i + 2] = (LLCP[i] >> 8) & 0xFF;
        LLCP_Bytes[4*i + 3] = LLCP[i] & 0xFF;
    }


    bytesToHexString(USB4_Header, 4, file, 1);
    bytesToHexString(LLCP_Bytes, 28, file, 0);
}
