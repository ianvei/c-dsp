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

// Function prototypes
void shuffle(int *array, int n);
void applyHannEnvelope(int16_t *grain);
void playNormal(FILE *outputFile, int16_t *grain, int grainSize);
void playReverse(FILE *outputFile, int16_t *grain, int grainSize);
void playFast(FILE *outputFile, int16_t *grain, int grainSize);
void playSlow(FILE *outputFile, int16_t *grain, int grainSize);
void playRepeated(FILE *outputFile, int16_t *grain, int grainSize);
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
        // applyHannEnvelope(grain);
        int effect = rand() % 5;
        switch (effect)
                {
            case 0:
                playNormal(outputFile, grain, GRAIN_SIZE);
                break;
            case 1:
                playReverse(outputFile, grain, GRAIN_SIZE);
                break;
            case 2:
                playFast(outputFile, grain, GRAIN_SIZE);
                break;
            case 3:
                playSlow(outputFile, grain, GRAIN_SIZE);
                break;
            case 4:
                playRepeated(outputFile, grain, GRAIN_SIZE);
                break;
        }
    }
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

void applyHannEnvelope(int16_t *grain)
{
    for(int j = 0; j < GRAIN_SIZE; j++){
        double multiplier = 0.5 * (1 - cos(2*M_PI*j/(GRAIN_SIZE-1)));
        grain[j] = (int16_t)(grain[j] * multiplier);
    }
}

void playNormal(FILE *outputFile, int16_t *grain, int grainSize)
{
    fwrite(grain, sizeof(int16_t), grainSize, outputFile);
}

void playReverse(FILE *outputFile, int16_t *grain, int grainSize)
{
    for (int j = grainSize - 1; j >= 0; j--)
    {
        fwrite(&grain[j], sizeof(int16_t), 1, outputFile);
    }
}

void playFast(FILE *outputFile, int16_t *grain, int grainSize)
{
    int skip = 2;  // Play at 2x speed
    for (int j = 0; j < grainSize; j += skip)
    {
        fwrite(&grain[j], sizeof(int16_t), 1, outputFile);
    }
}

void playSlow(FILE *outputFile, int16_t *grain, int grainSize)
{
    int stretch = 2;  // Play at 0.5x speed
    for (int j = 0; j < grainSize; j++)
    {
        for (int s = 0; s < stretch; s++)
        {
            fwrite(&grain[j], sizeof(int16_t), 1, outputFile);
        }
    }
}

void playRepeated(FILE *outputFile, int16_t *grain, int grainSize)
{
    int repeatCount = 1 + rand() % 4;  // 1-4 repeats
    for (int r = 0; r < repeatCount; r++)
    {
        fwrite(grain, sizeof(int16_t), grainSize, outputFile);
    }
}