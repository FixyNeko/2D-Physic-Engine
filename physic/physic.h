#ifndef DEF_PHYSIC
#define DEF_PHYSIC

#include <cmath>
#include <vector>
#include "object/Object.h"


void resolveCollision(Object* A, Object* B);
void positionCorrection(Object* A, Object* B);
void collisionDetection(Manifold* m);
bool collisionResolver(Manifold* m, AABB* a, AABB* b);
bool collisionResolver(Manifold* m, Circle* a, Circle* b);
bool collisionResolver(Manifold* m, AABB* a, Circle* b);
bool collisionResolver(Manifold* m, Circle* a, AABB* b);
void clamp( double min_extent, double max_extent, double closest);
void addObject(Object* O);
void removeObject(Object* O);

struct Manifold
{
    Object* A;
    Object* B;
    double penetrationDepth;
    Vec2 normal;
}

namespace physic{
    void update();
    void draw();
}

#endif