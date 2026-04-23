#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

int main()
{
    string filename;

    cout << "Enter source code filename: ";
    cin >> filename;

    ifstream file(filename, ios::binary);

    if (!file)
    {
        cout << "Error: File not found.\n";
        return 0;
    }

    int cacheSizeKB;
    int blockSize;

    cout << "Enter cache size (KB): ";
    cin >> cacheSizeKB;

    cout << "Enter block size (bytes): ";
    cin >> blockSize;

    int cacheLines = (cacheSizeKB * 1024) / blockSize;

    vector<int> cache(cacheLines, -1);

    int hits = 0;
    int misses = 0;

    char byte;
    int address = 0;

    while (file.get(byte))
    {
        int blockNumber = address / blockSize;

        int cacheLine = blockNumber % cacheLines;

        if (cache[cacheLine] == blockNumber)
        {
            hits++;
        }
        else
        {
            misses++;
            cache[cacheLine] = blockNumber;
        }

        address++;
    }

    cout << "\nCache Simulation Results\n";

    cout << "Total accesses: "
         << hits + misses << endl;

    cout << "Cache hits: "
         << hits << endl;

    cout << "Cache misses: "
         << misses << endl;

    double hitRatio =
        (double)hits / (hits + misses);

    cout << "Hit ratio: "
         << hitRatio << endl;

    return 0;
}
