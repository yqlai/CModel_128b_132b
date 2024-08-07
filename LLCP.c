#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Function to calculate HEC
uint8_t calculate_hec(uint32_t header) {
    // Based on the given CRC specification
    uint8_t poly = 0x07;
    uint8_t hec = 0x00;
    uint8_t xor_out = 0x55;
    uint32_t data = header;
    
    for (int i = 31; i >= 8; i--) {
        uint8_t bit = (data >> i) & 1;
        uint8_t c15 = (hec >> 7) & 1;
        hec <<= 1;
        if (bit ^ c15) {
            hec ^= poly;
        }
    }

    hec ^= xor_out;
    return hec;
}

int main(int argc, char *argv[]) {
    if (argc != 11) {
        fprintf(stderr, "Usage: %s <GapCounter> <AllocatedSlots> <ECF> <LLEI> <HEI> <ACT> <LVP-Lane1> <LVP-Lane2> <LVP-Lane3> <LVP-Lane4>\n", argv[0]);
        return 1;
    }

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

    // USB4 Tunneled Packet Header Fields
    uint8_t PDF = 0x8;
    uint8_t SuppID = 0x0;
    uint8_t Rsvd1 = 0x0;
    uint8_t HopID = 0x9; // This can be user-defined
    uint8_t Length = 0x1C;
    uint8_t HEC;
    
    // Construct the USB4 Tunneled Packet header (excluding HEC for now)
    uint32_t header = (PDF << 28) | (SuppID << 27) | (Rsvd1 << 23) | (HopID << 16) | (Length << 8);

    // Calculate HEC
    HEC = calculate_hec(header);

    // Construct the complete header including HEC
    uint8_t USB4_Header[4] = { (header >> 24) & 0xFF, (header >> 16) & 0xFF, 
                               (header >> 8) & 0xFF, (HEC) & 0xFF };

    // Print USB4 Header
    printf("USB4 Tunneled Packet Header:\n");
    for (int i = 0; i < 4; i++) {
        printf("%02X ", USB4_Header[i]);
    }
    printf("\n");

    // Construct the Link Layer Control Packet
    uint32_t LLCP[7];

    LLCP[0] = (Rsvd1 << 25) | (GapCounter << 8) | AllocatedSlots;
    LLCP[1] = ((ECF & 0xFFFF) << 16) | ((Rsvd1 & 0x3FFF) << 3) | ((LLEI & 0x1) << 2) | ((HEI & 0x1) << 1) | (ACT & 0x1);
    LLCP[2] = (uint32_t)((ECF >> 16) & 0xFFFFFFFF);
    LLCP[3] = (LVP_Lane1 << 16) | (uint32_t)((ECF >> 48) & 0xFFFF);
    LLCP[4] = (LVP_Lane2 << 16) | (uint32_t)((ECF >> 48) & 0xFFFF);
    LLCP[5] = (LVP_Lane3 << 16) | (uint32_t)((ECF >> 48) & 0xFFFF);
    LLCP[6] = (LVP_Lane4 << 16) | (uint32_t)((ECF >> 48) & 0xFFFF);

    // Print Link Layer Control Packet
    printf("Link Layer Control Packet:\n");
    for (int i = 0; i < 7; i++) {
        printf("%08X\n", LLCP[i]);
    }
    printf("\n");

    return 0;
}
