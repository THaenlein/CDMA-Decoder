#include <vector>
#include <algorithm>
#include <numeric>

#include "Decoder.hpp"

using namespace cdma;

/*static*/ const uint16_t Decoder::MAX_DEVIATION = 65;

Decoder::Decoder(std::vector<int16_t>& sequence) :
    chipSequence(std::move(sequence)),
    satelliteCount(
        std::abs(*std::max_element(chipSequence.begin(), chipSequence.end(),
        [](int16_t a, int16_t b) { return abs(a) < abs(b); })))
{

}

std::vector<Correlation> Decoder::decode(const std::vector<SequenceGenerator>& generators) const
{
    std::vector<Correlation> result;

    for (size_t currentSatellite = 0; currentSatellite < generators.size(); currentSatellite++)
    {
        std::vector<bool> sequence = generators[currentSatellite].generate();
        const uint16_t peak = static_cast<uint16_t>(sequence.size()) - MAX_DEVIATION * (this->satelliteCount - 1);
        correlate(sequence, peak, currentSatellite, &result);
    }

    return std::move(result);
}

void Decoder::correlate(
    std::vector<bool>&sequence,
    const uint16_t peak,
    size_t satelliteId,
    std::vector<Correlation>* outResult) const
{
    size_t sequenceSize = sequence.size();
    for (size_t offset = 0; offset < sequenceSize; offset++)
    {
        int16_t accumulatedSum = 0;
        for (size_t i = 0; i < sequenceSize; i++)
        {
            size_t index = (i + offset) % sequenceSize;
            accumulatedSum += (sequence[index] ? this->chipSequence[i] : -this->chipSequence[i]);
        }

        if (static_cast<uint16_t>(abs(accumulatedSum)) > peak)
        {
            outResult->push_back({
                static_cast<uint16_t>(satelliteId + 1),
                static_cast<uint16_t>(offset),
                accumulatedSum > 0 ? true : false});
            break;
        }
    }
}
