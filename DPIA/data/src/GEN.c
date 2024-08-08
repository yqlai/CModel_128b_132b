# include "headers/GEN.h"

// [MSA]
// Fill_Count {50,100}

// [BS]
// SR {0,1}
// CP {0,1}
// Fill_Count {50,100}

// [VD] -> VDP <EOC_1> <TU_type_1> <L_1> <Fill_Count_1> <Video_Count_1> ... <EOC_n> <TU_type_n> <L_n> <Fill_Count_n> <Video_Count_n>
// Number of TU_Sets {1:3}
// EOC {0,1}
// TU_Type {0,1}
// L {0,1}
// Fill_Count {0,1}
// Video_Count {0:63}

// [SD] -> 	4. SDP <EFC/ND_1> <NSS_1> <NSE_1> <L_1> <Fill_Count_1> <Secondary_Count_1> ... <EFC/ND_n> <NSS_n> <NSE_n> <L_n> <Fill_Count_n> <Secondary_Count_n>
// Number of TU_Sets {1:3}
// EFC_ND {0,1}
// NSS {0,1}
// NSE {0,1}
// L {0,1}
// Fill_Count {50,100}
// Secondary_Count {0:63}

void write_to_file(int number, char **string, FILE *file)
{
    for (int i = 0; i < number; i++)
    {
        fprintf(file, "%s ", string[i]);
    }
    fprintf(file, "\n");
}

void GEN_MSA(struct PacketConfig config, FILE *file)
{
    struct Field *msa_fillcount = &(config.fields[0]);

    for(int i=0 ; i<msa_fillcount->available_num ; i++)
    {
        int fillcount = msa_fillcount->available[i];

        char fillcountString[10];
        sprintf(fillcountString, "%d", fillcount);
        char *parameters[] = {"MSA", fillcountString};
        write_to_file(2, parameters, file);
    }
}

void GEN_BS(struct PacketConfig config, FILE *file)
{
    struct Field *bs_sr = &(config.fields[0]);
    struct Field *bs_cp = &(config.fields[1]);
    struct Field *bs_fillcount = &(config.fields[2]);

    for(int i=0 ; i<bs_sr->available_num ; i++)
    {
        int sr = bs_sr->available[i];

        for(int j=0 ; j<bs_cp->available_num ; j++)
        {
            int cp = bs_cp->available[j];

            for(int k=0 ; k<bs_fillcount->available_num ; k++)
            {
                int fillcount = bs_fillcount->available[k];

                char srString[10];
                char cpString[10];
                char fillcountString[10];
                
                sprintf(srString, "%d", sr);
                sprintf(cpString, "%d", cp);
                sprintf(fillcountString, "%d", fillcount);

                char *parameters[] = {"BSP", srString, cpString, fillcountString};
                write_to_file(4, parameters, file);
            }
        }
    }
}

void GEN_VD_recur(char **params, struct Field *vd_eoc, struct Field *vd_tu_type, struct Field *vd_l, struct Field *vd_fillcount, struct Field *vd_video_count, int ind_param, int num_params, FILE *file)
{
    if(ind_param == num_params)
    {
        write_to_file(num_params, params, file);
        return;
    }

    // printf("ind_param: %d, num_params: %d\n", ind_param, num_params);

    for(int i=0 ; i<vd_eoc->available_num ; i++)
        for(int j=0 ; j<vd_tu_type->available_num ; j++)
            for(int k=0 ; k<vd_l->available_num ; k++)
                for(int l=0 ; l<vd_fillcount->available_num ; l++)
                    for(int m=0 ; m<vd_video_count->available_num ; m++)
                    {
                        char eocString[10];
                        char tuTypeString[10];
                        char lString[10];
                        char fillcountString[10];
                        char videoCountString[10];
                        
                        sprintf(eocString, "%d", vd_eoc->available[i]);
                        sprintf(tuTypeString, "%d", vd_tu_type->available[j]);
                        sprintf(lString, "%d", vd_l->available[k]);
                        sprintf(fillcountString, "%d", vd_fillcount->available[l]);
                        sprintf(videoCountString, "%d", vd_video_count->available[m]);

                        int ind_param_t = ind_param;

                        params[ind_param_t++] = eocString;
                        params[ind_param_t++] = tuTypeString;
                        params[ind_param_t++] = lString;
                        params[ind_param_t++] = fillcountString;
                        params[ind_param_t++] = videoCountString;

                        GEN_VD_recur(params, vd_eoc, vd_tu_type, vd_l, vd_fillcount, vd_video_count, ind_param_t, num_params, file);
                    }
}

void GEN_VD(struct PacketConfig config, FILE *file)
{
    struct Field *vd_tu_sets = &(config.fields[0]);
    struct Field *vd_eoc = &(config.fields[1]);
    struct Field *vd_tu_type = &(config.fields[2]);
    struct Field *vd_l = &(config.fields[3]);
    struct Field *vd_fillcount = &(config.fields[4]);
    struct Field *vd_video_count = &(config.fields[5]);

    for(int ind_tu=0 ; ind_tu<vd_tu_sets->available_num ; ind_tu++)
    {
        int num_tus = vd_tu_sets->available[ind_tu];
        int num_params = 5 * num_tus + 1;
        char **parameters = (char**)malloc(num_params * sizeof(char*));

        printf("Num Params: %d\n", num_params);
        
        parameters[0] = "VDP";
        GEN_VD_recur(parameters, vd_eoc, vd_tu_type, vd_l, vd_fillcount, vd_video_count, 1, num_params, file);        
    }
}

void GEN_SD_recur(char **params, struct Field *sd_efc_nd, struct Field *sd_nss, struct Field *sd_nse, struct Field *sd_l, struct Field *sd_fillcount, struct Field *sd_secondary_count, int ind_param, int num_params, FILE *file)
{
    if(ind_param == num_params)
    {
        write_to_file(num_params, params, file);
        return;
    }

    for(int i=0 ; i<sd_efc_nd->available_num ; i++)
        for(int j=0 ; j<sd_nss->available_num ; j++)
            for(int k=0 ; k<sd_nse->available_num ; k++)
                for(int l=0 ; l<sd_l->available_num ; l++)
                    for(int m=0 ; m<sd_fillcount->available_num ; m++)
                        for(int n=0 ; n<sd_secondary_count->available_num ; n++)
                        {
                            char efcNdString[10];
                            char nssString[10];
                            char nseString[10];
                            char lString[10];
                            char fillcountString[10];
                            char secondaryCountString[10];
                            
                            sprintf(efcNdString, "%d", sd_efc_nd->available[i]);
                            sprintf(nssString, "%d", sd_nss->available[j]);
                            sprintf(nseString, "%d", sd_nse->available[k]);
                            sprintf(lString, "%d", sd_l->available[l]);
                            sprintf(fillcountString, "%d", sd_fillcount->available[m]);
                            sprintf(secondaryCountString, "%d", sd_secondary_count->available[n]);

                            int ind_param_t = ind_param;

                            params[ind_param_t++] = efcNdString;
                            params[ind_param_t++] = nssString;
                            params[ind_param_t++] = nseString;
                            params[ind_param_t++] = lString;
                            params[ind_param_t++] = fillcountString;
                            params[ind_param_t++] = secondaryCountString;

                            GEN_SD_recur(params, sd_efc_nd, sd_nss, sd_nse, sd_l, sd_fillcount, sd_secondary_count, ind_param_t, num_params, file);
                        }
}

void GEN_SD(struct PacketConfig config, FILE *file)
{
    struct Field *sd_tu_sets = &(config.fields[0]);
    struct Field *sd_efc_nd = &(config.fields[1]);
    struct Field *sd_nss = &(config.fields[2]);
    struct Field *sd_nse = &(config.fields[3]);
    struct Field *sd_l = &(config.fields[4]);
    struct Field *sd_fillcount = &(config.fields[5]);
    struct Field *sd_secondary_count = &(config.fields[6]);

    for(int ind_tu=0 ; ind_tu<sd_tu_sets->available_num ; ind_tu++)
    {
        int num_tus = sd_tu_sets->available[ind_tu];
        int num_params = 6 * num_tus + 1;
        char **parameters = (char**)malloc(num_params * sizeof(char*));

        printf("Num Params: %d\n", num_params);
        
        parameters[0] = "SDP";
        GEN_SD_recur(parameters, sd_efc_nd, sd_nss, sd_nse, sd_l, sd_fillcount, sd_secondary_count, 1, num_params, file);
    }
}