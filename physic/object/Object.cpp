#include "Object.h"
#include <iostream>
#include <GL/gl.h>

Object::Object(double _mass, double _inertia, double _restitution, double _staticFriction, double _dynamicFriction, Shape* _shape, bool _isStatic):
    restitution(_restitution), staticFriction(_staticFriction), dynamicFriction(_dynamicFriction), shape(_shape), isStatic(_isStatic){
        this->setMass(_mass);
        this->setInertia(_inertia);

        position = velocity = Vec2();
        rotation = rotationVelocity = 0;
}

Vec2& Object::getVelocity(){
    return velocity;
}

Vec2& Object::getPosition(){
    return position;
}

double Object::getRotation(){
    return rotation;
}

double Object::getRotationVelocity(){
    return rotationVelocity;
}

double Object::getRestitution() const{
    return restitution;
}

double Object::getStaticFriction() const{
    return staticFriction;
}
double Object::getDynamicFriction() const{
    return dynamicFriction;
}

double Object::getMass() const{
    return mass;
}

double Object::getInvMass() const{
    return invMass;
}

double Object::getInvInertia() const{
    return invInertia;
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

double& Object::setInertia(double& _inertia){
    inertia = _inertia;
    if( _inertia == 0)
        invInertia = 0;
    else
        invInertia = 1/_inertia;
    return inertia;
}

bool& Object::setStatic(bool _isStatic){
    isStatic = _isStatic;
    return isStatic;
}

bool& Object::getStatic(){
    return isStatic;
}

void Object::addVelocity(const Vec2& v){
    if(isStatic) return;
    Vec2 w = v;
    velocity += w;
}

void Object::move(const Vec2& v){
    if(isStatic) return;
    Vec2 w = v;
    position += w;
}

void Object::addRotationVelocity(double r){
    if(isStatic) return;
    rotationVelocity += r;
}

void Object::addRotation(double r){
    if(isStatic) return;
    rotation += r;
}

void Object::draw(){
    glPushMatrix();
    glTranslated(position.getX(), position.getY(), 0);
    glRotated(rotation * 180 / M_PI, 0, 0, 1);
    shape->draw();
    glPopMatrix();
}