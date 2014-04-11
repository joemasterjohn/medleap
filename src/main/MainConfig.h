#ifndef __medleap__MainConfig__
#define __medleap__MainConfig__

#include "util/Config.h"

/** Main app config file stored in <HOME>/.medleap */
class MainConfig : public Config
{
public:
    MainConfig();
    ~MainConfig();
    
    static const std::string WORKING_DIR;
    static const std::string USE_SRGB;
    static const std::string MULTISAMPLING;
    static const std::string SAMPLES;
};

#endif /* defined(__medleap__MainConfig__) */
