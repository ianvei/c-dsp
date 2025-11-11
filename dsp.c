#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int HEADER_SIZE = 44; // Standard WAV header size
const int GRAIN_SIZE = 44100; // 1 second grain size at 44.1kHz
void shuffle(int *array, int n);
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <input file> <output file> <gain>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL)
    {
        perror("Error opening input file");
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "wb");
    if (outputFile == NULL)
    {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }
    // convert gain argument to float (ascii to float)
    float gain = atof(argv[3]);
    if (gain < 0)
    {
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

    // sequence of samples after header, 16 bit signed integers
    int16_t sample;

    fseek(inputFile, 0, SEEK_END);
    long fileSize = ftell(inputFile);
    long audioSize = fileSize - HEADER_SIZE;
    int numSamples = audioSize / sizeof(int16_t);
    int numGrains = numSamples / GRAIN_SIZE;
    int *grainOrder = (int *)malloc(numGrains * sizeof(int));
    for(int i=0; i < numGrains; i++){
        grainOrder[i] = i;
    }
    shuffle(grainOrder, numGrains);
    int16_t *grain = (int16_t*)malloc(GRAIN_SIZE * sizeof(int16_t));
    for(int i = 0; i< numGrains; i++){
        fseek(inputFile, HEADER_SIZE + grainOrder[i] * GRAIN_SIZE * sizeof(int16_t), SEEK_SET);
        fread(grain, sizeof(int16_t), GRAIN_SIZE, inputFile);
        // for(int j = 0; j < GRAIN_SIZE; j++){
        //     double multiplier = 0.5 * (1 - cos(2*M_PI*j/(GRAIN_SIZE-1)));
        //     grain[j] = (int16_t)(grain[j] * multiplier);
        // }
        fwrite(grain, sizeof(int16_t), GRAIN_SIZE, outputFile);
    }

    // int16_t *samples = (int16_t *)malloc(audioSize * sizeof(int16_t));
    // fseek(inputFile, HEADER_SIZE, SEEK_SET);
    // fread(samples, sizeof(int16_t), numSamples, inputFile);
    // for(int i = numSamples-1; i >= 0; i--)
    // {
    //     srand(time(NULL));
    //     int nRandonNumber = rand() % ((i + 1) - 1) + 1;
    //     fwrite(&samples[nRandonNumber], sizeof(int16_t), 1, outputFile);
    // }
    // while (fread(&sample, sizeof(int16_t), 1, inputFile) == 1)
    // {
    //     srand(time(NULL));
    //     int nRandonNumber = rand() % ((4 + 1) - 0) + 0;
    //     sample = (int16_t)(sample * nRandonNumber);
    //     fwrite(&sample, sizeof(int16_t), 1, outputFile);
    // }
    free(grain);
    free(grainOrder);
    fclose(inputFile);
    fclose(outputFile);
    return 0;
}

// Fisher-Yates shuffle algorithm
void shuffle(int *array, int n)
{
    srand(time(NULL));
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        // Swap
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}