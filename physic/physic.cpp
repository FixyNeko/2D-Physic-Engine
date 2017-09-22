#include "physic.h"
#include "object/Object.h"
#include "utils/Vec2.h"
#include <iostream>

std::vector<Object*> scene;

void resolveCollision(Object* A, Object* B){
    //impulse resolution
    Vec2 normal; //from A to B
    double peneDepth;
    Vec2 rVel = A->getVelocity() - B->getVelocity();
    double normalVelocity = dot(rVel, normal);

    if(normalVelocity < 0)
        return;
    
    double e = std::min(A->getRestitution(), B->getRestitution()); //restitution
    double j = -(1+e) * normalVelocity; //impulse scalar
    j /= A->getInvMass() + B->getInvMass();

    //Apply impulse
    Vec2 impulse = normal * j;
    A->push(-impulse * A->getInvMass());
    B->push(impulse * B->getInvMass());
}

void positionCorrection(Object* A, Object* B){
    const float percent = 0.2; // Usually 20% - 80%
    const float treshold = 0.01; // Usually 0.1 to 0.1
    if(penetrationDepth < treshold) return;
    Vec2 correction = normal * (penetrationDepth * percent / (A->getInvMass() + B->getInvMass()));
    A->move(correction * -(A->getInvMass());
    B->move(correction * B->getInvMass());
}

void collisionDetection(Manifold* m){
    //return collision normal Vec2 and penetration depth double

}

void addObject(Object* O){
    scene.push_back(O);
}

void removeObject(Object* O){
}

namespace physic{
    void draw(){
        for(int i = 0; i < scene.size(); i++)
            scene[i]->draw();
        std::cout << "physic draw done" << std::endl << std::endl;
    }
}

//TODO take car of shape position (currently supposing centered)
bool collisionResolver(Manifold* m, AABB* abox, AABB* bbox){
    Object* A = m->A, B = m->B;
    Vec2 n = B->getPosition() - A->getPosition(); // from B to A

    //extents along x
    double a_extent = (abox->max.getX() - abox->min.getX()) / 2;
    double b_extent = (bbox->max.getX() - bbox->min.getX()) / 2;
    double x_overlap = a_extent + b_extent - abs(n.getX()); /*for SAT, distance beeteen boxes
                                                            is greather than dist_along boxes*/ 
    if(x_overlap > 0){ // SAT, not separed in x
        //extents along x
        a_extent = (abox->max.getY() - abox->min.getY()) / 2;
        b_extent = (bbox->max.getY() - bbox->min.getY()) / 2;
        double y_overlap = a_extent + b_extent - abs(n.getY()); //for SAT

        if(y_overlap > 0){ //SAT, nor in y
            Vec2 rVelocity = A->getVelocity() - B->getVelocity(); // find in which axis the collision is happening
            if(abs(rVelocity.getX()) > abs(rVelocity.getY())){ // aka where objects are coming from relatively
                if(n.getX() < 0) // points toward B
                    m->normal = Vec2(-1,0);
                else
                    m->normal = Vec2(1,0);
                m->penetrationDepth = x_overlap;
                return true
            }
            else{
                if(n.getY() < 0) // points toward B
                   m->normal = Vec2(0,-1);
                else
                    m->normal = Vec2(0,1);
                m->penetrationDepth = y_overlap;
                return true
            }
        }
    }
    return false;
}
bool collisionResolver(Manifold* m, Circle* a, Circle* b){
    Object* A = m->A, B = m->B;
    Vec2 n = B->getPosition() - A->getPosition(); // from A to B

    float r = a->getRadius() + b->getRadius();

    if(n.lengthSquared() > r*r)
        return false // no collision
    
    float d = n.length();

    if(d != 0){
        m->penetrationDepth = r - d;
        m->normal = n / d; //d is n length, return a unit vector
    }
    else{
        m->penetrationDepth = max(a->getRadius(), b->getRadius());
        m->normal = Vec(0, 1); //default direction
    }
    return true;
}
bool collisionResolver(Manifold* m, AABB* abox, Circle* b){
    Object* A = m->A, B = m->B;
    Vec n = B->getPosition() - A->getPosition();
    Vec closest = n; // Closest point on A to B's center (actually just dist beetween object's centers)

    double x_extent = (abox->max.getX() - abox->min.getX());
    double y_extent = (abox->max.getY() - abox->min.getY());

    closest = Vec(clamp(-x_extent, x_extent, closest.getX()),
                    clamp(-y_extent, y_extent, closest.getY()));

    bool inside = false; // spécial case if center of circle is in the box

    if( n == closest){
        inside = true;
        //clamp to closest side/extent
        if(abs(n.getX()) < abs(n.getY())){
            if(closest.getX() > 0)
                closest.setX(x_extent);
            else
                closest.setX(-x_extent);
        }
        else {
            if(closest.getY() > 0)
            closest.setY(y_extent);
        else
            closest.setY(-y_extent);
        }
    }

    Vec2 normal = n - closest; // not normalised atm, represents distance from b center to closest point
    double d = normal.lengthSquared();
    double r = b->getRadius();
    
    if(d > r*r && !inside) // when inside, you want to push no matter what
        return false;
    
    d = normal.length(); // sqrt(d) works too
    n->normalize();

    if(inside)
        m->normal = -n; // need to push the other way
    else
        m->normal = n;

    m->penetrationDepth = r - d;

    return true;
}
bool collisionResolver(Manifold* m, Circle* a, AABB* b){
    return collisionResolver(m, b, a);
}

void clamp( double min_extent, double max_extent, double closest){
    if(*closest > max_extent)
        return max_extent;
    if(*closest < min_extent)
        return min_extent;
    return closest;
}