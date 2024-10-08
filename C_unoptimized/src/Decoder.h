#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "SequenceGenerator.h"


typedef struct
{
    int satelliteId;
    int offset;
    bool message;
}Correlation;


void CDMA_decode(bool** sequences, int32_t* chipSequence, int numSendingSatellites, Correlation* outCorrelations);
