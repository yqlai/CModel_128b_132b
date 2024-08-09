# include "headers/utils.h"
# include "headers/MSA.h"
# include "headers/SDP.h"
# include "headers/VDP.h"
# include "headers/BSP.h"

enum PacketType
{
    PACKET_TYPE_UNKNOWN,
    PACKET_TYPE_MSA,
    PACKET_TYPE_BSP,
    PACKET_TYPE_VDP,
    PACKET_TYPE_SDP
};

void parse(char *line, uint8_t *bytes, int idx)
{
    char *token = strtok(line, " ");
    token = strtok(NULL, " ");
    for(int i=0 ; i<4 ; i++)
    {
        token = strtok(NULL, " ");
        bytes[idx+i] = strtol(token, NULL, 16);
    }
}

int LANE_NUMBER = 0;
int write_file_ind = 0;
FILE *input_file = NULL;
FILE *output_file = NULL;

int main(int argc, char *argv[])
{
    const char *filename = argc == 1 ? "sample.txt" : argv[1];
    const char *inputfolder = "data/input/";
    const char *outputfolder = "data/output/";
    char inputfilepath[100];
    char outputfilepath[100];

    sprintf(inputfilepath, "%s%s", inputfolder, filename);
    sprintf(outputfilepath, "%s%s", outputfolder, filename);

    extern FILE *input_file;
    input_file = fopen(inputfilepath, "r");
    if(input_file == NULL)
    {
        fprintf(stderr, "Error reading input file.\n");
        exit(1);
    }

    extern FILE *output_file;
    output_file = fopen(outputfilepath, "w");
    if(output_file == NULL)
    {
        fprintf(stderr, "Error opening output file.\n");
        exit(1);
    }

    extern int LANE_NUMBER;
    fscanf(input_file, "%d", &LANE_NUMBER);

    char line[100];
    uint32_t Bytes[4], valid_byte, header_byte;
    int last_PDF = 0; // indicate the previous packet type
    while(fscanf(input_file, "%2X %2X %2X %2X %2X %2X\n", &valid_byte, &header_byte, &Bytes[0], &Bytes[1], &Bytes[2], &Bytes[3]) != EOF)
    {
        uint8_t data[4] = {Bytes[0], Bytes[1], Bytes[2], Bytes[3]};
        if(header_byte == 0)
        {
            fprintf(stderr, "Error parsing. Maybe get the wrong length.\n");
            printf("Data: %2x %2x %2x %2x\n", data[0], data[1], data[2], data[3]);
            return 1;
        }

        struct USB4_Header usb4_header = parse_USB4_Header(DoubleWord2uint32(data));
        printf("------USB4 Header------\n");
        printf("Data: %2x %2x %2x %2x\n", data[0], data[1], data[2], data[3]); 
        printf("PDF: %d\n", usb4_header.PDF);
        printf("Length: %d\n", usb4_header.Length);
        printf("-----------------------\n\n");
        
        uint8_t *payload_header = (uint8_t *)malloc(4 * sizeof(uint8_t));
        uint8_t *payload_data = (uint8_t *)malloc((usb4_header.Length - 4) * sizeof(uint8_t));
        uint8_t dummy;

        int fillcount;
        enum Control_Symbol k[1];
        
        switch(usb4_header.PDF)
        {
            case PDF_MAIN_STREAM_ATTRIBUTE_PACKET:
                fillcount = get_Fill_Count();
                k[1] = SF;
                for(int i=0 ; i<fillcount ; i++)
                    transform_k(k, 1);

                fgets(line, 100, input_file);
                parse(line, payload_header, 0);

                for(int i=0 ; i<(usb4_header.Length / 4) - 1 ; i++)
                {
                    fgets(line, 100, input_file);
                    parse(line, payload_data, i*4);
                }

                transform_MSA(payload_header, payload_data);

                break;


            case PDF_BLANK_START_PACKET:
                fillcount = get_Fill_Count();
                k[1] = SF;
                for(int i=0 ; i<fillcount ; i++)
                    transform_k(k, 1);

                fgets(line, 100, input_file);
                parse(line, payload_header, 0);

                for(int i=0 ; i<(usb4_header.Length / 4) - 1 ; i++)
                {
                    fgets(line, 100, input_file);
                    parse(line, payload_data, i*4);
                }
                transform_BSP(payload_header, payload_data);

                break;


            case PDF_VIDEO_DATA_PACKET:
                payload_data = (uint8_t *)malloc(usb4_header.Length * sizeof(uint8_t));
                for(int i=0 ; i<usb4_header.Length / 4 ; i++)
                {
                    fgets(line, 100, input_file);
                    parse(line, payload_data, i*4);
                }
                transform_VDP(payload_data, usb4_header.Length);

                break;


            case PDF_SECONDARY_DATA_PACKET:
                payload_data = (uint8_t *)malloc(usb4_header.Length * sizeof(uint8_t));
                for(int i=0 ; i<usb4_header.Length / 4 ; i++)
                {
                    fgets(line, 100, input_file);
                    parse(line, payload_data, i*4);
                }
                transform_SDP(payload_data, usb4_header.Length, last_PDF);
                
                break;
            
            default:
                fprintf(stderr, "Unknown PDF. Expect get 1 , 2, 3 or 4. But got %d\n", usb4_header.PDF);
                return 1;
        }

        last_PDF = usb4_header.PDF;

        free(payload_header);
        free(payload_data);

    }
    return 0;
}