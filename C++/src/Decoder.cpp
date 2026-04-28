#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdlib>

#include "Decoder.hpp"

/*static*/ const uint16_t cdma::Decoder::MAX_DEVIATION = 65;

cdma::Decoder::Decoder(const std::vector<int16_t>& sequence) :
    chipSequence(sequence),
    satelliteCount(
        std::abs(*std::max_element(chipSequence.begin(), chipSequence.end(),
        [](int16_t a, int16_t b) { return std::abs(a) < std::abs(b); })))
{

}

std::vector<cdma::Correlation> cdma::Decoder::decode(const std::vector<SequenceGenerator>& generators) const
{
    std::vector<Correlation> result;

    for (size_t currentSatellite = 0; currentSatellite < generators.size(); currentSatellite++)
    {
        std::vector<bool> sequence = generators[currentSatellite].generate();
        const uint16_t peak = static_cast<uint16_t>(sequence.size()) - MAX_DEVIATION * (this->satelliteCount - 1);
        correlate(sequence, peak, currentSatellite, result);
    }

    return result;
}

void cdma::Decoder::correlate(
    const std::vector<bool>& sequence,
    const uint16_t peak,
    size_t satelliteId,
    std::vector<Correlation>& outResult) const
{
    const size_t sequenceSize = sequence.size();
    for (size_t offset = 0; offset < sequenceSize; offset++)
    {
        int32_t accumulatedSum = 0;
        for (size_t i = 0; i < sequenceSize; i++)
        {
            size_t index = (i + offset) % sequenceSize;
            accumulatedSum += (sequence[index] ? this->chipSequence[i] : -this->chipSequence[i]);
        }

        if (static_cast<uint16_t>(std::abs(accumulatedSum)) > peak)
        {
            outResult.push_back({
                static_cast<uint16_t>(satelliteId + 1),
                static_cast<uint16_t>(offset),
                accumulatedSum > 0 ? true : false});
            break;
        }
    }
}
