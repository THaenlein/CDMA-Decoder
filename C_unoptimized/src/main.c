#include <iostream>
#include <iomanip>
#include <iterator>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

#include "SequenceGenerator.hpp"
#include "Decoder.hpp"

using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;
using Duration = std::chrono::duration<double>;

// Buch Seite 580+

/*
Optimierungsideen:
1. Sequenzgeneratoren als Konstanten abspeichern, da diese nicht abhängig vom Input sind. Das spart die Generierungszeit.
2. Statt sequenziell die Korrelationsergebnisse zu generieren (für jede Sequenzfolge nacheinander), könnten mehrere Threads gleichzeitig dekodieren.
3. Float statt double verwenden.
*/


int main(int argc, char* argv[])
{
#ifndef NDEBUG
    argv[1] = "gps_sequence.txt";
#endif

    std::ifstream is(argv[1]);
    std::istream_iterator<int16_t> start(is), end;
    std::vector<int16_t> numbers(start, end);
    const uint32_t sequenceLenth = static_cast<uint32_t>(numbers.size());

    TimeStamp timeStart = std::chrono::system_clock::now();

    std::vector<cdma::IndexPair> shiftRegisterSumIndices {
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

    std::vector<cdma::SequenceGenerator> generators;
    generators.reserve(shiftRegisterSumIndices.size());
    for (const cdma::IndexPair& indices : shiftRegisterSumIndices)
    {
        generators.push_back(cdma::SequenceGenerator(indices, sequenceLenth));
    }

    cdma::Decoder chipDecoder(numbers);
    std::vector<cdma::Correlation> decodeResult = chipDecoder.decode(generators);

    TimeStamp timeEnd = std::chrono::system_clock::now();
    Duration timeSpan = timeEnd - timeStart;
    double runTime = timeSpan.count();

    for (cdma::Correlation& correlation : decodeResult)
    {
        std::cout << "Satellite " << std::setw(2) << correlation.satelliteId
                  << " has sent bit " << correlation.message
                  << " (delta = " << std::setw(3) << correlation.offset << ")" << std::endl;
    }
    std::cout << "Time spent decoding signal: " << std::setprecision(6) << runTime << " seconds." << std::endl;
}