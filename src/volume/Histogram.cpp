#include "Histogram.h"
#include <algorithm>

Histogram::Histogram(int min, int max, int numBins)
{
    numBins = std::min(max - min, numBins);

    this->min = min;
    this->max = max;
    this->numBins = numBins;
    this->binWidth = (double)(max - min) / numBins;
    bins = new unsigned int[numBins];
}

Histogram::~Histogram()
{
    delete[] bins;
}

template <typename T>
void Histogram::readData(T* data, int numElements)
{
    for (int i = 0; i < numElements; i++)
        bins[binIndex(*data)]++;
}

template <typename T>
int Histogram::binIndex(T value)
{
    return std::min(numBins - 1, (int)((value - min) / binWidth));
}

void Histogram::clearBins()
{
    for (int i = 0; i < numBins; i++)
        bins[i] = 0;
}

unsigned int* Histogram::getBins()
{
    return bins;
}

unsigned int Histogram::getSize(int bin)
{
    return bins[bin];
}

int Histogram::getMin()
{
    return min;
}

int Histogram::getMax()
{
    return max;
}

int Histogram::getNumBins()
{
    return numBins;
}