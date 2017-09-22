#ifndef DEF_PHYSIC
#define DEF_PHYSIC

#include <cmath>
#include <vector>
#include "object/Object.h"
#include "object/shapes/AABB.h"
#include "object/shapes/Circle.h"

struct Manifold
{
    Object* A;
    Object* B;
    double penetrationDepth;
    Vec2 normal;
};

void resolveCollision(Object* A, Object* B);
void positionCorrection(Object* A, Object* B);
bool collisionResolver(Manifold* m, AABB* a, AABB* b);
bool collisionResolver(Manifold* m, Circle* a, Circle* b);
bool collisionResolver(Manifold* m, AABB* a, Circle* b);
bool collisionResolver(Manifold* m, Circle* a, AABB* b);
double clamp( double min_extent, double max_extent, double closest);
void addObject(Object* O);
void removeObject(Object* O);

namespace physic{
    void update();
    void draw();
}

#endif