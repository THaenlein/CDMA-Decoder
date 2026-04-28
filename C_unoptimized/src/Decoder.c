#include <stdlib.h>

#include "Decoder.h"
#include "stdafx.h"


#define MAX_DEVIATION 65

static bool _correlate(const bool* sequence, const int32_t* chipSequence, int peak, int satelliteId, int foundCorrelations, Correlation* outCorrelations)
{
	int offset;
	for (offset = 0; offset < CHIP_SEQUENCE_LENGTH; offset++)
	{
		int32_t accumulatedSum = 0;
		int i;
		for (i = 0; i < CHIP_SEQUENCE_LENGTH; i++)
		{
			size_t index = (i + offset) % CHIP_SEQUENCE_LENGTH;
			accumulatedSum += (sequence[index] ? chipSequence[i] : -chipSequence[i]);
		}

		if (abs(accumulatedSum) > peak)
		{
			outCorrelations[foundCorrelations].satelliteId = satelliteId + 1;
			outCorrelations[foundCorrelations].offset = offset;
			outCorrelations[foundCorrelations].message = accumulatedSum > 0 ? true : false;
			return true;
		}
	}
	return false;
}

void CDMA_decode(bool* const* sequences, const int32_t* chipSequence, int numSendingSatellites, Correlation* outCorrelations)
{
	if (sequences == NULL || chipSequence == NULL || outCorrelations == NULL || numSendingSatellites <= 0)
	{
		return;
	}

	int i;
	int numCorrelationsFound = 0;
	int peak = CHIP_SEQUENCE_LENGTH - MAX_DEVIATION * (numSendingSatellites - 1);
	for (i = 0; i < NUM_SATELLITES; i++)
	{
		if (numCorrelationsFound == numSendingSatellites)
		{
			break;
		}
		bool correlationFound = _correlate(sequences[i], chipSequence, peak, i, numCorrelationsFound, outCorrelations);
		numCorrelationsFound += (int)correlationFound;
	}

	return;
}
