#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>
using namespace std;
#define MAX_VALUE_SIZE 8
#define MEMORY_SIZE (1ULL << 24)
struct CacheEntry
{
    bool valid;
    int tag;
    time_t LRU;
};
struct CacheSet
{
    vector<CacheEntry> ways;
};
vector<CacheSet> cacheMemory;
vector<char> mainMemory(MEMORY_SIZE);
int CACHE_SIZE;
int ASSOCIATIVITY;
int BLOCK_SIZE;
int OFFSET_BITS;
int INDEX_BITS;
int TAG_BITS;
int SETS;
int FRAMES;
time_t globalTime = 0;
int log2cache(int n)
{
    return log2(n);
}
int getTag(int address)
{
    return address >> (OFFSET_BITS + INDEX_BITS);
}
int getIndex(int address)
{
    return (address >> OFFSET_BITS) & ((1 << INDEX_BITS) - 1);
}
int getOffset(int address)
{
    return address & ((1 << OFFSET_BITS) - 1);
}
bool isInCache(int index, int tag)
{
    for (auto &entry : cacheMemory[index].ways)
    {
        if (entry.valid && entry.tag == tag)
            return true;
    }
    return false;
}
void load(int address, int size)
{
    int tag = getTag(address);
    int index = getIndex(address);

    if (isInCache(index, tag))
    {
        cout << "load 0x" << hex << address << " hit ";
    }
    else
    {
        cout << "load 0x" << hex << address << " miss ";

        bool inserted = false;
        for (auto &entry : cacheMemory[index].ways)
        {
            if (!entry.valid)
            {
                entry.valid = true;
                entry.tag = tag;
                entry.LRU = globalTime++;
                inserted = true;
                break;
            }
        }
        if (!inserted)
        {
            int lruIndex = 0;
            time_t minTime = cacheMemory[index].ways[0].LRU;

            for (int i = 1; i < ASSOCIATIVITY; i++)
            {
                if (cacheMemory[index].ways[i].LRU < minTime)
                {
                    minTime = cacheMemory[index].ways[i].LRU;
                    lruIndex = i;
                }
            }
            cacheMemory[index].ways[lruIndex].tag = tag;
            cacheMemory[index].ways[lruIndex].LRU = globalTime++;
        }
    }
    for (int i = 0; i < size; i++)
        printf("%02x",(unsigned char)mainMemory[address + i]);

    cout << endl;
}
void store(int address, string data, int size)
{
    int tag = getTag(address);
    int index = getIndex(address);
    for (int i = 0; i < size; i++)
    {
        string byteStr = data.substr(i * 2, 2);
        mainMemory[address + i] = stoi(byteStr, nullptr, 16);
    }
    if (isInCache(index, tag))
        cout << "store 0x" << hex << address << " hit" << endl;
    else
        cout << "store 0x" << hex << address << " miss" << endl;
}
int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cout << "Usage:\n";
        cout << "./cachesim trace.txt 32 1 32\n";
        return 0;
    }
    ifstream file(argv[1]);
    CACHE_SIZE = stoi(argv[2]);
    ASSOCIATIVITY = stoi(argv[3]);
    BLOCK_SIZE = stoi(argv[4]);
    FRAMES = CACHE_SIZE * 1024 / BLOCK_SIZE;
    SETS = FRAMES / ASSOCIATIVITY;
    OFFSET_BITS = log2cache(BLOCK_SIZE);
    INDEX_BITS = log2cache(SETS);
    TAG_BITS = 24 - OFFSET_BITS - INDEX_BITS;
    cacheMemory.resize(SETS);
    for (int i = 0; i < SETS; i++)
    {
        cacheMemory[i].ways.resize(ASSOCIATIVITY);
        for (auto &entry : cacheMemory[i].ways)
            entry.valid = false;
    }
    string operation;
    while (file >> operation)
    {
        int address, size;
        file >> hex >> address;
        file >> dec >> size;
        if (operation == "load")
            load(address, size);
        else
        {
            string data;
            file >> data;
            store(address, data, size);
        }
    }
    return 0;
}
