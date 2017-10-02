#ifndef DEF_CIRCLE
#define DEF_CIRCLE

#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>
#include "../../utils/Vec2.h"
#include "Shape.h"

class Circle: public Shape
{
    Vec2 pos;
    double radius;
    int type;

public:
    Circle();
    Circle(double radius);
    Circle(Vec2 pos, double radius);

    Circle& translate(Vec2 v);
    double getRadius() const;
    Vec2& getPosition();
    virtual int getType() const;
    virtual void draw() const;
};

#endif