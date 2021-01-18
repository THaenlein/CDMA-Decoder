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
2. Statt sequenziell die Korrelationsergebnisse zu generieren (für jede Sequenzfolge nacheinander), könnten mehrere Threads gleichzeitig dekodieren.
3. Float statt double verwenden.
4. Benutze Bitfelder statt Arrays für die Muttersequenzen
*/


int main(int argc, char* argv[])
{
#ifndef NDEBUG
    argv[1] = "gps_sequence.txt";
#endif

    FILE* f = fopen(argv[1], "r");
    int32_t chipSequence[CHIP_SEQUENCE_LENGTH];
    uint16_t i;

    for (i = 0; i < CHIP_SEQUENCE_LENGTH; i++)
    {
        int result = fscanf(f, "%d ", &chipSequence[i]);
    }
    fclose(f);

    clock_t start = clock();

    bool* sequences[NUM_SATELLITES];
    for (i = 0; i < NUM_SATELLITES; i++)
    {
        sequences[i] = malloc(CHIP_SEQUENCE_LENGTH * sizeof(bool));
    }
    for (i = 0; i < NUM_SATELLITES; i++)
    {
        CDMA_GenerateSequence(sequences[i], CHIP_SEQUENCE_LENGTH, i);
    }

    int maxElement = chipSequence[0];
    for (i = 0; i < CHIP_SEQUENCE_LENGTH; i++)
    {
        if (abs(chipSequence[i]) > maxElement)
        {
            maxElement = abs(chipSequence[i]);
        }
    }
    Correlation* correlationResults = malloc(maxElement * sizeof(Correlation));
    CDMA_decode(sequences, chipSequence, maxElement, correlationResults);

    clock_t end = clock();
    float timeSpan = (float)(end - start) / CLOCKS_PER_SEC;

    // TODO: Fix output
    for (i = 0; i < maxElement; i++)
    {
        printf("Satellite %2d has sent bit %d (delta = %3d)\n", 
            correlationResults[i].satelliteId, correlationResults[i].message, correlationResults[i].offset);
    }
    printf("Time spent decoding signal: %.5f seconds.\n", timeSpan);

    return 0;
}