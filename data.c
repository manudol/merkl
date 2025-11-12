#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "gettime.h"
#include "data.h"

char* serialize_data(char* id, char* seq)
{
    char time[20];
    get_str_time(time);
    
    size_t totallen = snprintf(
        NULL, 0,
        "{\n\t\"id\": \"%s\",\n\t\"seq\":\"%s\",\n\t\"timestamp\": \"%s\"\n}",
        id, seq, time
    );

    char* sr_data = malloc(totallen + 1);
    sprintf(sr_data, "{\n	\"id\": \"%s\",\n	\"seq\":\"%s\",\n	\"timestamp\": \"%s\"\n}", id, seq, time);
    return sr_data;
}

char* null_data()
{
    char time[20];
    get_str_time(time);
    
    size_t totallen = snprintf(
        NULL, 0,
        "{\n\t\"id\": \"%p\",\n\t\"seq\":\"%p\",\n\t\"timestamp\": \"%s\"\n}",
        NULL, NULL, time
    );

    char* sr_data = malloc(totallen + 1);
    sprintf(sr_data, "{\n	\"id\": \"%p\",\n	\"seq\":\"%p\",\n	\"timestamp\": \"%s\"\n}", NULL, NULL, time);
    return sr_data;
}
