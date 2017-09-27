#ifndef DEF_PHYSIC
#define DEF_PHYSIC

#include <cmath>
#include <vector>
#include "object/Object.h"
#include "object/shapes/AABB.h"
#include "object/shapes/Circle.h"
#include "object/shapes/Polygon.h"

struct Manifold
{
    Object* A;
    Object* B;
    double penetrationDepth;
    Vec2 normal;
};

void resolveCollision(Manifold* m);
void positionCorrection(Manifold* m);

bool AABBvsAABB(Manifold* m);
bool AABBvsCIRCLE(Manifold* m);
bool AABBvsPOLYGON(Manifold* m);
bool CIRCLEvsCIRCLE(Manifold* m);
bool CIRCLEvsPOLYGON(Manifold* m);
bool POLYGONvsPOLYGON(Manifold* m);

bool swap(Manifold* m);
double clamp( double min_extent, double max_extent, double closest);
void addObject(Object* O);
void removeObject(Object* O);

namespace physic{
    void update(int dt);
    void draw();
}

#endif