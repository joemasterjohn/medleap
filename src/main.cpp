#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "gl/shader.h"
#include "gl/program.h"
#include "gdcmImageReader.h"
#include "gdcmAttribute.h"
#include "gdcmTag.h"
#include "gdcmDirectoryHelper.h"
#include "gdcmScanner.h"
#include "gdcmIPPSorter.h"

GLFWwindow* window;
GLuint vao;
GLuint vbo;
Program* program;
GLuint texture = 0;
std::vector<std::string> imageFilesSorted;



// currently this will return a vector storing the file names for all MR images in a series
std::vector<std::string> findSeriesInDirectory(const char* dir)
{
    // look for the unique MR series UIDs in the DICOM files in the directory
    gdcm::Directory::FilenamesType series = gdcm::DirectoryHelper::GetMRImageSeriesUIDs(dir);
    if (series.size() == 0) {
        std::cerr << "No MR series found in directory: " << dir << std::endl;
        return std::vector<std::string>();
    }
    
    // just use first series for now
    std::string mySeries = series[0];

    // create a scanner that will match files by series UID
    gdcm::Scanner scanner;
    scanner.AddTag(gdcm::Tag(0x0020,0x000e));
    gdcm::Directory directory;
    directory.Load(dir);
    scanner.Scan(directory.GetFilenames());
    
    gdcm::Directory::FilenamesType seriesValues = scanner.GetOrderedValues(gdcm::Tag(0x0020,0x000e));
    size_t theNumSeries = seriesValues.size();
    
    for (size_t i = 0; i < theNumSeries; ++i) {
        if (mySeries == seriesValues[i]) {
            gdcm::Directory::FilenamesType files = scanner.GetAllFilenamesFromTagToValue(gdcm::Tag(0x0020,0x000e),
                                                                                         seriesValues[i].c_str());
            gdcm::IPPSorter sorter;
            sorter.SetComputeZSpacing(true);
            sorter.SetZSpacingTolerance(0.000001);
            if (!sorter.Sort(files)) {
                std::cerr << "problem sorting images" << std::endl;
                return std::vector<std::string>();
            }
            
            gdcm::Directory::FilenamesType sortedFiles = sorter.GetFilenames();
            return sortedFiles;
        }
    }
    
    return std::vector<std::string>();
}

void loadDICOM(const char* fileName)
{
    gdcm::ImageReader reader;
    reader.SetFileName(fileName);
    if (!reader.Read())
        std::cerr << "couldn't read dicom file" << std::endl;
    
    gdcm::Image& img = reader.GetImage();
    img.Print(std::cout);
    
    // I only support MR and CT images, which are grayscale in value.
    if (img.GetNumberOfDimensions() != 2) {
        std::cerr << "Only support 2D DICOM images." << std::endl;
    }
    
    gdcm::DataSet ds = reader.GetFile().GetDataSet();
    
    // determine the modality
    {
        gdcm::Attribute<0x0008, 0x0060> at;
        at.Set(ds);
        std::cout << "modality = " << at.GetValue() << std::endl;
    }
    
    double windowCenter;
    double windowWidth;
    
    // determine the window center
    {
        gdcm::Attribute<0x0028,0x1050> at;
        at.Set(ds);
        std::cout << "window center = " << at.GetValue() << std::endl;
        windowCenter = at.GetValue();
    }

    // determine the window center
    {
        gdcm::Attribute<0x0028,0x1051> at;
        at.Set(ds);
        std::cout << "window width = " << at.GetValue() << std::endl;
        windowWidth = at.GetValue();
    }
    
    int width = img.GetColumns();
    int height = img.GetRows();
    
    double inputMin = windowCenter - windowWidth / 2.0;
    double inputMax = windowCenter + windowWidth / 2.0;
    std::cout << "min,max = " << inputMin << ", " << inputMax << std::endl;
    
    
    // * 2 because it's UINT16, this is hard-coded and bad fix later
    char* inData = new char[width * height * 2];
    img.GetBuffer(inData);
    
    // only used in CT images:
    double intercept = img.GetIntercept();
    double slope = img.GetSlope();
    
    unsigned char* outData = new unsigned char[width * height];
    
    
    // assuming type UINT16; change this
    unsigned short* pixel = (unsigned short*)inData;
    for (int i = 0; i < width * height; i++) {
        // first apply the modality LUT
        signed short realWorldValue = (*pixel) * slope + intercept;
        
        // now apply the VOI LUT
        if (realWorldValue <= inputMin)
            outData[i] = 0;
        else if (realWorldValue >= inputMax)
            outData[i] = 255;
        else
        {
            double normalized = (realWorldValue - inputMin) / (inputMax - inputMin);
            outData[i] = normalized * 255;
        }
        
        pixel++;
    }
    
    // 1) Apply the Modality LUT
    //    This is a transformation from manufacturer-dependent pixel values to manufacturer-independent values. For example,
    //    Hounsfield Units for CT images. This is either a linear transformation using the tags Rescale Slope (0028,1053) and
    //    Rescale Intercept (0028,1052) or a non-linear transformation using a Modality LUT Sequence (0028,3000).
    
    // 2) Apply the VOI LUT
    //    This is a transformation from modality pixel values to print/display values. It is applied after any Modality LUT.
    //    For a linear transformation, the Window Center (0028,1050) and Window Width (0028,1051) values are used. A VOI LUT
    //    Sequence is used for non-linear transformations (0028,3010). There may be multiple window center and window width
    //    pairs.
    
    //  Load data into a byte buffer. Cast it as the size that its stored in the DICOM file while traversing. Using the size
    // of the window width to determine output pixel buffer size.
    
    
    if (!texture) {
        glGenTextures(1, &texture);
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Could try a better alignment based on data type to make this quicker
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 width,
                 height,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 outData);
    
    delete[] inData;
    delete[] outData;
}

void init()
{
    //loadDICOM();
    
    //loadBitmap();
    
    program = Program::create("shaders/simple1.vert",
                              "shaders/simple1.frag");
    program->enable();
    
    const GLubyte* str = glGetString(GL_VERSION);
    std::cout << "Version = " << str << std::endl;
    
    // create a VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // create a VBO
    GLfloat data[] = { -1, -1, 0, 0,
                        1, -1, 1, 0,
                        1,  1, 1, 1,
                       -1, -1, 0, 0,
                        1,  1, 1, 1,
                        -1,  1, 0, 1};
    
    GLsizei stride = 4 * sizeof(GLfloat);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    
    int loc = program->getAttribute("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = program->getAttribute("vTexCoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
}

void reshape(int width, int height)
{
    
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void keyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static int curImage = 0;
    if (key == GLFW_KEY_RIGHT) {
        curImage = (curImage + 1) % imageFilesSorted.size();
        std::cout << "display image: " << imageFilesSorted[curImage] << std::endl;
        loadDICOM(imageFilesSorted[curImage].c_str());
    }
}

bool initWindow(int width, int height, const char* title)
{
    if (!glfwInit())
        return false;
    
    // Use 32-bit color (in sRGB) and 24-bit for depth buffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    
    // Use OpenGL 3.2 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create a window
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    
	// Set window callback functions for events
    //glfwSetFramebufferSizeCallback(window, resizeCB);
	glfwSetKeyCallback(window, keyboardCB);
	//glfwSetMouseButtonCallback(window, mouseCB);
    //glfwSetCursorPosCallback(window, cursorCB);
    
    // Set vertical retrace rate (0 == run as fast as possible)
    //glfwSwapInterval(vsync);
    
    // Initialize GLEW, which provides access to OpenGL functions / extensions
	glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        glfwTerminate();
        return false;
    }
    
    // Let program initialize OpenGL resources
    init();
    reshape(width, height);
    
    // Start rendering loop
    while (!glfwWindowShouldClose(window)) {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return true;
}

int main(int argc, char** argv)
{
    imageFilesSorted = findSeriesInDirectory("/Users/justin/Projects/reach/testdata");
    initWindow(600, 600, "hello world");
    return 0;
}
