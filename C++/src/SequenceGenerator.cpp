#include<deque>

#include "SequenceGenerator.hpp"

using namespace std;
using namespace cdma;

/*static*/ const size_t SequenceGenerator::REGISTER_LENGTH = 10;

/*static*/ MotherSequenceIndices SequenceGenerator::SHIFT_INDICES({ 2 }, { 1, 2, 5, 7, 8 });

vector<bool> SequenceGenerator::generate()
{
    vector<bool> sequence(this->sequenceLength);
    MotherSequences motherSequences(deque<bool>(REGISTER_LENGTH, true), deque<bool>(REGISTER_LENGTH, true));

    for (size_t i = 0; i < this->sequenceLength; i++)
    {
        bool motherFirst = motherSequences.first.back();
        bool motherSecond = motherSequences.second[registerSumIndices.first] ^ motherSequences.second[registerSumIndices.second];

        // Is this the right insertion order? Push front or push back? 
        sequence.push_back(motherFirst ^ motherSecond);

        shiftMotherSequence(motherSequences.first, SHIFT_INDICES.first);
        shiftMotherSequence(motherSequences.second, SHIFT_INDICES.second);
    }
    
    return move(sequence);
}
void SequenceGenerator::shiftMotherSequence(deque<bool>& motherSequence, vector<uint8_t>& xorIndices)
{
    bool newElement = motherSequence.back();
    for (uint8_t index : xorIndices)
    {
        newElement ^= motherSequence[index];
    }
    motherSequence.pop_back();
    motherSequence.push_front(newElement);
}
