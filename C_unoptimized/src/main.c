#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "stdafx.h"
#include "SequenceGenerator.h"
//#include "Decoder.h"

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

    // TODO: Put some data on heap
    bool sequences[NUM_SATELLITES][CHIP_SEQUENCE_LENGTH];
    for (i = 0; i < NUM_SATELLITES; i++)
    {
        CDMA_GenerateSequence(sequences[i], CHIP_SEQUENCE_LENGTH, i);
    }

    //cdma::Decoder chipDecoder(numbers);
    //std::vector<cdma::Correlation> decodeResult = chipDecoder.decode(generators);

    clock_t end = clock();
    float timeSpan = (float)(end - start) / CLOCKS_PER_SEC;

    // TODO: Fix output
    //for (cdma::Correlation& correlation : decodeResult)
    //{
    //    std::cout << "Satellite " << std::setw(2) << correlation.satelliteId
    //              << " has sent bit " << correlation.message
    //              << " (delta = " << std::setw(3) << correlation.offset << ")" << std::endl;
    //}
    printf("Time spent decoding signal: %.5f seconds.\n", timeSpan);

    return 0;
}