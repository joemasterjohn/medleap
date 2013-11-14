#include <iostream>
#include "gdcmImageReader.h"
#include "gdcmDirectoryHelper.h"

int main(int argc, char** argv)
{
/*
    if (argc != 2)
    {
        std::cerr << "Must provide a directory";
        return -1;
    }

    using namespace gdcm;

    Directory::FilenamesType series = DirectoryHelper::GetCTImageSeriesUIDs(argv[1]);

    std::cout << "CT Series:" << std::endl;
    for (int i = 0; i < series.size(); ++i)
    {
        std::cout << "Series " << i << " : " << series[i] << std::endl;
    }

    std::cout << "MR Series:" << std::endl;
    series = DirectoryHelper::GetMRImageSeriesUIDs(argv[1]);
    for (int i = 0; i < series.size(); ++i)
    {
        std::cout << "Series " << i << " : " << series[i] << std::endl;
        std::vector<DataSet> data = DirectoryHelper::LoadImageFromFiles(argv[1], series[i]);    
    }
*/
    gdcm::ImageReader reader;
    const char* filename = argv[1];
    reader.SetFileName(filename);  

    if (!reader.Read())
    {
        std::cerr << "Could not read: " << filename << std::endl;
        return -1;
    }

    gdcm::Image& img = reader.GetImage();

    img.Print(std::cout);

    std::cout << "Width:  " << img.GetColumns() << std::endl;
    std::cout << "Height: " << img.GetRows() << std::endl;

    char* buf = new char[115200];
    if (!img.GetBuffer(buf))
        std::cerr << "error reading buffer" << std::endl;

    

    return 0;
}
