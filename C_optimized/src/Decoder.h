#pragma once

#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    int32_t satelliteId;
    uint16_t offset;
    bool message;
}Correlation;


void CDMA_decode(int32_t* __restrict chipSequence, uint32_t numSendingSatellites, Correlation* outCorrelations);
