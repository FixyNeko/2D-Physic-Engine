#include "Object.h"
#include <iostream>

Object::Object(double _mass, double _restitution, Shape* _shape, bool _isStatic):
    restitution(_restitution), shape(_shape), isStatic(_isStatic){
        this->setMass(_mass);
        position = velocity = Vec2();
        rot = rotVelocity = 0;
}

Vec2& Object::getVelocity(){
    return velocity;
}

Vec2& Object::getPosition(){
    return position;
}

double Object::getRestitution() const{
    return restitution;
}

double Object::getMass() const{
    return mass;
}

double Object::getInvMass() const{
    return invMass;
}

double& Object::setMass(double& _mass){
    mass = _mass;
    if( _mass == 0)
        invMass = 0;
    else
        invMass = 1/_mass;
    return mass;
}

void Object::push(const Vec2& v){
    if(isStatic) return;
    Vec2 w = v;
    velocity += w;
}

void Object::move(const Vec2& v){
    if(isStatic) return;
    Vec2 w = v;
    position += w;
}

void Object::draw(){
    
    shape->draw();
    std::cout << "Object draw done" << std::endl;
}