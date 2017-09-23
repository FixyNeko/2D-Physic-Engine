#include "Object.h"
#include <iostream>
#include <GL/gl.h>

Object::Object(double _mass, double _restitution, Shape* _shape, bool _isStatic):
    restitution(_restitution), shape(_shape), isStatic(_isStatic){
        this->setMass(_mass);
        position = velocity = Vec2();
        rotation = rotationVelocity = 0;
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

Shape* Object::getShape(){
    return shape;
}

double& Object::setMass(double& _mass){
    mass = _mass;
    if( _mass == 0)
        invMass = 0;
    else
        invMass = 1/_mass;
    return mass;
}

bool& Object::setStatic(bool _isStatic){
    isStatic = _isStatic;
    return isStatic;
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
    glPushMatrix();
    glTranslated(position.getX(), position.getY(), 0);
    glRotated(rotation, 0, 0, 1);
    shape->draw();
    glPopMatrix();
}