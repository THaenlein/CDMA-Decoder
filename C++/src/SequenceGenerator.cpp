#include "SequenceGenerator.hpp"

using namespace std;
using namespace cdma;

/*static*/ const size_t SequenceGenerator::REGISTER_LENGTH = 10;

/*static*/ MotherSequenceIndices SequenceGenerator::SHIFT_INDICES({ 2 }, { 1, 2, 5, 7, 8 });

std::vector<bool> SequenceGenerator::generate()
{
    std::vector<bool> sequence(this->sequenceLength);
    MotherSequences motherSequences(REGISTER_LENGTH , REGISTER_LENGTH);
    std::fill(motherSequences.first.begin(), motherSequences.first.end(), true);
    std::fill(motherSequences.second.begin(), motherSequences.second.end(), true);

    for (size_t i = 0; i < this->sequenceLength; i++)
    {
        bool motherFirst = *(motherSequences.first.end() - 1);
        bool motherSecond = motherSequences.second[registerSumIndices.first] ^ motherSequences.second[registerSumIndices.second];

        // Is this the right insertion order? Push front or push back? 
        sequence.push_back(motherFirst ^ motherSecond);

        shiftMotherSequence(motherSequences.first, SHIFT_INDICES.first);
        shiftMotherSequence(motherSequences.second, SHIFT_INDICES.second);
    }
    
    return std::move(sequence);
}
void SequenceGenerator::shiftMotherSequence(std::vector<bool>& motherSequence, vector<uint8_t>& xorIndices)
{
    bool newElement = *(motherSequence.end() - 1);
    for (uint8_t index : xorIndices)
    {
        newElement ^= motherSequence[index];
    }
    motherSequence.pop_back();
    motherSequence.insert(motherSequence.begin(), newElement);
}
