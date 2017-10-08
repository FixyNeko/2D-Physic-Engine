#include "physic.h"
#include "object/Object.h"
#include "utils/Vec2.h"
#include <iostream>
#include <cstdint>
#include <vector>
#include <GL/gl.h>
#include <SDL2/SDL.h>

std::vector<Object *> scene;
std::vector<Manifold *> manifolds;

double scale = 10000 / 100;
double speed = 0.7;
Vec2 acceleration = Vec2(0, -9.8 * scale);

// AABB 0, CIRCLE 1, POLYGON 2
static bool (*const collisionResolverArray[3][3])(Manifold *m) =
    {{AABBvsAABB, AABBvsCIRCLE, AABBvsPOLYGON},
     {swap, CIRCLEvsCIRCLE, CIRCLEvsPOLYGON},
     {swap, swap, POLYGONvsPOLYGON}};


void resolveCollision(Manifold *m)
{
    glEnable(GL_POINT_SIZE);
    glPointSize(5);
    glBegin(GL_POINTS);
        glVertex2d(m->contactPosition.getX(), m->contactPosition.getY());
    glEnd();

    Object *A = m->A;
    Object *B = m->B;
    //impulse resolution
    Vec2 ArotationVelPos = m->contactPosition - A->getPosition();
    Vec2 BrotationVelPos = m->contactPosition - B->getPosition();
    Vec2 ArotationVel = cross(A->getRotationVelocity(), ArotationVelPos);
    Vec2 BrotationVel = cross(B->getRotationVelocity(), BrotationVelPos);
    Vec2 rVel = A->getVelocity() + ArotationVel - B->getVelocity() - BrotationVel;
    double normalVelocity = dot(rVel, m->normal);
    Vec2 tangeant = rVel - (m->normal * normalVelocity);
    tangeant.normalize();
    double tangeantVelocity = dot(rVel, tangeant);

    if (normalVelocity < 0)
        return;

    double e = std::min(A->getRestitution(), B->getRestitution()); //restitution
    double jn = (1 + e) * normalVelocity;                          //impulse scalar
    jn /= A->getInvMass() + B->getInvMass() + pow(cross(ArotationVelPos, m->normal), 2) * A->getInvInertia() + pow(cross(BrotationVelPos, m->normal), 2) * B->getInvInertia();

    double mu = sqrt(pow(A->getStaticFriction(), 2) + pow(B->getStaticFriction(), 2));
    double jt = (clamp(0, mu, tangeantVelocity) == tangeantVelocity) ?
                clamp(0, mu, tangeantVelocity) :
                sqrt(pow(A->getDynamicFriction(), 2) + pow(B->getDynamicFriction(), 2));
    jt /= A->getInvMass() + B->getInvMass() + pow(cross(ArotationVelPos, tangeant), 2) * A->getInvInertia() + pow(cross(BrotationVelPos, tangeant), 2) * B->getInvInertia();

    //Apply impulse
    Vec2 impulse = m->normal * jn;
    A->addVelocity(-impulse * A->getInvMass());
    B->addVelocity(impulse * B->getInvMass());
    //Apply friction
    if(abs( jt ) > jn * mu)
        jt = -jn * sqrt(pow(A->getDynamicFriction(), 2) + pow(B->getDynamicFriction(), 2));

    Vec2 friction = tangeant * jt;
    A->addVelocity(-friction * A->getInvMass());
    B->addVelocity(friction * B->getInvMass());
    //apply rotation
    A->addRotationVelocity(cross(ArotationVelPos, impulse) * -A->getInvInertia());
    B->addRotationVelocity(cross(BrotationVelPos, impulse) * B->getInvInertia());

    // rotation with friction vector, test
    A->addRotationVelocity(cross(ArotationVelPos, friction) * -A->getInvInertia());
    B->addRotationVelocity(cross(BrotationVelPos, friction) * B->getInvInertia());
}

void positionCorrection(Manifold *m)
{
    Object *A = m->A;
    Object *B = m->B;
    const float percent = 0.8;  // Usually 20% - 80%
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
    int startTime = SDL_GetTicks();

    double dts = ((double)dt) * speed / 1000;
    for (int i = 0; i < scene.size(); i++)
    {
        scene[i]->addVelocity(acceleration * dts);
        scene[i]->move(scene[i]->getVelocity() * dts);

        scene[i]->addRotation(scene[i]->getRotationVelocity() * dts);
    }
    
    for (int i = 0; i < manifolds.size(); i++)
    {
        Manifold *m = manifolds[i];
        Shape *a = m->A->getShape();
        Shape *b = m->B->getShape();

        if ((m->A->canMove() || m->B->canMove()) && broadPhase(m) && collisionResolverArray[a->getType()][b->getType()](m))
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

    bool inside = false; // special case if center of circle is in the box

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
    m->contactPosition = closest;

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
    Vec2 aPos = a->getPosition();
    Vec2 bPos = b->getPosition();
    Vec2 n = B->getPosition() + bPos - A->getPosition() - aPos; // from A to B

    float r = a->getRadius() + b->getRadius();

    if (n.lengthSquared() > r * r)
        return false; // no collision

    float d = n.length();

    if (d != 0)
    {
        m->penetrationDepth = r - d;
        m->normal = n / d; //d is n length, return a unit vector
        m->contactPosition = n / d * a->getRadius() + A->getPosition();
    }
    else
    {
        m->penetrationDepth = std::max(a->getRadius(), b->getRadius());
        m->normal = Vec2(0, 1); //default direction
        m->contactPosition = A->getPosition();
    }
    return true;
}
bool CIRCLEvsPOLYGON(Manifold *m)
{
    Object* A = m->A;
    Object* B = m->B;
    Circle* circleShape = (Circle*) A->getShape();
    Poly* polyShape = (Poly*) B->getShape();
    std::vector<Vec2 *> vertexs = polyShape->getVertexs();

    m->penetrationDepth = 99999999999999999999999999999999999.;

    for (int i = 0; i < vertexs.size(); i++)
    {
        Vec2 AB = *vertexs[(i + 1) % vertexs.size()] - *vertexs[i];
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
        angle += B->getRotation() * 180 / M_PI;
        
        Vec2 Bpos = B->getPosition();

        Vec2 v = *vertexs[0];
        v.rotate(B->getRotation() * 180 / M_PI);
        v += Bpos;
        v.rotate(-angle);
        double maxB = v.getX();
        double minB = maxB;
        for (int j = 1; j < vertexs.size(); j++)
        {
            v = *vertexs[j]; // rotate every vertex to project on x axis, -angle to go fom cur. pos to axis (angle is from axis)
            v.rotate(B->getRotation() * 180 / M_PI);
            v += Bpos;
            v.rotate(-angle);
            minB = (v.getX() < minB) ? v.getX() : minB;
            maxB = (v.getX() > maxB) ? v.getX() : maxB;
        }

        Vec2 circlePos = circleShape->getPosition();
        circlePos.rotate(A->getRotation() * 180 / M_PI);
        circlePos += A->getPosition();
        circlePos.rotate(-angle);
        double maxA = circlePos.getX() + circleShape->getRadius();
        double minA = maxA - 2 * circleShape->getRadius();

        double overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < 0)
            return false; // this axis separates the polygons, not touching

        if (overlap < m->penetrationDepth) // we found a new minimum axis of penetration, so it's this new point which is coliding
        {
            m->penetrationDepth = overlap;
            double moyA = circlePos.getX();
            double moyB = (minB + maxB) / 2;
            m->normal = Vec2(1, 0).rotate( (moyA < moyB)? angle : angle + 180 ); // normal to the line, depends of relative position of objects
            
            // contact position is the Y coord of the circle, since the edge colliding is vertical
            Vec2 contact = circlePos.rotate(angle);
            Vec2 a = m->normal * circleShape->getRadius();
            contact += a;
            m->contactPosition = contact;
        }
    }
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
        angle += A->getRotation() * 180 / M_PI;
        

        Vec2 Apos = A->getPosition();
        Vec2 Bpos = B->getPosition();

        Vec2 v = *a[0];
        v.rotate(A->getRotation() * 180 / M_PI);
        v += Apos;
        v.rotate(-angle);
        double maxA = v.getX();
        double minA = maxA;
        for (int j = 1; j < a.size(); j++)
        {
            v = *a[j]; // rotate every vertex to project on x axis, -angle to go fom cur. pos to axis (angle is from axis)
            v.rotate(A->getRotation() * 180 / M_PI);
            v += Apos;
            v.rotate(-angle);
            minA = (v.getX() < minA) ? v.getX() : minA;
            maxA = (v.getX() > maxA) ? v.getX() : maxA;
        }

        v = *b[0];
        v.rotate(B->getRotation() * 180 / M_PI);
        v += Bpos;
        v.rotate(-angle);
        double maxB = v.getX();
        double minB = maxB;
        for (int j = 1; j < b.size(); j++)
        {
            v = *b[j];
            v.rotate(B->getRotation() * 180 / M_PI);
            v += Bpos;
            v.rotate(-angle);
            minB = (v.getX() < minB) ? v.getX() : minB;
            maxB = (v.getX() > maxB) ? v.getX() : maxB;
        }

        double overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < 0)
            return false; // this axis separates the polygons, not touching

        if (overlap < m->penetrationDepth) // we found a new minimum axis of penetration, so it's this new point which is coliding
        {
            m->penetrationDepth = overlap;
            double moyA = (minA + maxA) / 2;
            double moyB = (minB + maxB) / 2;
            m->normal = Vec2(1, 0).rotate( (moyA < moyB)? angle : angle + 180 ); // normal to the line, depends of relative position of objects
            
            Vec2 min = *b[0];
            min.rotate(B->getRotation() * 180 / M_PI);
            Vec2 minCopy = min;
            minCopy.rotate(-angle); // reduce calculs when no new min
            // brute searching the minimum x point for collision coordinates
            for(int j = 1; j < b.size(); j++){ // point colliding is on B, since it collide into A line 
                Vec2 v = *b[j];
                v.rotate(B->getRotation() * 180 / M_PI);
                v.rotate(-angle);
                if(moyA < moyB){ // depending of the side b is hitting, we need maxX or minX
                    if(v.getX() < minCopy.getX()){
                        min = *b[j];
                        min.rotate(B->getRotation() * 180 / M_PI);
                        minCopy = min;
                        minCopy.rotate(-angle);
                    }
                }
                else{
                    if(v.getX() > minCopy.getX()){
                        min = *b[j];
                        min.rotate(B->getRotation() * 180 / M_PI);
                        minCopy = min;
                        minCopy.rotate(-angle);
                    }
                }
            }

            min += B->getPosition();
            m->contactPosition = min;
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
        angle += B->getRotation() * 180 / M_PI;

        Vec2 Apos = A->getPosition();
        Vec2 Bpos = B->getPosition();

        Vec2 v = *a[0];
        v.rotate(A->getRotation() * 180 / M_PI);
        v += Apos;
        v.rotate(-angle);
        double maxA = v.getX();
        double minA = maxA;
        for (int j = 1; j < a.size(); j++)
        {
            v = *a[j]; // rotate every vertex to project on x axis, -angle to go fom cur. pos to axis (angle is from axis)
            v.rotate(A->getRotation() * 180 / M_PI);
            v += Apos;
            v.rotate(-angle);
            minA = (v.getX() < minA) ? v.getX() : minA;
            maxA = (v.getX() > maxA) ? v.getX() : maxA;
        }

        v = *b[0];
        v.rotate(B->getRotation() * 180 / M_PI);
        v += Bpos;
        v.rotate(-angle);
        double maxB = v.getX();
        double minB = maxB;
        for (int j = 1; j < b.size(); j++)
        {
            v = *b[j];
            v.rotate(B->getRotation() * 180 / M_PI);
            v += Bpos;
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

            Vec2 min = *a[0];
            min.rotate(A->getRotation() * 180 / M_PI);
            Vec2 minCopy = min;
            minCopy.rotate(-angle); // reduce calculs when no new min
            // brute searching the minimum x point for collision coordinates
            for(int j = 1; j < a.size(); j++){ // point colliding is on A, since it collide into B line 
                Vec2 v = *a[j];
                v.rotate(A->getRotation() * 180 / M_PI);
                v.rotate(-angle);
                if(moyA > moyB){ // depending of the side a is hitting, we need maxX or minX
                    if(v.getX() < minCopy.getX()){
                        min = *a[j];
                        min.rotate(A->getRotation() * 180 / M_PI);
                        minCopy = min;
                        minCopy.rotate(-angle);
                    }
                }
                else{
                    if(v.getX() > minCopy.getX()){
                        min = *a[j];
                        min.rotate(A->getRotation() * 180 / M_PI);
                        minCopy = min;
                        minCopy.rotate(-angle);
                    }
                }
            }

            min += A->getPosition();
            m->contactPosition = min;
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

bool broadPhase(Manifold* m){
    Shape* Ashape = m->A->getShape();
    Shape* Bshape = m->B->getShape();

    m->A->setShape(m->A->getBroad());
    m->B->setShape(m->B->getBroad());

    bool broadPhasePassed = true;

    m->A->draw();
    m->B->draw();

    if(!collisionResolverArray[m->A->getShape()->getType()][m->B->getShape()->getType()](m))
        broadPhasePassed = false;

    m->A->setShape(Ashape);
    m->B->setShape(Bshape);

    return broadPhasePassed;
}

double clamp(double mini, double maxi, double value)
{
    if (value > maxi)
        return maxi;
    if (value < mini)
        return mini;
    return value;
}