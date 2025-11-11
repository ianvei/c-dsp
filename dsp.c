#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

const int HEADER_SIZE = 44; // Standard WAV header size

int main(int argc, char *argv[])
{
    if(argc != 4) {
        printf("Usage: %s <input file> <output file> <gain>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "rb");
    if(inputFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "wb");
    if(outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }
     //convert gain argument to float (ascii to float)
    float gain = atof(argv[3]);
    if(gain < 0) {
        fprintf(stderr, "Invalid gain value. Gain must be non-negative.\n");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    // create buffer for header of size HEADER_SIZE
    // unsigned 8 bit integer array aka uint8_t aka buffer of 44 bytes to fit the header data
    uint8_t header[HEADER_SIZE];
    // read header data from input file and save header data to header buffer
    fread(header, sizeof(uint8_t), HEADER_SIZE, inputFile);
    // write header data to output file from header buffer
    fwrite(header, sizeof(uint8_t), HEADER_SIZE, outputFile);

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}
