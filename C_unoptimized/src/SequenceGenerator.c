#include <stdio.h>
#include <string.h> 

#include "SequenceGenerator.h"


static uint8_t shiftRegisterSumIndices[NUM_SATELLITES][2] = {
    { 1, 5 },
    { 2, 6 },
    { 3, 7 },
    { 4, 8 },
    { 0, 8 },
    { 1, 9 },
    { 0, 7 },
    { 1, 8 },
    { 2, 9 },
    { 1, 2 },
    { 2, 3 },
    { 4, 5 },
    { 5, 6 },
    { 6, 7 },
    { 7, 8 },
    { 8, 9 },
    { 0, 3 },
    { 1, 4 },
    { 2, 5 },
    { 3, 6 },
    { 4, 7 },
    { 5, 8 },
    { 0, 2 },
    { 3, 5 }
};

static uint8_t SHIFT_INDICES_TOP[] = { 2 };
static uint8_t SHIFT_INDICES_BOTTOM[] = { 1, 2, 5, 7, 8 };


void CDMA_GenerateSequence(bool* sequence, int size, int indexOfIndices)
{
    bool motherSequenceTop    [REGISTER_LENGTH];
    bool motherSequenceBottom [REGISTER_LENGTH];
    memset(motherSequenceTop,    true, REGISTER_LENGTH * sizeof(bool));
    memset(motherSequenceBottom, true, REGISTER_LENGTH * sizeof(bool));

    int i;
    for (i = 0; i < size; i++)
    {
        bool motherFirst = motherSequenceTop[REGISTER_LENGTH - 1];
        bool motherSecond = motherSequenceBottom[shiftRegisterSumIndices[indexOfIndices][0]] ^
                            motherSequenceBottom[shiftRegisterSumIndices[indexOfIndices][1]];

        sequence[i] = motherFirst ^ motherSecond;

        _shiftMotherSequence(motherSequenceTop, REGISTER_LENGTH, SHIFT_INDICES_TOP, sizeof(SHIFT_INDICES_TOP));
        _shiftMotherSequence(motherSequenceBottom, REGISTER_LENGTH, SHIFT_INDICES_BOTTOM, sizeof(SHIFT_INDICES_BOTTOM));
    }

    return;
}

void _shiftMotherSequence(bool* sequence, int size, uint8_t* xorIndices, int indiceSize)
{
    bool newElement = sequence[size - 1];
    int i;
    for (i = 0; i < indiceSize; i++)
    {
        uint8_t index = xorIndices[i];
        newElement ^= sequence[index];
    }

    // Shift mother sequence
    for (i = size - 1; i > 0; i--)
    {
        sequence[i] = sequence[i - 1];
    }

    sequence[0] = newElement;
}
