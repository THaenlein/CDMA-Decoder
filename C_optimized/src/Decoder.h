#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "SequenceGenerator.h"


typedef struct
{
    int satelliteId;
    uint16_t offset;
    bool message;
}Correlation;


void CDMA_decode(int32_t* chipSequence, uint32_t numSendingSatellites, Correlation* outCorrelations);
