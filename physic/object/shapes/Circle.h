#ifndef DEF_CIRCLE
#define DEF_CIRCLE

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
    virtual int getType() const;
    virtual void draw() const;
};

#endif