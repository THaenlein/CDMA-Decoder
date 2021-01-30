#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "stdafx.h"


#define REGISTER_LENGTH 10


void CDMA_GenerateSequence(bool* sequence, int size, int indexOfIndices);
