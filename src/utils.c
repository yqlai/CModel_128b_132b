# include "headers/utils.h"

// Function to calculate HEC
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

// Utility function to convert byte array back to hex string
void bytesToHexString(const unsigned char* byteArray, int length, FILE* file, int isHeader) {
    static int ind_SDP = 0;
    for (int i = 0; i < length; i++) {
        if(file)
        {
            if (ind_SDP % 4 == 0)
            {
                fprintf(file, "  %02X", 1);
                fprintf(file, "  %02X", isHeader);
            }

            fprintf(file, "  %02X", byteArray[i]);
            ind_SDP++;

            if (ind_SDP % 4 == 0)
                fprintf(file, "\n");
        }
    }
    if (isHeader == 1) ind_SDP = 0;
}