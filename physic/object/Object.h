#ifndef DEF_OBJECT
#define DEF_OBJECT

#include "shapes/Shape.h"
#include "shapes/AABB.h"
#include "shapes/Circle.h"
#include "shapes/Polygon.h"
#include "../utils/Vec2.h"
#include <cmath>
#include <vector>

class Object
{
    double mass;
    double invMass;
    double inertia;
    double invInertia;
    double restitution;
    double staticFriction;
    double dynamicFriction;
    bool canMove;
    bool canRotate;
    Vec2 position;
    Vec2 velocity;
    double rotation;
    double rotationVelocity;

    GLuint textureID;
    double textureZoom;
    
    Shape* shape;
    Shape* broadShape;

public:
    Object(double mass, double inertia, double restitution, double staticFriction, double dynamicFriction, Shape* shape, bool canMove, bool canRotate, GLuint textureID, double textureZoom);
    Vec2& getVelocity();
    Vec2& getPosition();
    double getRotation();
    double getRotationVelocity();
    double getRestitution() const;
    double getStaticFriction() const;
    double getDynamicFriction() const;
    double getMass() const;
    double getInvMass() const;
    double getInvInertia() const;
    Shape* getShape();
    Shape* setShape(Shape* shape);
    Shape* getBroad();
    double& setMass(double& _mass);
    double& setInertia(double& _inertia);
    bool& setStatic(bool _canMove);
    bool& getCanMove();
    void addVelocity(const Vec2& v);
    void move(const Vec2& v);
    void addRotationVelocity(double rotationVelocity);
    void addRotation(double r);
    void draw();

};

#endif