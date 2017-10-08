#ifndef DEF_POLYGON
#define DEF_POLYGON

#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>
#include <vector>
#include "../../utils/Vec2.h"
#include "Shape.h"

class Poly: public Shape
{
    std::vector<Vec2 *> vertexs;
    int type;

public:
    Poly();
    Poly(const Poly& p);
    Poly(std::vector<Vec2 *> vertexs);

    std::vector<Vec2 *> getVertexs() const;
    virtual int getType() const;
    virtual void draw(GLuint textureID, double textureZoom) const;
};

#endif