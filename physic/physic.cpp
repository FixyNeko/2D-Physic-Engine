#include "physic.h"
#include "object/Object.h"
#include "utils/Vec2.h"
#include <iostream>
#include <cstdint>
#include <vector>
#include <GL/gl.h>

std::vector<Object *> scene;
std::vector<Manifold *> manifolds;

double scale = 10000 / 100;
double speed = 1;
Vec2 acceleration = Vec2(0, -9.8 * scale);

// AABB 0, CIRCLE 1, POLYGON 2
static bool (*const collisionResolverArray[3][3])(Manifold *m) =
    {{AABBvsAABB, AABBvsCIRCLE, AABBvsPOLYGON},
     {swap, CIRCLEvsCIRCLE, CIRCLEvsPOLYGON},
     {swap, swap, POLYGONvsPOLYGON}};


void resolveCollision(Manifold *m)
{
    Object *A = m->A;
    Object *B = m->B;
    //impulse resolution
    Vec2 rVel = A->getVelocity() - B->getVelocity();
    double normalVelocity = dot(rVel, m->normal);
    Vec2 tangeant = rVel - (m->normal * normalVelocity);
    tangeant.normalize();
    double tangeantVelocity = dot(rVel, tangeant);

    if (normalVelocity < 0)
        return;

    double e = std::min(A->getRestitution(), B->getRestitution()); //restitution
    double jn = (1 + e) * normalVelocity;                          //impulse scalar
    jn /= A->getInvMass() + B->getInvMass();

    double mu = sqrt(pow(A->getStaticFriction(), 2) + pow(B->getStaticFriction(), 2));
    double jt = (clamp(0, std::min(A->getStaticFriction(), B->getStaticFriction()), tangeantVelocity) != tangeantVelocity) ?
                clamp(0, std::min(A->getStaticFriction(), B->getStaticFriction()), tangeantVelocity) :
                std::min(A->getDynamicFriction(), B->getDynamicFriction());
    jt /= A->getInvMass() + B->getInvMass();

    //Apply impulse
    Vec2 impulse = m->normal * jn;
    A->push(-impulse * A->getInvMass());
    B->push(impulse * B->getInvMass());
    //Apply friction
    if(abs( jt ) > jn * mu)
        jt = -jn * sqrt(pow(A->getDynamicFriction(), 2) + pow(B->getDynamicFriction(), 2));

    Vec2 friction = tangeant * jt;
    A->push(-friction * A->getInvMass());
    B->push(friction * B->getInvMass());
}

void positionCorrection(Manifold *m)
{
    Object *A = m->A;
    Object *B = m->B;
    const float percent = 0.2;  // Usually 20% - 80%
    const float treshold = 0.1; // Usually 0.01 to 0.1 //counted in units, the biger your objects/gravity, the bigger this value
    if (m->penetrationDepth < treshold)
        return;
    Vec2 correction = m->normal * (m->penetrationDepth * percent / (A->getInvMass() + B->getInvMass()));
    A->move(correction * -(A->getInvMass()));
    B->move(correction * B->getInvMass());
}

void addObject(Object *O)
{
    for (int i = 0; i < scene.size(); i++)
    {
        Manifold *m = new Manifold();
        m->A = O;
        m->B = scene[i];
        manifolds.push_back(m);
    }
    scene.push_back(O);
}

void removeObject(Object *O)
{
}

namespace physic
{
void draw()
{
    for (int i = 0; i < scene.size(); i++)
        scene[i]->draw();
}

void update(int dt)
{
    double dts = ((double)dt) * speed / 1000;
    for (int i = 0; i < scene.size(); i++)
    {
        scene[i]->push(acceleration * dts);
        scene[i]->move(scene[i]->getVelocity() * dts);
    }
    
    for (int i = 0; i < manifolds.size(); i++)
    {
        Manifold *m = manifolds[i];
        Shape *a = m->A->getShape();
        Shape *b = m->B->getShape();

        if (collisionResolverArray[a->getType()][b->getType()](m))
        {
            resolveCollision(m);
            positionCorrection(m);
        }
    }
}
}

//TODO take care of shape position (currently supposing centered)
bool AABBvsAABB(Manifold *m)
{
    Object *A = m->A;
    Object *B = m->B;
    AABB *abox = (AABB *)A->getShape();
    AABB *bbox = (AABB *)B->getShape();
    Vec2 n = B->getPosition() - A->getPosition(); // from B to A

    //extents along x
    double a_extent = (abox->max.getX() - abox->min.getX()) / 2;
    double b_extent = (bbox->max.getX() - bbox->min.getX()) / 2;
    double x_overlap = a_extent + b_extent - abs(n.getX()); /*for SAT, distance beeteen boxes
                                                            is greather than dist_along boxes*/
    if (x_overlap > 0)
    { // SAT, not separed in x
        //extents along x
        a_extent = (abox->max.getY() - abox->min.getY()) / 2;
        b_extent = (bbox->max.getY() - bbox->min.getY()) / 2;
        double y_overlap = a_extent + b_extent - abs(n.getY()); //for SAT

        if (y_overlap > 0)
        {                                                         //SAT, nor in y
            Vec2 rVelocity = A->getVelocity() - B->getVelocity(); // find in which axis the collision is happening
            if (abs(rVelocity.getX()) > abs(rVelocity.getY()))
            {                     // aka where objects are coming from relatively
                if (n.getX() < 0) // points toward B
                    m->normal = Vec2(-1, 0);
                else
                    m->normal = Vec2(1, 0);
                m->penetrationDepth = x_overlap;

                return true;
            }
            else
            {
                if (n.getY() < 0) // points toward B
                    m->normal = Vec2(0, -1);
                else
                    m->normal = Vec2(0, 1);
                m->penetrationDepth = y_overlap;

                return true;
            }
        }
    }
    return false;
}
bool AABBvsCIRCLE(Manifold *m)
{

    Object *A = m->A;
    Object *B = m->B;
    AABB *abox = (AABB *)A->getShape();
    Circle *b = (Circle *)B->getShape();
    Vec2 n = B->getPosition() - A->getPosition();
    Vec2 closest = n; // Closest point on A to B's center (actually just dist beetween object's centers)

    double x_extent = (abox->max.getX() - abox->min.getX());
    double y_extent = (abox->max.getY() - abox->min.getY());

    closest = Vec2(clamp(-x_extent / 2, x_extent / 2, closest.getX()),
                   clamp(-y_extent / 2, y_extent / 2, closest.getY()));

    bool inside = false; // spécial case if center of circle is in the box

    if (n == closest)
    {
        inside = true;
        //clamp to closest side/extent
        if (abs(n.getX()) < abs(n.getY()))
        {
            if (closest.getX() > 0)
                closest.setX(x_extent / 2);
            else
                closest.setX(-x_extent / 2);
        }
        else
        {
            if (closest.getY() > 0)
                closest.setY(y_extent / 2);
            else
                closest.setY(-y_extent / 2);
        }
    }

    Vec2 normal = n - closest; // not normalised atm, represents distance from b center to closest point
    double d = normal.lengthSquared();
    double r = b->getRadius();

    if (d > r * r && !inside) // when inside, you want to push no matter what
        return false;
    d = normal.length(); // sqrt(d) works too

    if (inside)
        m->normal = normal / -d;
    else
        m->normal = normal / d;
    m->penetrationDepth = r - d;

    return true;
}
bool AABBvsPOLYGON(Manifold *m)
{
    return false;
}
bool CIRCLEvsCIRCLE(Manifold *m)
{
    Object *A = m->A;
    Object *B = m->B;
    Circle *a = (Circle *)A->getShape();
    Circle *b = (Circle *)B->getShape();
    Vec2 n = B->getPosition() - A->getPosition(); // from A to B

    float r = a->getRadius() + b->getRadius();

    if (n.lengthSquared() > r * r)
        return false; // no collision

    float d = n.length();

    if (d != 0)
    {
        m->penetrationDepth = r - d;
        m->normal = n / d; //d is n length, return a unit vector
    }
    else
    {
        m->penetrationDepth = std::max(a->getRadius(), b->getRadius());
        m->normal = Vec2(0, 1); //default direction
    }
    return true;
}
bool CIRCLEvsPOLYGON(Manifold *m)
{
    return false;
}
bool POLYGONvsPOLYGON(Manifold *m)
{
    Object* A = m->A;
    Object* B = m->B;
    Poly* p1 = (Poly*) A->getShape();
    Poly* p2 = (Poly*) B->getShape();
    std::vector<Vec2 *> a = p1->getVertexs();
    std::vector<Vec2 *> b = p2->getVertexs();

    m->penetrationDepth = 9999999999999999999999999999999999999.; // max value possible
    //check SAT normal to all axis of A
    for (int i = 0; i < a.size(); i++)
    {

        Vec2 AB = *a[(i + 1) % a.size()] - *a[i];
        double angle = 0; // used to rotate all vertexs, in order project on x axis
        if (AB.getY() < 0)
        {
            angle = -std::acos(AB.getX() / AB.length()) * 180 / M_PI;
        }
        else if (AB.getY() > 0)
        {
            angle = std::acos(AB.getX() / AB.length()) * 180 / M_PI;
        }
        else
        {
            angle = (AB.getX() < 0) ? 180 : 0;
        }
        angle -= 90.; // because we project on a normal line to every side
        

        Vec2 Apos = A->getPosition();
        Vec2 Bpos = B->getPosition();

        Vec2 v = (*a[0] + Apos);
        v.rotate(-angle);
        double maxA = v.getX();
        double minA = maxA;
        for (int j = 1; j < a.size(); j++)
        {
            v = (*a[j] + Apos); // rotate every vertex to project on x axis, -angle to go fom cur. pos to axis (angle is from axis)
            v.rotate(-angle);
            minA = (v.getX() < minA) ? v.getX() : minA;
            maxA = (v.getX() > maxA) ? v.getX() : maxA;
        }

        v = (*b[0] + Bpos);
        v.rotate(-angle);
        double maxB = v.getX();
        double minB = maxB;
        for (int j = 1; j < b.size(); j++)
        {
            v = (*b[j] + Bpos);
            v.rotate(-angle);
            minB = (v.getX() < minB) ? v.getX() : minB;
            maxB = (v.getX() > maxB) ? v.getX() : maxB;
        }

        double overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < 0)
            return false; // this axis separates the polygons, not touching

        if (overlap < m->penetrationDepth)
        {
            m->penetrationDepth = overlap; // searching least axis of penetration
            double moyA = (minA + maxA) / 2;
            double moyB = (minB + maxB) / 2;
            m->normal = Vec2(1, 0).rotate( (moyA < moyB)? angle : angle + 180 ); // normal to the line, depends of relative position of objects
        }
    }

    //check SAT normal to all axis of B
    for (int i = 0; i < b.size(); i++)
    {
        Vec2 AB = *b[(i + 1) % b.size()] - *b[i];
        double angle = 0; // used to rotate all vertexs, in order project on x axis
        if (AB.getY() < 0)
        {
            angle = -std::acos(AB.getX() / AB.length()) * 180 / M_PI;
        }
        else if (AB.getY() > 0)
        {
            angle = std::acos(AB.getX() / AB.length()) * 180 / M_PI;
        }
        else
        {
            angle = (AB.getX() < 0) ? 180 : 0;
        }
        angle -= 90.; // because we project on a normal line to every side

        Vec2 Apos = A->getPosition();
        Vec2 Bpos = B->getPosition();

        Vec2 v = (*a[0] + Apos);
        v.rotate(-angle);
        double maxA = v.getX();
        double minA = maxA;
        for (int j = 1; j < a.size(); j++)
        {
            v = (*a[j] + Apos); // rotate every vertex to project on x axis, -angle to go fom cur. pos to axis (angle is from axis)
            v.rotate(-angle);
            minA = (v.getX() < minA) ? v.getX() : minA;
            maxA = (v.getX() > maxA) ? v.getX() : maxA;
        }

        v = (*b[0] + Bpos);
        v.rotate(-angle);
        double maxB = v.getX();
        double minB = maxB;
        for (int j = 1; j < b.size(); j++)
        {
            v = (*b[j] + Bpos);
            v.rotate(-angle);
            minB = (v.getX() < minB) ? v.getX() : minB;
            maxB = (v.getX() > maxB) ? v.getX() : maxB;
        }

        double overlap = std::min(maxA, maxB) - std::max(minA, minB);

        if (overlap < 0)
            return false; // this axis separates the polygons, not touching

        if (overlap < m->penetrationDepth)
        {
            m->penetrationDepth = overlap; // searching least axis of penetration
            double moyA = (minA + maxA) / 2;
            double moyB = (minB + maxB) / 2;
            m->normal = Vec2(1, 0).rotate( (moyA < moyB)? angle : angle + 180 ); // normal to the line, depends of relative position of objects
        }
    }
    return true; // no separate axis found, colliding, m was updated
}
bool swap(Manifold *m)
{
    Object *A = m->A;
    m->A = m->B;
    m->B = A;
    return collisionResolverArray[m->A->getShape()->getType()]
                                 [m->B->getShape()->getType()](m);
}

double clamp(double mini, double maxi, double value)
{
    if (value > maxi)
        return maxi;
    if (value < mini)
        return mini;
    return value;
}