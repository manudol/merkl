#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#include "gettime.h"

long get_utc()
{
    time_t rawtime;
    time(&rawtime);
    return rawtime;    
}

void get_str_time(char str[20])
{
    sprintf(str, "%ld", get_utc());
}


// int main()
// {
//     char time[20];
//     get_str_time(time);
// 
//     char* id = "id123";
//     char* seq = "ACGATCGATGCATGCATCG";
//     
//     size_t totallen = strlen(id) + strlen(seq) + strlen(time);
//     char* sr_data = malloc(totallen + 1);
//     sprintf(sr_data, "{\n	\"id\": \"%s\",\n	\"seq\":\"%s\",\n	\"timestamp\": \"%s\"\n}", id, seq, time);
// 
//     printf("%s\n", sr_data);
// 
//     FILE* fp = fopen("req.json", "w");
//     if (fp == NULL) {
//         perror("File ptr is null\n");
//     }
//     fprintf(fp, "%s", sr_data);
// 
//     free(sr_data);
//     return 0;
// }
