#ifndef __MEDLEAP_HISTOGRAM__
#define __MEDLEAP_HISTOGRAM__

#include <algorithm>

class Histogram
{
public:
    Histogram(int min, int max, int numBins);
    ~Histogram();
    
    /** Updates the histogram with data values */
    template <typename T> void readData(T* data, int numElements);
    
    /** Sets all bins to 0 and resets the max frequency */
    void clearBins();
    
    /** Returns the lower bound of a bin */
    double getBinLower(int binIndex);
    
    /** Returns the upper bound of a bin */
    double getBinUpper(int binIndex);
    
    /** Pointer to the raw data */
    unsigned int* getBins();
    
    /** Returns the number of values (frequency) in a given bin */
    unsigned int getSize(int binIndex);
    
    /** Returns the smallest value accepted by the histogram */
    int getMin();
    
    /** Returns the largest value accepted by the histogram */
    int getMax();
    
    /** Returns getMax() - getMin() */
    int getRange();
    
    /** Returns the size of the largest bin */
    int getMaxFrequency();
    
    /** Returns number of bins in the histogram */
    int getNumBins();
    
    /** Prints a visual representation of histogram to stdout */
    void print();
    
private:
    int min;
    int max;
    int range;
    int maxFrequency;
    int numBins;
    double binWidth;
    unsigned int* bins;
};

template <typename T>
void Histogram::readData(T* data, int numElements)
{
    for (int i = 0; i < numElements; i++) {
        T value = *data++;
        if (value >= min && value <= max) {
            // determine which bin the value belongs to, increment the bin size, and check if it
            // is the largest size in the data set
            int binIndex = (int)((value - min) / binWidth);
            if (++bins[binIndex] > maxFrequency) {
                maxFrequency = bins[binIndex];
            }
        }
    }
}

#endif // __MEDLEAP_HISTOGRAM__

