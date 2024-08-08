# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>

# ifndef __CONFIG_H
# define __CONFIG_H

# define MAX_FIELD_NUM 10

enum ConfigType
{
    PACKET_NAME,
    FIELD_SETTING,
    COMMAND
};

struct Field
{
    char *FieldName;
    int *available;
    int available_num;
};

struct PacketConfig
{
    char *PacketName;
    struct Field *fields;
    int field_num;
};

int iscommand(char*);
enum ConfigType identify_type(char*);
int is_valid_config_value(char*);
void get_available_array(int**, int*, char*);
struct PacketConfig* get_configs();

# endif