#include "SequenceGenerator.hpp"

/*static*/ const size_t cdma::SequenceGenerator::REGISTER_LENGTH = 10;

/*static*/ const cdma::MotherSequenceIndices cdma::SequenceGenerator::SHIFT_INDICES({ 2 }, { 1, 2, 5, 7, 8 });

std::vector<bool> cdma::SequenceGenerator::generate() const
{
    std::vector<bool> sequence;
    sequence.reserve(this->sequenceLength);
    MotherSequences motherSequences(std::deque<bool>( REGISTER_LENGTH, true ), std::deque<bool>(REGISTER_LENGTH, true));

    for (size_t i = 0; i < this->sequenceLength; i++)
    {
        bool motherFirst = motherSequences.first.back();
        bool motherSecond = 
            motherSequences.second[this->registerSumIndices.first] ^
            motherSequences.second[this->registerSumIndices.second];

        sequence.push_back(motherFirst ^ motherSecond);

        shiftMotherSequence(motherSequences.first, SHIFT_INDICES.first);
        shiftMotherSequence(motherSequences.second, SHIFT_INDICES.second);
    }
    
    return sequence;
}

void cdma::SequenceGenerator::shiftMotherSequence(std::deque<bool>& motherSequence, const std::vector<uint8_t>& xorIndices) const
{
    bool newElement = motherSequence.back();
    for (uint8_t index : xorIndices)
    {
        newElement ^= motherSequence[index];
    }
    motherSequence.pop_back();
    motherSequence.push_front(newElement);
}
