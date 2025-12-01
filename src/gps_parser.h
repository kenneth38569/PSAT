#ifndef GPS_PARSER_H
#define GPS_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Structure to hold parsed GPS data
typedef struct {
    char time[12];           // UTC time (HHMMSS.SS)
    float latitude;          // Latitude in decimal degrees
    float longitude;         // Longitude in decimal degrees
    int fix_quality;         // 0=invalid, 1=GPS fix, 2=DGPS fix
    int num_satellites;      // Number of satellites used
    float altitude;          // Altitude in meters
    char is_valid;           // 1 if data is valid
} GPS_Data;

// Parse a GNGGA sentence
void parse_gngga(char *sentence, GPS_Data *data) {
    char *token;
    char sentence_copy[256];
    strcpy(sentence_copy, sentence);
    
    int field = 0;
    token = strtok(sentence_copy, ",");
    
    while (token != NULL && field < 12) {
        switch(field) {
            case 1:  // Time
                strcpy(data->time, token);
                break;
            case 2:  // Latitude
                data->latitude = atof(token) / 100.0;
                break;
            case 4:  // Longitude
                data->longitude = atof(token) / 100.0;
                break;
            case 6:  // Fix quality
                data->fix_quality = atoi(token);
                data->is_valid = (data->fix_quality > 0) ? 1 : 0;
                break;
            case 7:  // Number of satellites
                data->num_satellites = atoi(token);
                break;
            case 9:  // Altitude
                data->altitude = atof(token);
                break;
        }
        token = strtok(NULL, ",");
        field++;
    }
}

// Print parsed GPS data
void print_gps_data(GPS_Data *data) {
    if (data->is_valid) {
        printf("=== GPS Data ===\n");
        printf("Time: %s UTC\n", data->time);
        printf("Latitude: %.6f°\n", data->latitude);
        printf("Longitude: %.6f°\n", data->longitude);
        printf("Altitude: %.1f m\n", data->altitude);
        printf("Satellites: %d\n", data->num_satellites);
        printf("Fix Quality: %d\n\n", data->fix_quality);
    } else {
        printf("No GPS fix yet - searching for satellites...\n\n");
    }
}

#endif