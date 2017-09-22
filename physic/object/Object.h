#ifndef DEF_OBJECT
#define DEF_OBJECT

#include "shapes/Shape.h"
#include "../utils/Vec2.h"

class Object
{
    double mass;
    double invMass;
    double restitution;
    Shape* shape;
    bool isStatic;
    Vec2 position;
    Vec2 velocity;
    double rot;
    double rotVelocity;

public:
    Object(double mass, double restitution, Shape* shape, bool isStatic);
    Vec2& getVelocity();
    Vec2& getPosition();
    double getRestitution() const;
    double getMass() const;
    double getInvMass() const;
    double& setMass(double& _mass);
    void push(const Vec2& v);
    void move(const Vec2& v);
    void draw();

};

#endif