#ifndef CGL_VIEWPORT_H_
#define CGL_VIEWPORT_H_

namespace cgl
{
    class Viewport
    {
    public:
        int x;
        int y;
        int width;
        int height;
        
        Viewport();
        
        Viewport(int x, int y, int width, int height);
        
        /** Calculates aspect ratio (width / height). */
        float aspect() const;
        
        /** Applies the viewport using glViewport(). */
        void apply() const;
        
        bool operator==(const Viewport&) const;
        
        bool operator!=(const Viewport&) const;
        
        bool contains(int x, int y) const;
    };
    
} // namespace cgl

#endif // CGL_VIEWPORT_H_
