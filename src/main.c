# include "headers/LLCP.h"
# include "headers/CDP.h"
# include "headers/DP.h"

enum PacketType
{
    PACKET_TYPE_UNKNOWN,
    PACKET_TYPE_LLCP,
    PACKET_TYPE_CDP,
    PACKET_TYPE_DP,
};

const char *PACKETTYPES[] = {
    "UNKNOWN",
    "LLCP",
    "CDP",
    "DP"
};

struct Packet
{
    enum PacketType type;
    int argc;
    char **argv;
};

enum PacketType identify_packet_type(const char *str)
{
    if(strcmp(str, "LLCP") == 0) return PACKET_TYPE_LLCP;
    if(strcmp(str, "CDP") == 0) return PACKET_TYPE_CDP;
    if(strcmp(str, "DP") == 0) return PACKET_TYPE_DP;
    return PACKET_TYPE_UNKNOWN;
}

struct Packet parse(const char* input)
{
    struct Packet packet;

    char *inputCopy = strdup(input);

    char *token = strtok(inputCopy, " ");
    packet.type = identify_packet_type(token);
    free(inputCopy);
    inputCopy = strdup(input);

    int argc = 1;
    char *tmp = inputCopy;
    while(*(tmp++) != '\0')
        if(*tmp == ' ') argc++;


    packet.argv = (char**)malloc((argc+1) * sizeof(char*));
    if(packet.argv == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    strcpy(inputCopy, input);
    token = strtok(inputCopy, " ");
    for(int i=0 ; i<argc ; i++)
    {
        packet.argv[i] = strdup(token);
        token = strtok(NULL, " \n");
        if(packet.argv[i] == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    packet.argc = argc;

    free(inputCopy);

    return packet;
}

int is_comment(const char *line)
{
    if(line[0] == '#') return 1;
    if(line[0] == '\n') return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    const char *filename = argc == 1 ? "sample.txt" : argv[1];
    const char *inputfolder = "data/input/";
    const char *outputfolder = "data/output/";
    char inputfilepath[100];
    char outputfilepath[100];

    sprintf(inputfilepath, "%s%s", inputfolder, filename);
    sprintf(outputfilepath, "%s%s", outputfolder, filename);

    FILE *inputfile = fopen(inputfilepath, "r");
    if(inputfile == NULL)
    {
        fprintf(stderr, "Error reading input file.\n");
        exit(1);
    }

    FILE *outputfile = fopen(outputfilepath, "w");
    if(outputfile == NULL)
    {
        fprintf(stderr, "Error opening output file.\n");
        exit(1);
    }

    char line[500];
    while(fgets(line, 500, inputfile))
    {
        if (is_comment(line)) continue;
        struct Packet packet = parse(line);
        printf("Generating %s packet\n", PACKETTYPES[packet.type]);
        switch(packet.type)
        {
            case PACKET_TYPE_LLCP:
                LLCP_GEN(packet.argc, packet.argv, outputfile);
                break;
            case PACKET_TYPE_CDP:
                CDP_GEN(packet.argc, packet.argv, outputfile);
                break;
            case PACKET_TYPE_DP:
                DP_GEN(packet.argc, packet.argv, outputfile);
                break;
            case PACKET_TYPE_UNKNOWN:
                fprintf(stderr, "Unknown packet type\n");
                break;
        }
    }
    return 0;
}