# include "headers/CDP.h"

#define HEADER_SIZE 4
#define NUM_TU_FIELDS 8 // 2 hexadecimal digits per byte

uint32_t generate_CDP_USB4_Header(uint8_t Length)
{
    uint8_t PDF = PDF_CONTROL_AND_DATA_PACKET;
    uint8_t SuppID = SUPP_ID_DEFAULT;
    uint8_t Rsvd1 = RESERVED_DEFAULT;
    uint8_t HopID = HOPID_DEFAULT; 

    uint32_t header = (PDF << 28) | (SuppID << 27) | (Rsvd1 << 23) | (HopID << 16) | (Length << 8);
    header = header | calculateHEC(header);

    return header;
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

void generate_TU(uint8_t *packet, uint8_t *Length, int argc, char **argv)
{
    // Construct the TUs
    int index = HEADER_SIZE; // Start index after the header

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
                *Length += 4;
                
                i += 3;
            }

            // Data TU
            else if (data_tu == 1)
            {
                add_tu_header(packet, &index, count, 1, 0);
                add_tu_payload(packet, &index, count*4);
                
                *Length += (4 + count * 4); // Each count is a Double Words (4 bytes)
                 
                i += 2;
            }

            else {
                printf("Invalid input format.\n");
                return;
            }
        } 
        else {
            printf("Invalid input format.\n");
            return;
        }
    }
}

void CDP_GEN(int argc, char *argv[], FILE* file) {
    if (argc < 2) {
        printf("Usage: %s <Params of TU 1> <Params of TU 2> ... <Params of TU n>\n", argv[0]);
        printf("If it is a Control TU,  Params should be 0 <Count> <Control Type>.\n");
        printf("If it is a Data TU,     Params should be 1 <Count>.\n");
        return;
    }

    uint8_t Length = 0;

    // Construct the TUs
    int index = HEADER_SIZE; // Start index after the header
    uint8_t packet[248]; // Adjust size if necessary

    generate_TU(packet, &Length, argc, argv);

    uint32_t header = generate_CDP_USB4_Header(Length);
    packet[0] = (header >> 24) & 0xFF;
    packet[1] = (header >> 16) & 0xFF;
    packet[2] = (header >> 8) & 0xFF;
    packet[3] = header & 0xFF;

    bytesToHexString(packet, HEADER_SIZE, file, 1);
    bytesToHexString(packet + HEADER_SIZE, Length, file, 0);
}