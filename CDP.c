#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define HEADER_SIZE 4
#define NUM_TU_FIELDS 8 // 2 hexadecimal digits per byte

#define HEC_POLY 0x07
#define HEC_INIT 0x00
#define HEC_XOROUT 0x55

uint8_t calculateHEC(uint32_t header){
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
        data[i] = (header >> (24 - i * 8)) & 0xFF;
    }

    uint8_t crc = (uint8_t)HEC_INIT;
    for (int i = 0; i < 3; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ HEC_POLY;
            else
                crc <<= 1;
        }
    }
    crc ^= HEC_XOROUT;
    return crc;
}

/**
 * Adds a TU header to the buffer.
 *
 * @param buffer The buffer to which the TU header will be added.
 * @param index The current index in the buffer.
 * @param count The value of the count field in the TU header.
 * @param data_tu The value of the data TU field in the TU header.
 * @param control_type The value of the control type field in the TU header.
 */
void add_tu_header(uint8_t *buffer, int *index, uint8_t count, uint8_t data_tu, uint8_t control_type) {
    // Create a 4-byte TU header where:
    // Reserved[31:16] is 0
    // Count[15:8] is the value passed as 'count'
    // Data TU[7] is the value passed as 'data_tu'
    // Reserved[6:4] is 0
    // Control Type[3:0] is the value passed as 'control_type'
    uint32_t tu_header = (count << 8) | (data_tu << 7) | control_type;

    // Copy TU header to buffer in big endian format
    buffer[(*index)++] = (tu_header >> 24) & 0xFF;
    buffer[(*index)++] = (tu_header >> 16) & 0xFF;
    buffer[(*index)++] = (tu_header >> 8) & 0xFF;
    buffer[(*index)++] = tu_header & 0xFF;
}

/**
 * Adds a TU payload to the buffer.
 *
 * This function adds a TU payload to the buffer by filling it with values from 0 to length-1.
 *
 * @param buffer The buffer to which the TU payload will be added.
 * @param index A pointer to the current index in the buffer.
 * @param length The length of the TU payload.
 */
void add_tu_payload(uint8_t *buffer, int *index, uint8_t length)
{
    // Padding is not required since length is multiple of DoubleWords
    for(int i=0 ; i<length ; i++)
        buffer[(*index)++] = (i & 0xFF);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <Params of TU 1> <Params of TU 2> ... <Params of TU n>\n", argv[0]);
        printf("If it is a Control TU,  Params should be 0 <Count> <Control Type>.\n");
        printf("If it is a Data TU,     Params should be 1 <Count>.\n");
        return 1;
    }

    // USB4 Tunneled Packet Header Fields
    uint8_t PDF = 0x9;
    uint8_t SuppID = 0x0;
    uint8_t Rsvd1 = 0x0;
    uint8_t HopID = 0x9; // This can be user-defined
    uint8_t Length = 4;


    // Construct the TUs
    int index = HEADER_SIZE; // Start index after the header
    uint8_t packet[248]; // Adjust size if necessary

    for (int i = 1; i < argc;)
    {
        // Check if the TU is Data TU or Control TU
        if (argc - i >= 2)
        {
            uint8_t data_tu = strtol(argv[i], NULL, 10);
            uint8_t count = strtol(argv[i+1], NULL, 10);

            // Control TU
            if (data_tu == 0 && argc - i >= 3)
            {
                uint8_t control_type = strtol(argv[i+2], NULL, 10);
                add_tu_header(packet, &index, count, 0, control_type);
                Length += 4;
                
                i += 3;
            }

            // Data TU
            else if (data_tu == 1)
            {
                add_tu_header(packet, &index, count, 1, 0);
                add_tu_payload(packet, &index, count*4);
                
                Length += (4 + count * 4); // Each count is a Double Words (4 bytes)
                 
                i += 2;
            }

            else {
                printf("Invalid input format.\n");
                return 1;
            }
        } 
        else {
            printf("Invalid input format.\n");
            return 1;
        }
    }

    uint32_t header = (PDF << 28) | (SuppID << 27) | (Rsvd1 << 23) | (HopID << 16) | (Length << 8);
    header = header | calculateHEC(header);
    packet[0] = (header >> 24) & 0xFF;
    packet[1] = (header >> 16) & 0xFF;
    packet[2] = (header >> 8) & 0xFF;
    packet[3] = header & 0xFF;

    // Print the packet in hexadecimal
    printf("Generated USB4 Control and Data Packet:\n");
    for (int j = 0; j < index; ) {
        for(int k = 0; k < 4 && j < index; k++, j++) {
            printf("%02X ", packet[j]);
        }
        printf("\n");
    }

    return 0;
}
