#include "Histogram.h"
#include <algorithm>
#include <iostream>

Histogram::Histogram(int min, int max, int numBins)
{
    numBins = std::min(max - min + 1, numBins);
    
    this->min = min;
    this->max = max;
    this->numBins = numBins;
    this->binWidth = (double)(max - min + 1) / numBins;
    bins = new unsigned int[numBins];
    clearBins();
}

Histogram::~Histogram()
{
    delete[] bins;
}

void Histogram::clearBins()
{
    std::fill(bins, bins+numBins, 0);
    maxFrequency = 0;
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

int Histogram::getMaxFrequency()
{
    return maxFrequency;
}

int Histogram::getNumBins()
{
    return numBins;
}

double Histogram::getBinLower(int binIndex)
{
    return min + binIndex * binWidth;
}

double Histogram::getBinUpper(int binIndex)
{
    return min + (binIndex + 1) * binWidth;
}

void Histogram::print()
{
    for (int i = 0; i < numBins; i++) {
        std::cout << "bin " << i << " [ " << getBinLower(i) << ", " << getBinUpper(i) << "] = " << getSize(i);
        std::cout << std::endl;
    }
}