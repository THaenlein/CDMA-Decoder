#include <iostream>
#include <iomanip>
#include <iterator>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

#include "SequenceGenerator.hpp"
#include "cdma.hpp"

using namespace std;
using TimeStamp = chrono::time_point<chrono::system_clock>;
using Duration = chrono::duration<double>;


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
    const string filePath(argv[1]);

    ifstream is(filePath);
    istream_iterator<int16_t> start(is), end;
    vector<int16_t> numbers(start, end);
    const uint32_t sequenceLenth = static_cast<uint32_t>(numbers.size());
    cout << "Read " << numbers.size() << " numbers" << endl;
    int16_t NumberOfSatellites = *max_element(numbers.begin(), numbers.end(),
        [](int16_t a, int16_t b) { return abs(a) < abs(b); });

    // TODO: Start timer
    TimeStamp timeStart = chrono::system_clock::now();

    // Maybe add last shift register element to each vector?
    
    vector<cdma::IndexPair> shiftRegisterSumIndices {
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

    vector<cdma::SequenceGenerator> generators;
    for (const cdma::IndexPair& indices : shiftRegisterSumIndices)
    {
        generators.push_back(cdma::SequenceGenerator(indices, sequenceLenth));
    }

    // TODO: Create chip sequence decoder

    // TODO: Create decoded signal 

    // TODO: Stop timer
    TimeStamp timeEnd = chrono::system_clock::now();
    Duration timeSpan = timeEnd - timeStart;
    double runTime = timeSpan.count();

    // TODO: Print results
    cout << "Time spent decoding signal: " << setprecision(6) << runTime << " seconds." << endl;
    cout << "Number of satellites: " << NumberOfSatellites << endl;
    //cout << "numbers read in:\n";
    //copy(numbers.begin(), numbers.end(), ostream_iterator<int16_t>(cout, " "));
    //cout << endl;
}