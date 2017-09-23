#ifndef DEF_SHAPE
#define DEF_SHAPE

#define _AABB 0
#define _CIRCLE 1
#define _POLYGON 2

class Shape
{
    int type;
public:
    virtual int getType() const;
    virtual void draw() const;
};

#endif