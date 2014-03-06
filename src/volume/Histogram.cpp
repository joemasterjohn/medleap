#include "Histogram.h"
#include <algorithm>
#include <iostream>

Histogram::Histogram(int min, int max, int numBins)
{
    numBins = std::min(max - min + 1, numBins);
    
    this->min = min;
    this->max = max;
    this->numBins = numBins;
    this->binWidth = (double)(max - min - 1) / numBins;
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
    // TODO: display range for the bin in data values
    for (int i = 0; i < numBins; i++) {
        std::cout << "bin " << i << " [ " << getBinLower(i) << ", " << getBinUpper(i) << "] = " << getSize(i);
//        for (int j = 0; j < getSize(i); j++)
//            std::cout << "*";
        std::cout << std::endl;
    }
}

void Histogram::printVisual(int rows, int cols)
{
    int heights[cols];
    int maxHeight = 1;
    
    int binsPerCol = numBins / cols;
    for (int i = 0; i < cols; i++) {
        // column height will be sum of all covered bins
        heights[i] = 0;
        for (int j = 0; j < binsPerCol; j++) {
            heights[i] += getSize(j + i * binsPerCol);
        }
        if (heights[i] > maxHeight)
            maxHeight = heights[i];
    }
    
    // normalize heights
    for (int i = 0; i < cols; i++) {
        heights[i] = (double)heights[i] / maxHeight * rows;
    }
    
    for (int i = 0; i < rows; i++) {
        int curHeight = rows - i - 1;
        for (int j = 0; j < cols; j++) {
            if (heights[j] > curHeight)
                std::cout << "*";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
}