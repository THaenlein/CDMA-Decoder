#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "stdafx.h"
#include "SequenceGenerator.h"
#include "Decoder.h"

// Buch Seite 580+

/*
Optimierungsideen:
1. Sequenzgeneratoren als Konstanten abspeichern, da diese nicht abhängig vom Input sind. Das spart die Generierungszeit.
2. Verwendung von "Binary GCD Algorithmus" statt Modulo-Operator
*/


int main(int argc, char* argv[])
{
#ifndef NDEBUG
    argv[1] = "gps_sequence.txt";
#endif

    FILE* f = fopen(argv[1], "r");
    int32_t chipSequence[CHIP_SEQUENCE_LENGTH];
    register int i;

    for (i = 0; i < CHIP_SEQUENCE_LENGTH; i++)
    {
        int result = fscanf(f, "%d ", &chipSequence[i]);
        (void)result;
    }
    fclose(f);

    clock_t start = clock();

    int maxElement = *chipSequence;
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
    CDMA_decode(chipSequence, (uint32_t) maxElement, correlationResults);

    clock_t end = clock();
    float timeSpan = (float)(end - start) / CLOCKS_PER_SEC;

    for (i = 0; i < maxElement; i++)
    {
        printf("Satellite %2d has sent bit %d (delta = %3d)\n", 
            correlationResults[i].satelliteId, correlationResults[i].message, correlationResults[i].offset);
    }
    printf("Time spent decoding signal: %.5f seconds.\n", timeSpan);

    return 0;
}