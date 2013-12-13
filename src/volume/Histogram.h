#ifndef __MEDLEAP_HISTOGRAM__
#define __MEDLEAP_HISTOGRAM__

class Histogram
{
public:
    Histogram(int min, int max, int numBins);
    ~Histogram();
    
    template <typename T> void readData(T* data, int numElements);
    
    void clearBins();
    unsigned int* getBins();
    unsigned int getSize(int bin);
    int getMin();
    int getMax();
    int getNumBins();
    
private:
    int min;
    int max;
    int numBins;
    double binWidth;
    unsigned int* bins;
    
    template <typename T> int binIndex(T value);
};

#endif // __MEDLEAP_HISTOGRAM__