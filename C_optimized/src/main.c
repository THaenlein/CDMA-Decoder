#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "stdafx.h"
#include "Decoder.h"

// Buch Seite 580+

/*
Optimierungsideen:
1. Sequenzgeneratoren als Konstanten abspeichern, da diese nicht abhängig vom Input sind. Das spart die Generierungszeit.
2. Verwendung von "Binary GCD Algorithmus" statt Modulo-Operator
*/


int main(int argc, char* argv[])
{
    const char* inputPath = NULL;
#ifndef NDEBUG
    inputPath = "gps_sequence.txt";
#else
    if (argc < 2)
    {
        fprintf(stderr, "Missing input file argument.\n");
        return 1;
    }
    inputPath = argv[1];
#endif

    FILE* f = fopen(inputPath, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Failed to open input file: %s\n", inputPath);
        return 1;
    }
    int32_t chipSequence[CHIP_SEQUENCE_LENGTH];
    int i;

    for (i = 0; i < CHIP_SEQUENCE_LENGTH; i++)
    {
        int result = fscanf(f, "%d ", &chipSequence[i]);
        if (result != 1)
        {
            fprintf(stderr, "Failed to parse chip sequence at position %d.\n", i);
            fclose(f);
            return 1;
        }
    }
    fclose(f);

    clock_t start = clock();

    int maxElement = abs(*chipSequence);
    int absolute;
    for (int32_t* seqPtr = chipSequence; seqPtr < chipSequence+CHIP_SEQUENCE_LENGTH; seqPtr++)
    {
        absolute = abs(*seqPtr);
        if (absolute > maxElement)
        {
            maxElement = absolute;
        }
    }
    Correlation* correlationResults = malloc(maxElement * sizeof(Correlation));
    if (correlationResults == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for decode results.\n");
        return 1;
    }
    CDMA_decode(chipSequence, (uint32_t) maxElement, correlationResults);

    clock_t end = clock();
    float timeSpan = (float)(end - start) / CLOCKS_PER_SEC;

    for (i = 0; i < maxElement; i++)
    {
        printf("Satellite %2d has sent bit %d (delta = %3d)\n", 
            correlationResults[i].satelliteId, correlationResults[i].message, correlationResults[i].offset);
    }
    printf("Time spent decoding signal: %.5f seconds.\n", timeSpan);

    free(correlationResults);
    return 0;
}
