#ifndef DEF_AABB
#define DEF_AABB

#include "../../utils/Vec2.h"
#include "Shape.h"

class AABB: public Shape
{
    Vec2 min, max;

public:
    AABB();
    AABB(double width, double height);
    AABB(double xMin, double yMin, double xMax, double yMax);
    AABB& translate(Vec2 v);
    AABB& rotate(double angle);
    virtual void draw() const;
};

#endif