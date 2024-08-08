# include "headers/config.h"

int iscommand(char *line)
{
    if (line[0] == '\n') return 1;
    return 0;
}

enum ConfigType identify_type(char *line)
{
    if (iscommand(line)) return COMMAND;
    if (line[0] == '[') return PACKET_NAME;
    return FIELD_SETTING;
}

int is_valid_config_value(char *line)
{
    if (line[0] != '{') return 0;
    if (line[strlen(line)-1] != '}' && (line[strlen(line)-1] == '\n' && line[strlen(line)-2] != '}')) return 0;
    return 1;
}

void get_available_array(int **available_t, int *length, char *line)
{
    if (is_valid_config_value(line) == 0)
    {
        fprintf(stderr, "Invalid config value\n");
        exit(1);
    }

    int tmpind = 0;
    while(line[++tmpind] != '}');
    line[tmpind] = '\0';
    line = line + 1;

    int *available;
    // if ':' is in line, then it is a range
    if (strchr(line, ':') != NULL)
    {
        char *token = strtok(line, ":");
        int start = (int)strtol(token, NULL, 10);
        token = strtok(NULL, ":");
        int end = (int)strtol(token, NULL, 10);
        (*length) = end - start + 1;

        available = (int*)malloc((end-start+1)*sizeof(int));
        int tmpind = 0;
        for (int i = start; i <= end; i++)
        {
            available[tmpind++] = i;
        }
    }
    else
    {
        char *copystring = strdup(line);
        char *token = strtok(copystring, ",");
        (*length) = 0;
        while(token != NULL)
        {
            token = strtok(NULL, ",");
            (*length)++;
        }
        
        copystring = strdup(line);
        token = strtok(copystring, ",");
        available = (int*)malloc((*length) * sizeof(int));

        int i=0;
        while(token != NULL)
        {
            available[i++] = strtol(token, NULL, 10);

            token = strtok(NULL, ",");
        }
    }
    (*available_t) = available;
}

struct PacketConfig* get_configs()
{
    const char *filename = "config.txt";
    const char *folder = "../data/";
    char filepath[100];
    sprintf(filepath, "%s%s", folder, filename);

    FILE *file = fopen(filepath, "r");
    if(file == NULL)
    {
        fprintf(stderr, "Error reading config file.\n");
        exit(1);
    }

    struct PacketConfig *packetconfigs;
    int packet_ind = -1;
    int packet_num = 0;

    char line[300];
    fgets(line, 300, file);
    packet_num = strtol(line, NULL, 10);
    packetconfigs = (struct PacketConfig*)malloc(packet_num * sizeof(struct PacketConfig));
    
    
    while(fgets(line, 300, file))
    {
        enum ConfigType type = identify_type(line);
        if (type == COMMAND) continue;

        if(type == PACKET_NAME)
        {
            int tmpind = 0;
            while((line[++tmpind] <= 'Z') && (line[tmpind] >= 'A') || (line[tmpind] <= 'z') && (line[tmpind] >= 'a'));
            line[tmpind] = '\0';
            packetconfigs[++packet_ind].PacketName = strdup(line+1);
            packetconfigs[packet_ind].field_num = 0;
            packetconfigs[packet_ind].fields = (struct Field*)malloc(MAX_FIELD_NUM * sizeof(struct Field));
        }
        else if(type == FIELD_SETTING)
        {
            char *token = strtok(line, " ");
            struct Field *tmp = (packetconfigs[packet_ind].fields);
            tmp[packetconfigs[packet_ind].field_num].FieldName = strdup(token);
            
            token = strtok(NULL, "");
            get_available_array(&(tmp[packetconfigs[packet_ind].field_num].available), &(tmp[packetconfigs[packet_ind].field_num].available_num), token);

            packetconfigs[packet_ind].field_num++;
        }
    }

    // printf("Packet Num: %d\n", packet_num);
    // for(int i=0; i<packet_num; i++)
    // {
    //     printf("Packet Name: %s\n", packetconfigs[i].PacketName);
    //     printf("Field Num: %d\n", packetconfigs[i].field_num);
    //     for(int j=0; j<packetconfigs[i].field_num; j++)
    //     {
    //         printf("%s: [", packetconfigs[i].fields[j].FieldName);
    //         for(int k=0; k<packetconfigs[i].fields[j].available_num; k++)
    //         {
    //             printf("%d ", packetconfigs[i].fields[j].available[k]);
    //         }
    //         printf("\b]\n");
    //     }
    // }

    fclose(file);
    
    return packetconfigs;
}