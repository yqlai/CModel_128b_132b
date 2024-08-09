# include "headers/utils.h"

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

uint8_t calculateECC(uint32_t header) {
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
        data[i] = (header >> (24 - i * 8)) & 0xFF;
    }

    uint8_t crc = ECC_INIT;
    for (int i = 0; i < 3; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ ECC_POLY;
            else
                crc <<= 1;
        }
    }
    crc ^= ECC_XOROUT; // XOROUT value is defined as 0x00 for the ECC calculation.
    return crc;
}

uint32_t DoubleWord2uint32(uint8_t *data)
{
    return data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
}

struct USB4_Header parse_USB4_Header(uint32_t header)
{
    struct USB4_Header usb4_header;
    usb4_header.PDF = (header >> 28) & 0xF;
    usb4_header.SuppID = (header >> 27) & 0x01;
    usb4_header.Reserved = (header >> 23) & 0x0F;
    usb4_header.HopID = (header >> 16) & 0x7F;
    usb4_header.Length = (header >> 8) & 0xFF;
    usb4_header.HEC = header & 0xFF;
    return usb4_header;
}

int verify_HEC(uint32_t header)
{
    return calculateHEC(header) == (header & 0xFF);
}

int verify_ECC(uint32_t header)
{
    return calculateECC(header) == (header & 0xFF);
}

char *kstring(enum Control_Symbol k)
{
    switch(k)
    {
        case SS: return "SS";
        case SE: return "SE";
        case BS: return "BS";
        case BE: return "BE";
        case BF: return "BF";
        case SR: return "SR";
        case CP: return "CP";
        case FS: return "FS";
        case FE: return "FE";
        case SF: return "SF";
    }
    return "Unknown";
}

void write_k_to_file(enum Control_Symbol *k, int size)
{
    extern int write_file_ind;

    extern int LANE_NUMBER;
    extern FILE* output_file;

    for(int i=0 ; i<size ; i++)
    {
        fprintf(output_file, "%s ", kstring(k[i]));
        write_file_ind++;
        if(write_file_ind == LANE_NUMBER)
        {
            fprintf(output_file, "\n");
            write_file_ind = 0;
        }
    }
}

void write_data_to_file(uint8_t *data, int size)
{
    extern int write_file_ind;

    extern int LANE_NUMBER;
    extern FILE* output_file;
    
    for(int i=0 ; i<size ; i++)
    {
        fprintf(output_file, "%02X ", data[i]);
        write_file_ind++;
        if(write_file_ind == LANE_NUMBER)
        {
            fprintf(output_file, "\n");
            write_file_ind = 0;
        }
    }
}

void transform_k(enum Control_Symbol *k, int size)
{
    extern int LANE_NUMBER;
    for(int i=0 ; i<size ; i++)
        for(int j=0 ; j<LANE_NUMBER ; j++)
            write_k_to_file(k+i, 1);
}

int get_Fill_Count()
{
    return 3;
}