#ifndef DEF_OBJECT
#define DEF_OBJECT

#include "shapes/Shape.h"
#include "../utils/Vec2.h"

class Object
{
    double mass;
    double invMass;
    double inertia;
    double restitution;
    double staticFriction;
    double dynamicFriction;
    Shape* shape;
    bool isStatic;
    Vec2 position;
    Vec2 velocity;
    double rotation;
    double rotationVelocity;

public:
    Object(double mass, double inertia, double restitution, double staticFriction, double dynamicFriction, Shape* shape, bool isStatic);
    Vec2& getVelocity();
    Vec2& getPosition();
    double getRotation();
    double getRotationVelocity();
    double getRestitution() const;
    double getStaticFriction() const;
    double getDynamicFriction() const;
    double getMass() const;
    double getInvMass() const;
    Shape* getShape();
    double& setMass(double& _mass);
    bool& setStatic(bool _isStatic);
    void addVelocity(const Vec2& v);
    void move(const Vec2& v);
    void draw();

};

#endif