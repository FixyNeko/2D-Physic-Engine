#ifndef DEF_SHAPE
#define DEF_SHAPE

#include <GL/gl.h>

#define _AABB 0
#define _CIRCLE 1
#define _POLYGON 2

class Shape
{
    int type;
public:
    virtual int getType() const;
    virtual void draw(GLuint textureID, double textureZoom) const;
};

#endif