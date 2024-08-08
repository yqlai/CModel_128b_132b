# include "headers/config.h"
# include "headers/GEN.h"

int main()
{
    FILE *file = fopen("input/exhaustion.txt", "w");

    struct PacketConfig *configs = get_configs();
    
    GEN_MSA(configs[0], file);
    GEN_BS(configs[1], file);
    GEN_VD(configs[2], file);
    GEN_SD(configs[3], file);

    printf("Completed.\n");

    return 0;
}